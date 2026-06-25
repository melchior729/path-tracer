#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "sphere_buffer.hpp"
#include "util.hpp"
#include "vec3.hpp"

static Vec3 sample_square() {
  return {random_float() - 0.5f, random_float() - 0.5f, 0};
}

inline void render([[maybe_unused]] const Camera &camera,
                   [[maybe_unused]] const SphereBuffer &spheres,
                   [[maybe_unused]] FrameBuffer &buffer) {

  // loop from j = 0 to height
  // loop from i = 0 to width
  //
  //
  // get the ray using camera.get_ray() & sample_square
  // compute the color by doing the depth loop in the world
  //
  //
  // after getting the color, convert to uint32_t / color and then do buffer.set

  sample_square();
}
