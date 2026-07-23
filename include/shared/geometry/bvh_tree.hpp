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

  bool HOSTDEV is_leaf() const {
    return left_i == -1 && right_i == -1;
  }
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

  int fill_tree(std::vector<Sphere> &spheres, size_t start, size_t end) {
    auto i{open++};
    auto node{root + i};

    auto span{end - start};
    if (span == 1) {
      set_leaf(node, spheres, start);
      return i;
    }

    auto mid{start + span / 2};
    auto axis{widest_centroid_axis(spheres, start, end)};
    std::nth_element(
        spheres.begin() + static_cast<ptrdiff_t>(start),
        spheres.begin() + static_cast<ptrdiff_t>(mid),
        spheres.begin() + static_cast<ptrdiff_t>(end),
        [axis](const Sphere &a, const Sphere &b) {
          return centroid_axis(a, axis) < centroid_axis(b, axis);
        });

    auto left{fill_tree(spheres, start, mid)};
    auto right{fill_tree(spheres, mid, end)};

    set_internal(node, root + left, root + right, axis);

    return i;
  }
};
