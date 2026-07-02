#pragma once

#include "config.hpp"
#include <algorithm>

struct Interval {
  float min;
  float max;

  constexpr Interval() : min(INF), max(-INF) {}

  constexpr Interval(float min, float max) : min(min), max(max) {}

  constexpr Interval(const Interval &a, const Interval &b) {
    min = std::min(a.min, b.min);
    max = std::max(a.max, b.max);
  }

  constexpr float size() const { return max - min; }

  constexpr bool surronds(float x) const { return min < x && x < max; }

  constexpr bool contains(float x) const { return min <= x && x <= max; }

  constexpr float clamp(float x) const { return std::clamp(x, min, max); }

  constexpr Interval expand(float delta) const {
    float padding{delta / 2};
    return {min - padding, max + padding};
  }

  static constexpr Interval empty() { return {INF, -INF}; }

  static constexpr Interval universe() { return {-INF, INF}; }
};
