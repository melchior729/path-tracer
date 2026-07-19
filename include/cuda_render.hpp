#pragma once

#include "camera.hpp"
#include "frame_buffer.hpp"
#include "state.hpp"

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

void init_gpu_state(GPUState *gpu, const CPUState *cpu);

void init_gpu_rng(void **state, size_t seed);

void copy_cam_to_gpu(Camera *d_c, const Camera *c);

void move_fb_to_cpu(FrameBuffer *b, const FrameBuffer *d_b);
