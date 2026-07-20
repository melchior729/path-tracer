#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "sphere_buffer.hpp"
#include "sphere_hit.hpp"
#include "state.hpp"
#include "util.hpp"
#include <cassert>

template <typename T>
static HOSTDEV bool scatters_successfully(const Ray in, const HitRecord &record,
                                          const Material &material,
                                          Vec3 &attenuation, T *generator,
                                          Ray &out) {
  switch (material.type) {
  case MaterialType::Lambertian:
    material.lambertian.scatter(record, random_vec3(generator), attenuation,
                                out);
    return true;
  case MaterialType::Metal:
    return material.metal.scatter(in, record, random_vec3(generator),
                                  attenuation, out);
  case MaterialType::Dielectric:
    return material.dielectric.scatter(in, record, random_float(generator),
                                       attenuation, out);
  default:
    // this should not happen.
    assert(false);
  }

  return false;
}

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

    if (!scatters_successfully(incoming, record, material, attenuation,
                               generator, scattered)) {
      return mix_with_sky(incoming, acc_attenuation);
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

inline void cpu_render(CPUState *cpu) {
  for (size_t j{}; j < HEIGHT; ++j) {
    for (size_t i{}; i < WIDTH; ++i) {
      render(cpu->camera.get(), cpu->spheres.get(),
             cpu->materials.get()->data(), cpu->buffer.get(), i, j,
             cpu->generator.get());
    }
  }
}

void gpu_render(GPUState *gpu);
