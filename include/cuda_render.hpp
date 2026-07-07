#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"

#define CUDA_CHECK(call)                                                       \
  do {                                                                         \
    cudaError_t err = call;                                                    \
    if (err != cudaSuccess) {                                                  \
      fprintf(stderr, "%s:%d CUDA error: %s\n", __FILE__, __LINE__,            \
              cudaGetErrorString(err));                                        \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, void *rng, FrameBuffer *buffer,
                 size_t width, size_t height);

void move_array_to_device(float **x, float **y, float **z, float **r,
                          size_t **m, size_t count);

void curand_malloc(void **state);

void cuda_rng_init(void *state, size_t seed);
