#pragma once

#include "ray.hpp"
#include "vec3.hpp"

struct HitRecord {
  size_t mat_idx;
  Point3 p;
  Vec3 normal;
  float t;
  bool front_facing;

  void set_normal(const Ray &ray, const Vec3 &surf_norm) {
    auto dir{ray.direction()};
    front_facing = dir.dot(surf_norm) < 0;
    normal = front_facing ? surf_norm : -surf_norm;
  }
};
