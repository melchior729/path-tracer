#pragma once

#include <vector>

struct SphereBuffer {
  std::vector<float> center_x;
  std::vector<float> center_y;
  std::vector<float> center_z;
  std::vector<float> radii;
  std::vector<size_t> materials;

  void add(float x, float y, float z, float r, size_t mat_i) {
    center_x.push_back(x);
    center_y.push_back(y);
    center_z.push_back(z);
    radii.push_back(r);
    materials.push_back(mat_i);
  }
};
