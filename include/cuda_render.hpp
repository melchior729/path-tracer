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

#define CUDA_PROFILE(call, region_name)                                        \
  nvtxRangePush(region_name);                                                  \
  call();                                                                      \
  nvtxRangePop();

void move_to_device(SphereBuffer *cpu_spheres, SphereBuffer **gpu_spheres,
                    Material *cpu_materials, Material **gpu_materials,
                    Camera **gpu_camera, FrameBuffer **buffer);

void cuda_copy_camera_to_device(Camera *d_c, Camera *c);

void move_fb_to_host(FrameBuffer *b, FrameBuffer *d_b);

void curand_malloc(void **state);

void cuda_rng_init(void *state, size_t seed);
