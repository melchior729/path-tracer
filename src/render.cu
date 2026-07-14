#include "config.hpp"
#include "cuda_render.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "render.cuh"
#include "sphere_buffer.hpp"
#include <cassert>
#include <curand_kernel.h>
#include <stdio.h>

static constexpr auto BLOCK_DIM{8};
static constexpr auto BLOCK_WIDTH{WIDTH / BLOCK_DIM};
static constexpr auto BLOCK_HEIGHT{HEIGHT / BLOCK_DIM};

static constexpr dim3 THREADS_PER_BLOCK{BLOCK_DIM, BLOCK_DIM};
static constexpr dim3 NUM_BLOCKS{BLOCK_WIDTH, BLOCK_HEIGHT};

__global__ void check_and_render(const Camera *camera,
                                 const SphereBuffer *spheres,
                                 const Material *materials, FrameBuffer *buffer,
                                 curandState *generator) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  size_t j{blockDim.y * blockIdx.y + threadIdx.y};
  if (i >= WIDTH || j >= HEIGHT) {
    return;
  }

  render(camera, spheres, materials, buffer, i, j, generator);
}

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, FrameBuffer *buffer,
                 void *generator) {
  check_and_render<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      camera, spheres, materials, buffer, (curandState *)generator);
  CUDA_CHECK(cudaDeviceSynchronize());
}

template <typename T> static void upload_arr(T **dest, T *src, size_t N) {
  auto T_bytes_per_arr{N * sizeof(T)};
  CUDA_CHECK(cudaMalloc((void **)dest, T_bytes_per_arr));
  CUDA_CHECK(cudaMemcpy(*dest, src, T_bytes_per_arr, cudaMemcpyHostToDevice));
}

static __global__ void set_gpu_sphere_members(SphereBuffer *gpu_spheres,
                                              float *d_x, float *d_y,
                                              float *d_z, float *d_r,
                                              size_t *d_m, size_t size) {
  gpu_spheres->center_x = d_x;
  gpu_spheres->center_y = d_y;
  gpu_spheres->center_z = d_z;
  gpu_spheres->radii = d_r;
  gpu_spheres->materials = d_m;
  gpu_spheres->size = size;
}

void move_to_device(SphereBuffer *cpu_spheres, SphereBuffer **gpu_spheres,
                    Material *cpu_materials, Material **gpu_materials,
                    Camera **gpu_camera, FrameBuffer **buffer) {
  float *d_x;
  float *d_y;
  float *d_z;
  float *d_r;
  size_t *d_m;

  auto size{cpu_spheres->size};
  upload_arr(&d_x, cpu_spheres->center_x, size);
  upload_arr(&d_y, cpu_spheres->center_y, size);
  upload_arr(&d_z, cpu_spheres->center_z, size);
  upload_arr(&d_r, cpu_spheres->radii, size);
  upload_arr(&d_m, cpu_spheres->materials, size);
  upload_arr(gpu_materials, cpu_materials, size);

  CUDA_CHECK(cudaMalloc((void **)gpu_spheres, sizeof(SphereBuffer)));
  set_gpu_sphere_members<<<1, 1>>>(*gpu_spheres, d_x, d_y, d_z, d_r, d_m, size);
  CUDA_CHECK(cudaMalloc((void **)gpu_camera, sizeof(Camera)));
  CUDA_CHECK(cudaMalloc((void **)buffer, sizeof(FrameBuffer)));
}

void move_fb_to_host(FrameBuffer *b, FrameBuffer *d_b) {
  CUDA_CHECK(cudaMemcpy(b, d_b, sizeof(FrameBuffer), cudaMemcpyDeviceToHost));
}

void cuda_copy_camera_to_device(Camera *d_c, Camera *c) {
  CUDA_CHECK(cudaMemcpy(d_c, c, sizeof(Camera), cudaMemcpyHostToDevice));
}

void curand_malloc(void **state) {
  CUDA_CHECK(cudaMalloc(state, sizeof(curandState) * BLOCK_WIDTH *
                                   BLOCK_HEIGHT * BLOCK_DIM));
}

__global__ void rng_init(curandState *rng, size_t seed) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  curand_init(seed, i, 0, &rng[i]);
}

void cuda_rng_init(void *state, size_t seed) {
  rng_init<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>((curandState *)state, seed);
}

__device__ float rand_float(curandState *generator) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  auto state{generator[i]};
  auto rand{curand_uniform(&state)};
  generator[i] = state;
  return rand;
}
