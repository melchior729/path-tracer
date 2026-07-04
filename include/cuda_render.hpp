#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, void *rng, FrameBuffer *buffer,
                 size_t buff_size);

void curand_malloc(void **state);

void cuda_rng_init(void *state, size_t seed);
