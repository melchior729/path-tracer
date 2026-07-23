#pragma once

#include "shared/geometry/bvh_tree.hpp"
#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/camera.hpp"
#include "shared/render/frame_buffer.hpp"
#include "shared/render/material.hpp"
#include <memory>
#include <random>

struct CPUState {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<std::vector<Material>> materials;
  std::unique_ptr<SphereBuffer> spheres;
  std::unique_ptr<FrameBuffer> buffer;
  std::unique_ptr<std::mt19937> generator;
  std::unique_ptr<BVHTree> tree;
  size_t mat_size;
};

struct GPUState {
  Camera *camera{};
  Material *materials{};
  SphereBuffer *spheres{};
  FrameBuffer *buffer{};
  void *generator{}; // curandState*
  BVHTree *tree{};
  float *sphere_x{};
  float *sphere_y{};
  float *sphere_z{};
  float *sphere_radii{};
  size_t *sphere_materials{};
  BVHNode *bvh_nodes{};
  size_t mat_size{};
};
