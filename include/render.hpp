#pragma once

#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "frame_buffer.hpp"
#include "hit_record.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include <cmath>

static Vec3 sample_square() {
  return {random_float() - 0.5f, random_float() - 0.5f, 0};
}

static Vec3 ray_color(const Ray &ray, const SphereBuffer &spheres,
                      const std::vector<Material> &materials, int depth) {
  if (depth <= 0) {
    return {};
  }

  HitRecord record;
  if (hit_spheres(spheres, ray, Interval{0.001, INFINITY}, record)) {
    Ray scattered;
    Vec3 attenuation;

    auto material{materials[record.mat_idx]};
    switch (material.type) {
    case MaterialType::Lambertian:
      if (material.lambertian.scatter(record, attenuation, scattered)) {
        return attenuation *
               ray_color(scattered, spheres, materials, depth - 1);
      }
      break;

    case MaterialType::Metal:
      if (material.metal.scatter(ray, record, attenuation, scattered)) {
        return attenuation *
               ray_color(scattered, spheres, materials, depth - 1);
      }
      break;

    case MaterialType::Dielectric:
      if (material.dielectric.scatter(ray, record, attenuation, scattered)) {
        return attenuation *
               ray_color(scattered, spheres, materials, depth - 1);
      }

      break;
    }
  }

  auto dir{norm(ray.direction())};
  auto a{0.5f * (dir.y() + 1.0f)};
  return (1.0f - a) * WHITE + a * SKY_BLUE;
}

static float gamma(float v) { return v > 0 ? std::sqrt(v) : 0; }

static Vec3 gamma_vec(const Vec3 &v) {
  static constexpr Interval intensity{0.000, 0.999};
  auto r_byte{intensity.clamp(gamma(v.x()))};
  auto g_byte{intensity.clamp(gamma(v.y()))};
  auto b_byte{intensity.clamp(gamma(v.z()))};
  return {r_byte, g_byte, b_byte};
}

inline void render(const Camera &camera, const SphereBuffer &spheres,
                   const std::vector<Material> &materials,
                   FrameBuffer &buffer) {
  for (size_t j{}; j < HEIGHT; ++j) {
    for (size_t i{}; i < WIDTH; ++i) {
      Vec3 col;
      for (size_t s{}; s < SAMPLE_COUNT; ++s) {
        auto ray{camera.get_ray(sample_square(), i, j)};
        col += ray_color(ray, spheres, materials, MAX_DEPTH);
      }

      col /= SAMPLE_COUNT;
      auto writtable{gamma_vec(col)};
      buffer.set(i, j, Color{writtable});
    }
  }
}
