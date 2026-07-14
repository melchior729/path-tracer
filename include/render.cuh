#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "sphere_buffer.hpp"
#include "sphere_hit.hpp"
#include "util.hpp"
#include <cassert>

template <typename T>
static HOSTDEV Vec3 ray_color(const Ray ray, const SphereBuffer *spheres,
                              const Material *materials, T *generator,
                              size_t depth) {
  Ray incoming{ray};
  Vec3 acc_attenuation{WHITE};

  for (size_t i{}; i < depth; ++i) {
    HitRecord record;
    if (!hit_spheres(spheres, incoming, Interval{0.001, INF}, &record)) {
      return mix_with_sky(incoming, acc_attenuation);
    }

    Ray scattered;
    Vec3 attenuation;
    auto material{materials[record.mat_idx]};
    auto rand_float{random_float(generator)};
    Vec3 rand_vec{rand_float, random_float(generator), random_float(generator)};

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

template <typename T>
inline HOSTDEV void render(const Camera *camera, const SphereBuffer *spheres,
                           const Material *materials, FrameBuffer *buffer,
                           size_t i, size_t j, T *generator) {
  Vec3 col;
  for (size_t s{}; s < SAMPLE_COUNT; ++s) {
    auto ray{camera->ray_at_pixel(
        sample_square(random_float(generator), random_float(generator)), i, j)};
    col += ray_color(ray, spheres, materials, generator, MAX_DEPTH);
  }

  col /= SAMPLE_COUNT;
  auto writtable{gamma_vec(col)};
  buffer->set(i, j, Color{writtable});
}

inline void cpu_render(const Camera *camera, const SphereBuffer *spheres,
                       const Material *materials, FrameBuffer *buffer,
                       std::mt19937 *generator) {
  for (size_t j{}; j < HEIGHT; ++j) {
    for (size_t i{}; i < WIDTH; ++i) {
      render(camera, spheres, materials, buffer, i, j, generator);
    }
  }
}

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, FrameBuffer *buffer,
                 void *generator);
