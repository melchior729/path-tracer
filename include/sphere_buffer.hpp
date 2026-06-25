#pragma once

#include <aabb.hpp>
#include <vector>

struct SphereBuffer {
  std::vector<float> center_x;
  std::vector<float> center_y;
  std::vector<float> center_z;
  std::vector<float> radii;

  void add(float x, float y, float z, float r) {
    center_x.push_back(x);
    center_y.push_back(y);
    center_z.push_back(z);
    radii.push_back(r);
  }
};
