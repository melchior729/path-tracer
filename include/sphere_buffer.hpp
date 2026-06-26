#pragma once

#include "hit_record.hpp"
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

static bool hit_sphere(const SphereBuffer &spheres, const Ray &ray, size_t i,
                       Interval interval, HitRecord &record) {
  auto x{spheres.center_x[i]};
  auto y{spheres.center_y[i]};
  auto z{spheres.center_z[i]};
  auto r{spheres.radii[i]};

  Vec3 center{x, y, z};

  auto oc{center - ray.origin()};
  auto dir{ray.direction()};

  auto a{dir.len_sq()};
  auto b{dir.dot(oc)};
  auto c{oc.len_sq() - r * r};

  auto disc{b * b - a * c};
  if (disc < 0) {
    return false;
  }

  auto disc_sqrt{std::sqrt(disc)};
  auto root{(b - disc_sqrt) / a};

  if (root <= interval.min || interval.max <= root) {
    root = (b + disc_sqrt) / a;
    if (root <= interval.min || interval.max <= root) {
      return false;
    }
  }

  record.t = root;
  record.p = ray.at(root);
  auto surface_normal{(record.p - center) / r};
  record.set_normal(ray, surface_normal);
  // record.material = material;
  return true;
}

inline bool hit_spheres(const SphereBuffer &spheres, const Ray &ray,
                        Interval interval, HitRecord &record) {
  HitRecord temp;
  auto closest{interval.max};
  auto hit{false};

  for (size_t i{}; i < spheres.center_x.size(); ++i) {
    if (hit_sphere(spheres, ray, i, {interval.min, closest}, temp)) {
      hit = true;
      closest = temp.t;
      record = temp;
    }
  }

  return hit;
}
