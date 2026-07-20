#include "gpu/cuda_commands.hpp"
#include "gpu/cuda_constants.cuh"
#include "gpu/gpu_memory.hpp"
#include "shared/base/state.hpp"
#include <curand_kernel.h>

static __global__ void init_gpu_sphere_buffer(SphereBuffer *spheres, float *d_x,
                                              float *d_y, float *d_z,
                                              float *d_r, size_t *d_m,
                                              size_t size) {
  spheres->center_x = d_x;
  spheres->center_y = d_y;
  spheres->center_z = d_z;
  spheres->radii = d_r;
  spheres->materials = d_m;
  spheres->size = size;
}

template <typename T> static void copy_to_gpu(T **dest, T *src, size_t N) {
  auto T_bytes_per_arr{N * sizeof(T)};
  CUDA_CHECK(cudaMalloc((void **)dest, T_bytes_per_arr));
  CUDA_CHECK(cudaMemcpy(*dest, src, T_bytes_per_arr, cudaMemcpyHostToDevice));
}

void init_gpu_state(GPUState *gpu, const CPUState *cpu) {
  float *d_x;
  float *d_y;
  float *d_z;
  float *d_r;
  size_t *d_m;

  auto size{cpu->spheres->size};
  copy_to_gpu(&d_x, cpu->spheres->center_x, size);
  copy_to_gpu(&d_y, cpu->spheres->center_y, size);
  copy_to_gpu(&d_z, cpu->spheres->center_z, size);
  copy_to_gpu(&d_r, cpu->spheres->radii, size);
  copy_to_gpu(&d_m, cpu->spheres->materials, size);
  copy_to_gpu(&gpu->materials, cpu->materials->data(), cpu->mat_size);

  CUDA_CHECK(cudaMalloc((void **)&gpu->spheres, sizeof(SphereBuffer)));
  CUDA_CHECK(cudaMalloc((void **)&gpu->camera, sizeof(Camera)));
  CUDA_CHECK(cudaMalloc((void **)&gpu->buffer, sizeof(FrameBuffer)));

  init_gpu_sphere_buffer<<<1, 1>>>(gpu->spheres, d_x, d_y, d_z, d_r, d_m, size);
}

void move_fb_to_cpu(FrameBuffer *b, const FrameBuffer *d_b) {
  CUDA_CHECK(cudaMemcpy(b, d_b, sizeof(FrameBuffer), cudaMemcpyDeviceToHost));
}

void copy_cam_to_gpu(Camera *d_c, const Camera *c) {
  CUDA_CHECK(cudaMemcpy(d_c, c, sizeof(Camera), cudaMemcpyHostToDevice));
}

static __global__ void rng_init(curandState *rng, size_t seed) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  curand_init(seed, i, 0, &rng[i]);
}

void init_gpu_rng(void **state, size_t seed) {
  CUDA_CHECK(cudaMalloc(state, sizeof(curandState) * BLOCK_WIDTH *
                                   BLOCK_HEIGHT * BLOCK_DIM));
  rng_init<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      static_cast<curandState *>(*state), seed);
}
