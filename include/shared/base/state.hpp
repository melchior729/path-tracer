#pragma once

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
  size_t mat_size;
};

struct GPUState {
  Camera *camera;
  Material *materials;
  SphereBuffer *spheres;
  FrameBuffer *buffer;
  void *generator; // curandState*
  size_t mat_size;
};
