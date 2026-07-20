#pragma once

#include "shared/base/config.hpp"
#include "vec3.hpp"

struct Ray {
  constexpr HOSTDEV Ray() {}

  constexpr HOSTDEV Ray(Point3 origin, Vec3 direction)
      : orig(origin), dir(direction) {}

  constexpr Point3 HOSTDEV at(float t) const { return orig + t * dir; }

  constexpr Point3 HOSTDEV origin() const { return orig; }

  constexpr Vec3 HOSTDEV direction() const { return dir; }

private:
  Point3 orig;
  Vec3 dir;
};
