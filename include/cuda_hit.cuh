#pragma once

#include "hit_record.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "sphere_buffer.hpp"

__device__ bool cuda_hit_spheres(const SphereBuffer *spheres, Ray ray,
                                 Interval interval, HitRecord *record);
