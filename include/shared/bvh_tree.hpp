#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/math/aabb.hpp"
// #include "shared/math/ray.hpp"
// #include "shared/render/hit_record.hpp"
// #include <stack>
#include <vector>

// there needs to be a BVH node creation before the rendering, and then
// memory allocated on the gpu for the BVH Node tree
// memcpy the CPU *bvhnode data to the GPU one.
// then the main shared/render/render.hpp code will be updated
struct BVHTree {

  // this needs to be iterative, builds the tree
  BVHTree(std::vector<Sphere> spheres) {
    nodes = static_cast<AABB *>(malloc(spheres.size() * sizeof(AABB)));
  }

  // for now, assume it is populated.
  // bool hit(const Ray in, Interval ray_t, HitRecord &record) {
  //   std::stack<BVHTree *> stack{};
  //   auto curr{this};
  //
  //   while (curr != nullptr || !stack.empty()) {
  //     while (curr != nullptr) {
  //       stack.push(this);
  //       // curr = curr->left;
  //     }
  //
  //     curr = stack.top();
  //     stack.pop();
  //     // curr = curr->right;
  //   }
  //
  //   return true;
  // }

  void left(size_t i) {
    auto left_i{2 * i + 1};
    if (left_i < size) {
      // return
    }
  }

private:
  AABB *nodes;
  size_t size;
};
