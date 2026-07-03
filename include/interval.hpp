#pragma once

#include "config.hpp"
#include <algorithm>

struct Interval {
  float min;
  float max;

  constexpr HOSTDEV Interval() : min(INF), max(-INF) {}

  constexpr HOSTDEV Interval(float min, float max) : min(min), max(max) {}

  constexpr HOSTDEV Interval(const Interval a, const Interval b) {
    min = std::min(a.min, b.min);
    max = std::max(a.max, b.max);
  }

  constexpr HOSTDEV float size() const { return max - min; }

  constexpr HOSTDEV bool surronds(float x) const { return min < x && x < max; }

  constexpr HOSTDEV bool contains(float x) const {
    return min <= x && x <= max;
  }

  constexpr HOSTDEV float clamp(float x) const {
    return std::clamp(x, min, max);
  }

  constexpr HOSTDEV Interval expand(float delta) const {
    float padding{delta / 2};
    return {min - padding, max + padding};
  }

  static constexpr HOSTDEV Interval empty() { return {INF, -INF}; }

  static constexpr Interval universe() { return {-INF, INF}; }
};
