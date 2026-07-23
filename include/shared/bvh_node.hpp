#include "shared/geometry/sphere_buffer.hpp"
#include "shared/math/aabb.hpp"
#include "shared/math/interval.hpp"
#include <random>

struct BVHNode {
  AABB bbox;
  int left_i;
  int right_i;
  int sphere_i;
};

static int random_int(int min, int max) {
  static std::mt19937 gen{SEED};
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(gen);
}

struct BVHTree {

  BVHTree(std::vector<Sphere> spheres) {
    size = spheres.size();
    tree = static_cast<BVHNode *>(malloc(size * sizeof(BVHNode)));
    fill_tree(spheres, 0, size);
  }

private:
  BVHNode *tree;
  size_t open{};
  size_t size;

  constexpr void set_leaf(BVHNode *leaf, std::vector<Sphere> spheres,
                          size_t i) {
    leaf->bbox = AABB{spheres[i]};
    leaf->sphere_i = i;
  }

  void set_internal(BVHNode *node, BVHNode *left, BVHNode *right) {
    node->left_i = left - tree;
    node->right_i = right - tree;
    node->bbox = AABB{left->bbox, right->bbox};
    node->sphere_i = -1;
  }

  static bool box_compare(Sphere &a, Sphere &b, size_t axis_n) {
    auto a_axis_interval{AABB{a}.interval_axis(axis_n)};
    auto b_axis_interval{AABB{b}.interval_axis(axis_n)};
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare(Sphere &a, Sphere &b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(Sphere &a, Sphere &b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(Sphere &a, Sphere &b) {
    return box_compare(a, b, 2);
  }

  int fill_tree(std::vector<Sphere> spheres, size_t start, size_t end) {
    auto i{open++};
    auto node{tree + i};

    auto span{end - start};
    if (span == 1) {
      set_leaf(node, spheres, start);
    }

    else if (span == 2) {
      auto left_ch{tree + open++};
      auto right_ch{tree + open++};

      set_leaf(left_ch, spheres, start);
      set_leaf(right_ch, spheres, start + 1);
      set_internal(node, left_ch, right_ch);
    }

    else {
      auto axis{random_int(0, 2)};
      auto comp{axis == 0   ? box_x_compare
                : axis == 1 ? box_y_compare
                            : box_z_compare};
      std::sort(spheres.begin() + static_cast<ptrdiff_t>(start),
                spheres.begin() + static_cast<ptrdiff_t>(end), comp);

      auto mid{start + span / 2};
      auto left{fill_tree(spheres, start, mid)};
      auto right{fill_tree(spheres, mid, end)};

      set_internal(node, tree + left, tree + right);
    }

    return i;
  }
};
