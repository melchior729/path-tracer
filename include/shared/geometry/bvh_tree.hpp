#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/geometry/sphere_hit.hpp"
#include "shared/math/aabb.hpp"
#include "shared/math/interval.hpp"
#include <algorithm>

struct BVHNode {
  AABB bbox;
  int left_i{-1};
  int right_i{-1};
  union {
    int sphere_i{-1};
    int split_axis;
  };

  bool HOSTDEV is_leaf() const { return left_i == -1 && right_i == -1; }
};

struct BVHTree {
  BVHNode *root;
  size_t size;

  BVHTree(std::vector<Sphere> &spheres) {
    size = spheres.size();
    root = static_cast<BVHNode *>(malloc((2 * size - 1) * sizeof(BVHNode)));
    fill_tree(spheres, 0, size);
  }

  BVHTree(const BVHTree &other) {
    size = other.size;
    root = static_cast<BVHNode *>(malloc((2 * size - 1) * sizeof(BVHNode)));
    std::copy(other.root, other.root + (2 * size - 1), root);
    open = other.open;
  }

  BVHTree &operator=(const BVHTree &other) {
    auto temp{other};
    std::swap(root, temp.root);
    size = temp.size;
    open = temp.open;
    return *this;
  }

  bool HOSTDEV hit(const Ray &ray, Interval interval, HitRecord *record,
                   const SphereBuffer *spheres) const {
    if (spheres->size <= 16) {
      return hit_spheres(spheres, ray, interval, record);
    }

    const auto origin{ray.origin()};
    const auto direction{ray.direction()};
    const Vec3 inverse_direction{1.0f / direction.x(), 1.0f / direction.y(),
                                 1.0f / direction.z()};

    // A balanced 25,000-leaf tree needs at most 16 pending node indices.
    // Indices keep this the same size as the previous eight-pointer stack.
    int stack[16];
    size_t sp{};

    bool hit{};
    stack[sp++] = 0;

    while (sp > 0) {
      auto node{root + stack[--sp]};
      if (!node->bbox.hit(origin, inverse_direction, interval)) {
        continue;
      }

      if (node->is_leaf()) {
        if (hit_sphere(spheres, ray, static_cast<size_t>(node->sphere_i),
                       interval, record)) {
          hit = true;
          interval.max = record->t;
        }
        continue;
      }

      auto near_i{node->left_i};
      auto far_i{node->right_i};
      if (direction[static_cast<size_t>(node->split_axis)] < 0.0f) {
        auto temp{near_i};
        near_i = far_i;
        far_i = temp;
      }

      // The stack is LIFO, so push the farther child first.
      stack[sp++] = far_i;
      stack[sp++] = near_i;
    }

    return hit;
  }

  ~BVHTree() { free(root); }

private:
  size_t open{};

  static constexpr size_t SAH_BUCKETS{12};

  constexpr void set_leaf(BVHNode *leaf, const std::vector<Sphere> &spheres,
                          size_t i) {
    leaf->bbox = AABB{spheres[i]};
    leaf->left_i = -1;
    leaf->right_i = -1;
    leaf->sphere_i = i;
  }

  void set_internal(BVHNode *node, BVHNode *left, BVHNode *right,
                    size_t split_axis) {
    node->left_i = left - root;
    node->right_i = right - root;
    node->bbox = AABB{left->bbox, right->bbox};
    node->split_axis = static_cast<int>(split_axis);
  }

  static float centroid_axis(const Sphere &sphere, size_t axis) {
    return axis == 0 ? sphere.x : axis == 1 ? sphere.y : sphere.z;
  }

  static size_t widest_centroid_axis(const std::vector<Sphere> &spheres,
                                     size_t start, size_t end) {
    auto min_x{spheres[start].x};
    auto max_x{min_x};
    auto min_y{spheres[start].y};
    auto max_y{min_y};
    auto min_z{spheres[start].z};
    auto max_z{min_z};

    for (auto i{start + 1}; i < end; ++i) {
      const auto &sphere{spheres[i]};
      min_x = std::min(min_x, sphere.x);
      max_x = std::max(max_x, sphere.x);
      min_y = std::min(min_y, sphere.y);
      max_y = std::max(max_y, sphere.y);
      min_z = std::min(min_z, sphere.z);
      max_z = std::max(max_z, sphere.z);
    }

    const auto x_extent{max_x - min_x};
    const auto y_extent{max_y - min_y};
    const auto z_extent{max_z - min_z};

    if (x_extent >= y_extent && x_extent >= z_extent) {
      return size_t{0};
    }
    return y_extent >= z_extent ? size_t{1} : size_t{2};
  }

  // Object-median split on the widest centroid axis. Always yields a balanced,
  // non-degenerate partition, so it is used as the fallback when SAH cannot find
  // a valid split (e.g. all centroids coincide on every axis).
  static void median_split(std::vector<Sphere> &spheres, size_t start,
                           size_t end, size_t &out_axis, size_t &out_mid) {
    const auto span{end - start};
    const auto mid{start + span / 2};
    const auto axis{widest_centroid_axis(spheres, start, end)};
    std::nth_element(spheres.begin() + static_cast<ptrdiff_t>(start),
                     spheres.begin() + static_cast<ptrdiff_t>(mid),
                     spheres.begin() + static_cast<ptrdiff_t>(end),
                     [axis](const Sphere &a, const Sphere &b) {
                       return centroid_axis(a, axis) < centroid_axis(b, axis);
                     });
    out_axis = axis;
    out_mid = mid;
  }

  // Binned Surface Area Heuristic. Bins primitive centroids into SAH_BUCKETS
  // buckets along each axis, evaluates the SAH cost
  // (left_count * left_area + right_count * right_area) at every bucket
  // boundary, and partitions around the cheapest one. Returns false (leaving the
  // caller to fall back to median_split) when no axis has centroid extent or the
  // chosen plane fails to separate the primitives.
  static bool sah_split(std::vector<Sphere> &spheres, size_t start, size_t end,
                        size_t &out_axis, size_t &out_mid) {
    constexpr size_t K{SAH_BUCKETS};

    float axis_min[3]{};
    float axis_extent[3]{};

    float best_cost{INF};
    size_t best_axis{};
    size_t best_bucket{};
    bool found{false};

    for (size_t axis{}; axis < 3; ++axis) {
      auto cmin{centroid_axis(spheres[start], axis)};
      auto cmax{cmin};
      for (auto i{start + 1}; i < end; ++i) {
        const auto c{centroid_axis(spheres[i], axis)};
        cmin = std::min(cmin, c);
        cmax = std::max(cmax, c);
      }

      const auto extent{cmax - cmin};
      axis_min[axis] = cmin;
      axis_extent[axis] = extent;
      if (extent <= 0.0f) {
        continue;
      }

      int counts[K]{};
      AABB bounds[K]{};
      for (auto i{start}; i < end; ++i) {
        auto b{bucket_of(centroid_axis(spheres[i], axis), cmin, extent)};
        counts[b] += 1;
        bounds[b] = AABB{bounds[b], AABB{spheres[i]}};
      }

      AABB suffix_box[K]{};
      int suffix_count[K]{};
      AABB acc_box{};
      int acc_count{};
      for (size_t i{K}; i-- > 0;) {
        acc_box = AABB{acc_box, bounds[i]};
        acc_count += counts[i];
        suffix_box[i] = acc_box;
        suffix_count[i] = acc_count;
      }

      AABB left_box{};
      int left_count{};
      for (size_t i{}; i + 1 < K; ++i) {
        left_box = AABB{left_box, bounds[i]};
        left_count += counts[i];
        const auto right_count{suffix_count[i + 1]};
        if (left_count == 0 || right_count == 0) {
          continue;
        }

        const auto cost{
            static_cast<float>(left_count) * left_box.surface_area() +
            static_cast<float>(right_count) * suffix_box[i + 1].surface_area()};
        if (cost < best_cost) {
          best_cost = cost;
          best_axis = axis;
          best_bucket = i;
          found = true;
        }
      }
    }

    if (!found) {
      return false;
    }

    const auto cmin{axis_min[best_axis]};
    const auto extent{axis_extent[best_axis]};
    const auto begin{spheres.begin()};
    const auto mid_it{std::partition(
        begin + static_cast<ptrdiff_t>(start),
        begin + static_cast<ptrdiff_t>(end), [&](const Sphere &s) {
          return bucket_of(centroid_axis(s, best_axis), cmin, extent) <=
                 best_bucket;
        })};

    const auto mid{static_cast<size_t>(mid_it - begin)};
    if (mid == start || mid == end) {
      return false;
    }

    out_axis = best_axis;
    out_mid = mid;
    return true;
  }

  static size_t bucket_of(float centroid, float cmin, float extent) {
    auto b{static_cast<size_t>(static_cast<float>(SAH_BUCKETS) *
                               ((centroid - cmin) / extent))};
    return b >= SAH_BUCKETS ? SAH_BUCKETS - 1 : b;
  }

  int fill_tree(std::vector<Sphere> &spheres, size_t start, size_t end) {
    auto i{open++};
    auto node{root + i};

    auto span{end - start};
    if (span == 1) {
      set_leaf(node, spheres, start);
      return i;
    }

    size_t axis{};
    size_t mid{};
    if (!sah_split(spheres, start, end, axis, mid)) {
      median_split(spheres, start, end, axis, mid);
    }

    auto left{fill_tree(spheres, start, mid)};
    auto right{fill_tree(spheres, mid, end)};

    set_internal(node, root + left, root + right, axis);

    return i;
  }
};
