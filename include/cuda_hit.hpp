#pragma once

#include "hit_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "sphere_buffer.hpp"

inline HOSTDEV bool
cuda_hit_spheres([[maybe_unused]] const SphereBuffer *spheres,
                 [[maybe_unused]] Ray ray, [[maybe_unused]] Interval interval,
                 [[maybe_unused]] HitRecord *record) {
  return false;
}
