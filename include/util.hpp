#pragma once

#include "vec3.hpp"
#include <random>

inline float random_float() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}

inline Vec3 random_vec3() {
  return {random_float(), random_float(), random_float()};
}

inline Vec3 random_vec3(float min, float max) {
  return {random_float(min, max), random_float(min, max),
          random_float(min, max)};
}

inline Vec3 random_norm() {
  static constexpr auto epilson{1e-45f};
  while (true) {
    auto v{random_vec3(-1.0f, 1.0f)};
    auto len_sq{v.len_sq()};
    if (len_sq <= 1.0f && len_sq > epilson) {
      return v;
    }
  }
}

inline Vec3 random_on_hemisphere(Vec3 normal) {
  auto v{random_norm()};
  return v.dot(normal) > 0 ? v : -v;
}
