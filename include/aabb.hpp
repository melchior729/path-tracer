#pragma once

#include "interval.hpp"
#include "ray.hpp"

struct AABB {
  Interval x, y, z;

  constexpr AABB() {}

  constexpr AABB(const Interval &x, const Interval &y, const Interval &z)
      : x(x), y(y), z(z) {}

  constexpr AABB(const Point3 &min, const Point3 &max) {
    x = (min.x() <= max.x()) ? Interval{min.x(), max.x()}
                             : Interval{max.x(), min.x()};
    y = (min.y() <= max.y()) ? Interval{min.y(), max.y()}
                             : Interval{max.y(), min.y()};
    z = (min.z() <= max.z()) ? Interval{min.z(), max.z()}
                             : Interval{max.z(), min.z()};
  }

  constexpr AABB(const AABB &a, const AABB &b) {
    x = Interval{a.x, b.x};
    y = Interval{a.y, b.y};
    z = Interval{a.z, b.z};
  }

  constexpr Interval &interval_axis(size_t n) {
    return (n == 1) ? y : (n == 2) ? z : x;
  }

  bool hit(const Ray &ray, Interval interval) {
    auto &origin{ray.origin()};
    auto &dir{ray.direction()};

    for (size_t ax{}; ax < 3; ++ax) {
      auto &axis{interval_axis(ax)};
      auto inv_dir{1.0f / dir[ax]};

      auto t0{(axis.min - origin[ax]) * inv_dir};
      auto t1{(axis.max - origin[ax]) * inv_dir};

      if (t0 < t1) {
        if (t0 > interval.min) {
          interval.min = t0;
        }
        if (t1 < interval.max) {
          interval.max = t1;
        }
      } else {
        if (t1 > interval.min) {
          interval.min = t1;
        }
        if (t0 < interval.max) {
          interval.max = t0;
        }
      }

      if (interval.max <= interval.min) {
        return false;
      }
    }

    return true;
  }
};
