#pragma once

#include "shared/math/ray.hpp"
#include "shared/math/vec3.hpp"

struct HitRecord {
  Point3 p;
  Vec3 normal;
  size_t mat_idx;
  float t;
  bool front_facing;

  constexpr HOSTDEV void set_normal(const Ray &ray, const Vec3 &surf_norm) {
    auto dir{ray.direction()};
    front_facing = dir.dot(surf_norm) < 0;
    normal = front_facing ? surf_norm : -surf_norm;
  }
};
