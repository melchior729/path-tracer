#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"

// 120 spheres: large enough to show the BVH crossover without making scene
// construction or linear comparison prohibitively expensive.
inline std::vector<Sphere>
bvh_stress_scene(std::vector<Material> &materials) {
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.75f, 0.12f, 0.08f}}});
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.08f, 0.3f, 0.8f}}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.9f, 0.75f, 0.25f}, 0.05f}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.65f, 0.7f, 0.75f}, 0.55f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{1.5f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{2.4f}});

  std::vector<Sphere> spheres;
  spheres.reserve(120);

  for (size_t layer{}; layer < 5; ++layer) {
    for (size_t row{}; row < 4; ++row) {
      for (size_t column{}; column < 6; ++column) {
        const auto x{(static_cast<float>(column) - 2.5f) * 1.2f};
        const auto y{(static_cast<float>(row) - 1.5f) * 1.2f};
        const auto z{-4.0f - static_cast<float>(layer) * 1.8f};
        const auto material{(layer * 24 + row * 6 + column) %
                            materials.size()};
        spheres.push_back({x, y, z, 0.48f, material});
      }
    }
  }

  return spheres;
}
