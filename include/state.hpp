#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"
#include <memory>
#include <random>

struct CPUState {
  std::unique_ptr<Camera> camera;
  std::unique_ptr<std::vector<Material>> materials;
  std::unique_ptr<SphereBuffer> spheres;
  std::unique_ptr<FrameBuffer> buffer;
  std::unique_ptr<std::mt19937> generator;
};

struct GPUState {
  Camera *camera;
  Material *materials;
  SphereBuffer *spheres;
  FrameBuffer *buffer;
  void *generator; // curandState*
};
