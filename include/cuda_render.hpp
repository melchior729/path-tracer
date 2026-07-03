#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"

#ifdef __CUDACC__
#include <curand_kernel.h>
#else
struct curandState;
#endif

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, FrameBuffer *buffer,
                 size_t buff_size);

void cuda_rng_init(curandState *state, size_t seed);
