#pragma once

#include "config.hpp"
#include "shared/math/interval.hpp"
#include "shared/math/ray.hpp"
#include "shared/math/vec3.hpp"
#include "shared/render/background.hpp"
#include <cmath>

inline HOSTDEV Vec3 sample_square(float rand_x, float rand_y) {
  return {rand_x - 0.5f, rand_y - 0.5f, 0};
}

static HOSTDEV float gamma(float f) { return f > 0 ? std::sqrt(f) : 0; }

inline HOSTDEV Vec3 gamma_vec(const Vec3 v) {
  static constexpr Interval intensity{0.000, 0.999};
  auto r_byte{intensity.clamp(gamma(v.x()))};
  auto g_byte{intensity.clamp(gamma(v.y()))};
  auto b_byte{intensity.clamp(gamma(v.z()))};
  return {r_byte, g_byte, b_byte};
}

inline HOSTDEV Vec3 mix_with_sky(Ray ray, Vec3 attenuation) {
  auto dir{norm(ray.direction())};
  auto a{0.5 * (dir.y() + 1.0f)};
  auto sky_val{(1.0f - a) * WHITE + a * SKY_BLUE};
  return attenuation * sky_val;
}
