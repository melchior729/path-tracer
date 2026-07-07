#pragma once

#include <cstdlib>
#include <vector>

struct Sphere {
  float x;
  float y;
  float z;
  float r;
  size_t mat_idx;
};

struct SphereBuffer {
  float *center_x;
  float *center_y;
  float *center_z;
  float *radii;
  size_t *materials;
  size_t size;

  SphereBuffer(const std::vector<Sphere> &spheres) {
    size = spheres.size();
    center_x = static_cast<float *>(malloc(size * sizeof(float)));
    center_y = static_cast<float *>(malloc(size * sizeof(float)));
    center_z = static_cast<float *>(malloc(size * sizeof(float)));
    radii = static_cast<float *>(malloc(size * sizeof(float)));
    materials = static_cast<size_t *>(malloc(size * sizeof(size_t)));

    for (size_t i{}; i < spheres.size(); ++i) {
      auto s{spheres[i]};
      center_x[i] = s.x;
      center_y[i] = s.y;
      center_z[i] = s.z;
      radii[i] = s.r;
      materials[i] = s.mat_idx;
    }
  }
};
