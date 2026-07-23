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

  ~BVHTree() { free(root); }

private:
  BVHNode *root;
  size_t open{};
  size_t size;

  constexpr void set_leaf(BVHNode *leaf, const std::vector<Sphere> &spheres,
                          size_t i) {
    leaf->bbox = AABB{spheres[i]};
    leaf->sphere_i = i;
  }

  void set_internal(BVHNode *node, BVHNode *left, BVHNode *right) {
    node->left_i = left - root;
    node->right_i = right - root;
    node->bbox = AABB{left->bbox, right->bbox};
    node->sphere_i = -1;
  }

  static bool box_compare(const Sphere &a, const Sphere &b, size_t axis_n) {
    auto a_axis_interval{AABB{a}.interval_axis(axis_n)};
    auto b_axis_interval{AABB{b}.interval_axis(axis_n)};
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare(const Sphere &a, const Sphere &b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const Sphere &a, const Sphere &b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const Sphere &a, const Sphere &b) {
    return box_compare(a, b, 2);
  }

  int fill_tree(std::vector<Sphere> &spheres, size_t start, size_t end) {
    auto i{open++};
    auto node{root + i};

    auto span{end - start};
    if (span == 1) {
      set_leaf(node, spheres, start);
      return i;
    }

    auto axis{random_int(0, 2)};
    auto comp{axis == 0   ? box_x_compare
              : axis == 1 ? box_y_compare
                          : box_z_compare};
    std::sort(spheres.begin() + static_cast<ptrdiff_t>(start),
              spheres.begin() + static_cast<ptrdiff_t>(end), comp);

    auto mid{start + span / 2};
    auto left{fill_tree(spheres, start, mid)};
    auto right{fill_tree(spheres, mid, end)};

    set_internal(node, root + left, root + right);

    return i;
  }
};
