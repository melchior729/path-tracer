#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"

inline std::vector<Sphere> three_spheres(std::vector<Material> &materials) {
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.8f, 0.15f, 0.1f}}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.85f, 0.85f, 0.9f}, 0.05f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{1.5f}});

  return {
      {-2.2f, 0.0f, -4.0f, 1.0f, 0},
      {0.0f, 0.0f, -4.0f, 1.0f, 1},
      {2.2f, 0.0f, -4.0f, 1.0f, 2},
  };
}
