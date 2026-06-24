#pragma once

#include <random>

constexpr float deg_to_rad(float d) { return d * std::numbers::pi / 180.0f; }

inline float random_float() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}
