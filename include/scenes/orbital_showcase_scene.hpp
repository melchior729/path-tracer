#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"
#include <cmath>

inline std::vector<Sphere>
orbital_showcase_scene(std::vector<Material> &materials) {
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.04f, 0.05f, 0.08f}}});
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.65f, 0.08f, 0.3f}}});
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.05f, 0.55f, 0.75f}}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.95f, 0.7f, 0.15f}, 0.08f}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.7f, 0.75f, 0.85f}, 0.35f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{1.5f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{2.4f}});

  std::vector<Sphere> spheres;
  spheres.reserve(64);

  // Floor and central glass-and-gold sculpture.
  spheres.push_back({0.0f, -101.0f, -8.0f, 100.0f, 0});
  spheres.push_back({0.0f, 1.0f, -7.0f, 1.45f, 6});
  spheres.push_back({0.0f, 2.7f, -7.0f, 0.45f, 3});
  spheres.push_back({-1.7f, 0.45f, -6.5f, 0.55f, 4});
  spheres.push_back({1.7f, 0.45f, -6.5f, 0.55f, 5});

  // Three tilted-looking orbital bands around the centerpiece.
  constexpr auto PI{3.1415926535f};
  constexpr size_t SPHERES_PER_RING{16};
  for (size_t ring{}; ring < 3; ++ring) {
    const auto radius{2.2f + static_cast<float>(ring) * 1.25f};
    for (size_t i{}; i < SPHERES_PER_RING; ++i) {
      const auto angle{2.0f * PI * static_cast<float>(i) /
                       static_cast<float>(SPHERES_PER_RING)};
      const auto phase{angle + static_cast<float>(ring) * 0.55f};
      const auto x{radius * std::cos(phase)};
      const auto y{2.2f + radius * 0.48f * std::sin(phase)};
      const auto z{-7.0f + 0.7f * std::sin(angle * 2.0f)};
      const auto sphere_radius{0.2f + 0.04f * static_cast<float>(ring)};
      const auto material{1 + (ring * SPHERES_PER_RING + i) % 6};
      spheres.push_back({x, y, z, sphere_radius, material});
    }
  }

  // A receding avenue adds depth and mixed reflection paths.
  for (size_t i{}; i < 5; ++i) {
    const auto z{-4.0f - static_cast<float>(i) * 2.0f};
    const auto radius{0.28f + static_cast<float>(i) * 0.06f};
    spheres.push_back({-4.8f, radius - 0.8f, z, radius, 1 + i % 6});
    spheres.push_back({4.8f, radius - 0.8f, z, radius, 1 + (i + 3) % 6});
  }

  return spheres;
}
