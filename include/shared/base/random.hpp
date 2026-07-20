#pragma once

#include "shared/math/vec3.hpp"
#include <random>

// without the ifdef, device compilation will attempt to compile the
// host specific code on the device, which fails.
template <typename T> inline HOSTDEV float random_float(T *generator) {
#ifdef __CUDA_ARCH__
  return gpu_rand_float((curandState *)(generator));
#else
  auto gen{static_cast<std::mt19937>(*generator)};
  std::uniform_real_distribution<float> distribution(0.0, 1.0);
  return distribution(gen);
#endif
  return 0.0f;
}

template <typename T>
inline HOSTDEV float random_float(float min, float max, T *generator) {
  return min + (max - min) * random_float(generator);
}

template <typename T> inline HOSTDEV Vec3 random_vec3(T *generator) {
  return {random_float(generator), random_float(generator),
          random_float(generator)};
}

template <typename T>
inline Vec3 HOSTDEV random_vec3(float min, float max, T *generator) {
  return {random_float(min, max, generator), random_float(min, max, generator),

          random_float(min, max, generator)};
}

template <typename T> inline HOSTDEV Vec3 random_norm(T *generator) {
  static constexpr auto epilson{1e-45f};
  while (true) {
    auto v{random_vec3(-1.0f, 1.0f, generator)};
    auto len_sq{v.len_sq()};
    if (len_sq <= 1.0f && len_sq > epilson) {
      return v;
    }
  }
}

template <typename T>
inline HOSTDEV Vec3 random_on_hemisphere(Vec3 normal, T *generator) {
  auto v{random_norm(generator)};
  return v.dot(normal) > 0 ? v : -v;
}
