#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"

inline void add_stress_test_materials(std::vector<Material> &materials) {
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.8f, 0.12f, 0.08f}}});
  materials.push_back(
      {.type = MaterialType::Lambertian,
       .lambertian = Lambertian{Vec3{0.08f, 0.35f, 0.8f}}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.9f, 0.7f, 0.2f}, 0.05f}});
  materials.push_back(
      {.type = MaterialType::Metal,
       .metal = Metal{Vec3{0.65f, 0.7f, 0.8f}, 0.5f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{1.5f}});
  materials.push_back(
      {.type = MaterialType::Dielectric,
       .dielectric = Dielectric{2.4f}});
}

inline std::vector<Sphere>
make_dense_sphere_field(std::vector<Material> &materials, size_t sphere_count,
                        size_t columns, size_t rows) {
  add_stress_test_materials(materials);

  std::vector<Sphere> spheres;
  spheres.reserve(sphere_count);

  constexpr auto SPACING{0.52f};
  constexpr auto RADIUS{0.21f};
  const auto layer_size{columns * rows};
  const auto center_column{static_cast<float>(columns - 1) / 2.0f};
  const auto center_row{static_cast<float>(rows - 1) / 2.0f};

  for (size_t i{}; i < sphere_count; ++i) {
    const auto index_in_layer{i % layer_size};
    const auto column{index_in_layer % columns};
    const auto row{index_in_layer / columns};
    const auto layer{i / layer_size};

    const auto x{(static_cast<float>(column) - center_column) * SPACING};
    const auto y{(static_cast<float>(row) - center_row) * SPACING};
    const auto z{-15.0f - static_cast<float>(layer) * SPACING};
    spheres.push_back({x, y, z, RADIUS, i % materials.size()});
  }

  return spheres;
}

inline std::vector<Sphere>
five_thousand_spheres(std::vector<Material> &materials) {
  return make_dense_sphere_field(materials, size_t{5000}, size_t{25},
                                 size_t{20});
}

inline std::vector<Sphere>
twenty_five_thousand_spheres(std::vector<Material> &materials) {
  return make_dense_sphere_field(materials, size_t{25000}, size_t{50},
                                 size_t{25});
}
