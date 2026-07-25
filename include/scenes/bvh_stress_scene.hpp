#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"
#include <cmath>
#include <vector>

inline std::vector<Sphere> bvh_stress_scene(std::vector<Material> &materials) {
  // 0: Simulated ground plane (massive sphere)
  materials.push_back({.type = MaterialType::Lambertian,
                       .lambertian = Lambertian{Vec3{0.5f, 0.5f, 0.5f}}});

  // 1: Center focus (Glass)
  materials.push_back(
      {.type = MaterialType::Dielectric, .dielectric = Dielectric{1.5f}});

  // 2: Left focus (Matte)
  materials.push_back({.type = MaterialType::Lambertian,
                       .lambertian = Lambertian{Vec3{0.4f, 0.2f, 0.1f}}});

  // 3: Right focus (Metal)
  materials.push_back({.type = MaterialType::Metal,
                       .metal = Metal{Vec3{0.7f, 0.6f, 0.5f}, 0.0f}});

  std::vector<Sphere> spheres;
  spheres.reserve(500);

  // Ground plane sphere
  spheres.push_back({0.0f, -1000.0f, 0.0f, 1000.0f, 0});

  // Hero spheres
  spheres.push_back({0.0f, 1.0f, 0.0f, 1.0f, 1});
  spheres.push_back({-4.0f, 1.0f, 0.0f, 1.0f, 2});
  spheres.push_back({4.0f, 1.0f, 0.0f, 1.0f, 3});

  // Simple pseudo-random hash to generate a deterministic scatter layout
  // without relying on <random> side-effects.
  auto hash = [](int x, int z) {
    float f = std::abs(x * 3.14159f + z * 2.71828f);
    return f - std::floor(f);
  };

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = hash(a, b);
      float x_pos = a + 0.9f * hash(a, b + 1);
      float z_pos = b + 0.9f * hash(a + 1, b);

      // Ensure smaller spheres don't clip into the hero spheres
      if ((x_pos * x_pos + z_pos * z_pos) > 4.0f) {
        if (choose_mat < 0.6f) {
          // Diffuse
          materials.push_back(
              {.type = MaterialType::Lambertian,
               .lambertian = Lambertian{
                   Vec3{hash(a + 2, b), hash(a + 3, b), hash(a + 4, b)}}});
          spheres.push_back({x_pos, 0.2f, z_pos, 0.2f, materials.size() - 1});
        } else if (choose_mat < 0.85f) {
          // Metal
          materials.push_back(
              {.type = MaterialType::Metal,
               .metal = Metal{Vec3{0.5f + 0.5f * hash(a, b + 2),
                                   0.5f + 0.5f * hash(a, b + 3),
                                   0.5f + 0.5f * hash(a, b + 4)},
                              0.3f * hash(a, b + 5)}});
          spheres.push_back({x_pos, 0.2f, z_pos, 0.2f, materials.size() - 1});
        } else {
          // Glass (reusing material 1)
          spheres.push_back({x_pos, 0.2f, z_pos, 0.2f, 1});
        }
      }
    }
  }

  return spheres;
}
