#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "shared/base/random.hpp"
#include "shared/base/util.hpp"
#include "shared/geometry/bvh_tree.hpp"
#include "shared/geometry/sphere_buffer.hpp"
#include "shared/math/interval.hpp"
#include "shared/math/ray.hpp"
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
                              const Material *materials, const BVHTree *tree,
                              T *generator, size_t depth) {
  Ray incoming{ray};
  Vec3 acc_attenuation{WHITE};

  for (size_t i{}; i < depth; ++i) {
    HitRecord record;

    if (!tree->hit(incoming, Interval{0.001f, INF}, &record, spheres)) {
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
                           size_t i, size_t j, const BVHTree *tree,
                           T *generator) {
  Vec3 col;
  for (size_t s{}; s < SAMPLE_COUNT; ++s) {
    auto ray{camera->ray_at_pixel(
        sample_square(random_float(generator), random_float(generator)), i, j)};
    col += ray_color(ray, spheres, materials, tree, generator, MAX_DEPTH);
  }

  col /= SAMPLE_COUNT;
  auto writtable{gamma_vec(col)};
  buffer->set(i, j, Color{writtable});
}
