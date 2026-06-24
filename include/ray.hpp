#pragma once

#include "vec3.hpp"

struct Ray {
  constexpr Ray() {}

  constexpr Ray(const Point3 &origin, const Vec3 &direction)
      : orig(origin), dir(direction) {}

  constexpr Point3 at(double t) const { return orig + t * dir; }

  const Point3 &origin() const { return orig; }

  const Vec3 &direction() const { return dir; }

private:
  Point3 orig;
  Vec3 dir;
};
