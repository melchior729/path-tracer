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

  SphereBuffer(const SphereBuffer &other) {
    size = other.size;
    center_x = static_cast<float *>(malloc(size * sizeof(float)));
    center_y = static_cast<float *>(malloc(size * sizeof(float)));
    center_z = static_cast<float *>(malloc(size * sizeof(float)));
    radii = static_cast<float *>(malloc(size * sizeof(float)));
    materials = static_cast<size_t *>(malloc(size * sizeof(size_t)));

    std::copy(other.center_x, other.center_x + size, center_x);
    std::copy(other.center_y, other.center_y + size, center_y);
    std::copy(other.center_z, other.center_z + size, center_z);
    std::copy(other.radii, other.radii + size, radii);
    std::copy(other.materials, other.materials + size, materials);
  }

  SphereBuffer &operator=(const SphereBuffer &other) {
    auto temp{other};
    std::swap(center_x, temp.center_x);
    std::swap(center_y, temp.center_y);
    std::swap(center_z, temp.center_z);
    std::swap(radii, temp.radii);
    std::swap(materials, temp.materials);
    return *this;
  }

  SphereBuffer(SphereBuffer &&other)
      : center_x(other.center_x), center_y(other.center_y),
        center_z(other.center_z), radii(other.radii),
        materials(other.materials), size(other.size) {
    other.center_x = nullptr;
    other.center_y = nullptr;
    other.center_z = nullptr;
    other.radii = nullptr;
    other.materials = nullptr;
  }

  SphereBuffer &operator=(SphereBuffer &&other) {
    std::swap(center_x, other.center_x);
    std::swap(center_y, other.center_y);
    std::swap(center_z, other.center_z);
    std::swap(radii, other.radii);
    std::swap(materials, other.materials);
    return *this;
  }

  ~SphereBuffer() {
    free(center_x);
    free(center_y);
    free(center_z);
    free(radii);
    free(materials);
  }
};
