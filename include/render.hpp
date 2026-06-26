#pragma once

#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "frame_buffer.hpp"
#include "hit_record.hpp"
#include "sphere_buffer.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include <cmath>

static constexpr Vec3 WHITE{1.0f, 1.0f, 1.0f};
static constexpr Vec3 SKY_BLUE{0.53f, 0.81f, 0.92f};

static Vec3 sample_square() {
  return {random_float() - 0.5f, random_float() - 0.5f, 0};
}

static constexpr Vec3 ray_color(const Ray &ray, const SphereBuffer &spheres,
                                int depth) {
  if (depth <= 0) {
    return {};
  }

  HitRecord record;
  if (hit_spheres(spheres, ray, Interval{0.001, INFINITY}, record)) {
    auto dir{record.normal + random_norm()};
    return 0.5 * ray_color({record.p, dir}, spheres, depth - 1);
  }

  auto dir{norm(ray.direction())};
  auto a{0.5f * (dir.y() + 1.0f)};
  return (1.0f - a) * WHITE + a * SKY_BLUE;
}

inline void render(const Camera &camera, const SphereBuffer &spheres,
                   FrameBuffer &buffer) {
  for (size_t j{}; j < HEIGHT; ++j) {
    for (size_t i{}; i < WIDTH; ++i) {
      Vec3 col;
      for (size_t s{}; s < SAMPLE_COUNT; ++s) {
        auto ray{camera.get_ray(sample_square(), i, j)};
        col += ray_color(ray, spheres, MAX_DEPTH);
      }

      col /= SAMPLE_COUNT;
      buffer.set(i, j, Color{col});
    }
  }
}
