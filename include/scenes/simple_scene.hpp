#pragma once

#include "shared/geometry/sphere_buffer.hpp"
#include "shared/render/material.hpp"

inline SphereBuffer simple_scene(std::vector<Material> &materials) {
  Lambertian red{Vec3{0.8f, 0.1f, 0.1f}};
  Lambertian dark{Vec3{0.15f, 0.15f, 0.15f}};
  Metal mirror{Vec3{0.9f, 0.9f, 0.9f}, 0.0f};
  Metal brushed{Vec3{0.57f, 0.79f, 0.53f}, 0.7f};
  Dielectric glass{1.5f};

  Material mat0{.type = MaterialType::Lambertian, .lambertian = red};
  Material mat1{.type = MaterialType::Lambertian, .lambertian = dark};
  Material mat2{.type = MaterialType::Metal, .metal = mirror};
  Material mat3{.type = MaterialType::Metal, .metal = brushed};
  Material mat4{.type = MaterialType::Dielectric, .dielectric = glass};

  materials.push_back(mat0); // 0: red diffuse
  materials.push_back(mat1); // 1: dark diffuse
  materials.push_back(mat2); // 2: mirror metal
  materials.push_back(mat3); // 3: brushed metal
  materials.push_back(mat4); // 4: glass

  std::vector<Sphere> spheres;
  spheres.push_back({0.0f, 0.0f, -3.0f, 1.0f, 0});      // red, center
  spheres.push_back({2.5f, 0.0f, -4.0f, 1.0f, 2});      // mirror, right
  spheres.push_back({-2.5f, 0.0f, -4.0f, 1.0f, 3});     // brushed, left
  spheres.push_back({0.0f, 2.5f, -2.0f, 0.5f, 4});      // glass, foreground
  spheres.push_back({0.0f, -101.0f, -3.0f, 100.0f, 1}); // dark, ground sphere

  return SphereBuffer{spheres};
}
