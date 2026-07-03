#pragma once

#include "camera.hpp"
#include "cpu_hit.hpp"
#include "frame_buffer.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sphere_buffer.hpp"
#include "util.hpp"
#include <cassert>

static Vec3 sample_square() {
  return {random_float() - 0.5f, random_float() - 0.5f, 0};
}

static Vec3 mix_with_sky(const Ray &ray, const Vec3 &attenuation) {
  auto dir{norm(ray.direction())};
  auto a{0.5 * (dir.y() + 1.0f)};
  auto sky_val{(1.0f - a) * WHITE + a * SKY_BLUE};
  return attenuation * sky_val;
}

static Vec3 ray_color(const Ray &ray, const SphereBuffer &spheres,
                      const std::vector<Material> &materials, size_t depth) {
  Ray incoming{ray};
  Vec3 acc_attenuation{WHITE};

  for (size_t i{}; i < depth; ++i) {
    HitRecord record;
    if (!cpu_hit_spheres(spheres, incoming, Interval{0.001, INF}, record)) {
      return mix_with_sky(incoming, acc_attenuation);
    }

    Ray scattered;
    Vec3 attenuation;
    auto material{materials[record.mat_idx]};
    auto rand_float{random_float()};
    Vec3 rand_vec{rand_float, random_float(), random_float()};

    switch (material.type) {
    case MaterialType::Lambertian:
      material.lambertian.scatter(record, rand_vec, attenuation, scattered);
      break;
    case MaterialType::Metal:
      if (!material.metal.scatter(incoming, record, rand_vec, attenuation,
                                  scattered)) {
        return mix_with_sky(incoming, acc_attenuation);
      }
      break;
    case MaterialType::Dielectric:
      if (!material.dielectric.scatter(incoming, record, rand_float,
                                       attenuation, scattered)) {
        return mix_with_sky(incoming, acc_attenuation);
      }
      break;
    default:
      assert(false);
    }

    incoming = scattered;
    acc_attenuation *= attenuation;
  }

  return {};
}

static float gamma(float v) { return v > 0 ? std::sqrt(v) : 0; }

static Vec3 gamma_vec(const Vec3 &v) {
  static constexpr Interval intensity{0.000, 0.999};
  auto r_byte{intensity.clamp(gamma(v.x()))};
  auto g_byte{intensity.clamp(gamma(v.y()))};
  auto b_byte{intensity.clamp(gamma(v.z()))};
  return {r_byte, g_byte, b_byte};
}

inline void cpu_render(const Camera &camera, const SphereBuffer &spheres,
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
