#pragma once

#include "material.hpp"
#include "sphere_buffer.hpp"

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

inline SphereBuffer complex_scene(std::vector<Material> &materials) {
  // Original 5
  Lambertian red{Vec3{0.8f, 0.1f, 0.1f}};
  Lambertian dark{Vec3{0.15f, 0.15f, 0.15f}};
  Metal mirror{Vec3{0.9f, 0.9f, 0.9f}, 0.0f};
  Metal brushed{Vec3{0.57f, 0.79f, 0.53f}, 0.7f};
  Dielectric glass{1.5f};

  // 7 New Materials
  Metal gold{Vec3{0.8f, 0.6f, 0.2f}, 0.1f};
  Metal copper{Vec3{0.7f, 0.3f, 0.2f}, 0.05f};
  Metal rough_steel{Vec3{0.4f, 0.4f, 0.4f}, 0.8f};
  Lambertian matte_black{Vec3{0.02f, 0.02f, 0.02f}};
  Lambertian neon_blue{Vec3{0.1f, 0.2f, 0.9f}};
  Dielectric diamond{2.4f};
  Dielectric water{1.33f};

  materials.push_back(
      {.type = MaterialType::Lambertian, .lambertian = red}); // 0
  materials.push_back(
      {.type = MaterialType::Lambertian, .lambertian = dark});          // 1
  materials.push_back({.type = MaterialType::Metal, .metal = mirror});  // 2
  materials.push_back({.type = MaterialType::Metal, .metal = brushed}); // 3
  materials.push_back(
      {.type = MaterialType::Dielectric, .dielectric = glass}); // 4

  materials.push_back({.type = MaterialType::Metal, .metal = gold}); // 5
  materials.push_back(
      {.type = MaterialType::Lambertian, .lambertian = matte_black}); // 6
  materials.push_back(
      {.type = MaterialType::Dielectric, .dielectric = diamond});      // 7
  materials.push_back({.type = MaterialType::Metal, .metal = copper}); // 8
  materials.push_back(
      {.type = MaterialType::Lambertian, .lambertian = neon_blue}); // 9
  materials.push_back(
      {.type = MaterialType::Metal, .metal = rough_steel}); // 10
  materials.push_back(
      {.type = MaterialType::Dielectric, .dielectric = water}); // 11

  std::vector<Sphere> spheres;

  // Ground plane
  spheres.push_back(
      {0.0f, -1000.0f, 0.0f, 1000.0f, 6}); // Matte black infinite floor

  // Centerpiece
  spheres.push_back({0.0f, 1.0f, -4.0f, 1.0f, 7}); // Giant diamond
  spheres.push_back({0.0f, 2.3f, -4.0f, 0.3f, 5}); // Gold crown

  // Left Pillar
  spheres.push_back({-2.0f, 0.5f, -3.5f, 0.5f, 8});  // Copper base
  spheres.push_back({-2.0f, 1.5f, -3.5f, 0.5f, 2});  // Mirror mid
  spheres.push_back({-2.0f, 2.5f, -3.5f, 0.5f, 10}); // Steel top

  // Right Pillar
  spheres.push_back({2.0f, 0.5f, -3.5f, 0.5f, 4}); // Glass base
  spheres.push_back({2.0f, 1.5f, -3.5f, 0.5f, 5}); // Gold mid
  spheres.push_back({2.0f, 2.5f, -3.5f, 0.5f, 9}); // Blue top

  // Background Arch (Alternating rough steel and matte black)
  spheres.push_back({-4.0f, 1.0f, -6.0f, 1.0f, 10});
  spheres.push_back({-3.0f, 3.0f, -6.0f, 1.0f, 6});
  spheres.push_back({-1.5f, 4.5f, -6.0f, 1.0f, 10});
  spheres.push_back({0.0f, 5.0f, -6.0f, 1.0f, 6});
  spheres.push_back({1.5f, 4.5f, -6.0f, 1.0f, 10});
  spheres.push_back({3.0f, 3.0f, -6.0f, 1.0f, 6});
  spheres.push_back({4.0f, 1.0f, -6.0f, 1.0f, 10});

  // Scattered Foreground
  spheres.push_back({-0.8f, 0.2f, -2.0f, 0.2f, 11}); // Water drop
  spheres.push_back({0.8f, 0.2f, -2.0f, 0.2f, 0});   // Red marble
  spheres.push_back({0.0f, 0.2f, -1.5f, 0.2f, 2});   // Mirror marble
  spheres.push_back({-1.5f, 0.2f, -2.5f, 0.2f, 5});  // Gold marble
  spheres.push_back({1.5f, 0.2f, -2.5f, 0.2f, 8});   // Copper marble

  // Floating debris around center
  spheres.push_back({-1.0f, 1.5f, -2.5f, 0.1f, 9});
  spheres.push_back({1.0f, 1.8f, -2.8f, 0.1f, 9});
  spheres.push_back({0.5f, 0.8f, -3.0f, 0.1f, 4});
  spheres.push_back({-0.5f, 2.0f, -3.2f, 0.15f, 8});
  spheres.push_back({0.0f, 3.5f, -4.0f, 0.2f, 5});

  return SphereBuffer{spheres};
}
