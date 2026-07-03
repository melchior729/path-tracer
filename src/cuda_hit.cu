#include "hit_record.hpp"
#include "interval.hpp"
#include "sphere_buffer.hpp"

__device__ bool hit_spheres([[maybe_unused]] const SphereBuffer *spheres,
                            [[maybe_unused]] Ray ray,
                            [[maybe_unused]] Interval interval,
                            [[maybe_unused]] HitRecord *record) {
  return false;
}

__device__ bool cuda_hit_spheres(const SphereBuffer *spheres, Ray ray,
                                 Interval interval, HitRecord *record) {
  return hit_spheres(spheres, ray, interval, record);
}
