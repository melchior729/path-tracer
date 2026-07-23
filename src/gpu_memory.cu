#include "gpu/cuda_commands.hpp"
#include "gpu/cuda_constants.cuh"
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

static __global__ void init_bvh_tree(BVHTree *tree, BVHNode *root,
                                     size_t size) {
  tree->root = root;
  tree->size = size;
}

template <typename T> static void copy_to_gpu(T **dest, T *src, size_t N) {
  auto T_bytes_per_arr{N * sizeof(T)};
  CUDA_CHECK(cudaMalloc((void **)dest, T_bytes_per_arr));
  CUDA_CHECK(cudaMemcpy(*dest, src, T_bytes_per_arr, cudaMemcpyHostToDevice));
}

template <typename T> static void free_gpu_ptr(T *&ptr) {
  if (ptr != nullptr) {
    CUDA_CHECK(cudaFree(ptr));
    ptr = nullptr;
  }
}

static void destroy_gpu_scene(GPUState *gpu) {
  free_gpu_ptr(gpu->sphere_x);
  free_gpu_ptr(gpu->sphere_y);
  free_gpu_ptr(gpu->sphere_z);
  free_gpu_ptr(gpu->sphere_radii);
  free_gpu_ptr(gpu->sphere_materials);
  free_gpu_ptr(gpu->bvh_nodes);
  free_gpu_ptr(gpu->materials);
  gpu->mat_size = 0;
}

void upload_scene_to_gpu(GPUState *gpu, const CPUState *cpu) {
  destroy_gpu_scene(gpu);

  auto size{cpu->spheres->size};
  copy_to_gpu(&gpu->sphere_x, cpu->spheres->center_x, size);
  copy_to_gpu(&gpu->sphere_y, cpu->spheres->center_y, size);
  copy_to_gpu(&gpu->sphere_z, cpu->spheres->center_z, size);
  copy_to_gpu(&gpu->sphere_radii, cpu->spheres->radii, size);
  copy_to_gpu(&gpu->sphere_materials, cpu->spheres->materials, size);
  copy_to_gpu(&gpu->materials, cpu->materials->data(), cpu->mat_size);
  copy_to_gpu(&gpu->bvh_nodes, cpu->tree->root, 2 * size - 1);

  init_gpu_sphere_buffer<<<1, 1>>>(
      gpu->spheres, gpu->sphere_x, gpu->sphere_y, gpu->sphere_z,
      gpu->sphere_radii, gpu->sphere_materials, size);
  init_bvh_tree<<<1, 1>>>(gpu->tree, gpu->bvh_nodes, size);
  CUDA_CHECK(cudaGetLastError());
  CUDA_CHECK(cudaDeviceSynchronize());
  gpu->mat_size = cpu->mat_size;
}

void init_gpu_state(GPUState *gpu, const CPUState *cpu) {
  // CUDA_CHECK(cudaDeviceSetLimit(cudaLimitStackSize, 4096););

  CUDA_CHECK(cudaMalloc((void **)&gpu->spheres, sizeof(SphereBuffer)));
  CUDA_CHECK(cudaMalloc((void **)&gpu->camera, sizeof(Camera)));
  CUDA_CHECK(cudaMalloc((void **)&gpu->buffer, sizeof(FrameBuffer)));
  CUDA_CHECK(cudaMalloc((void **)&gpu->tree, sizeof(BVHTree)));

  upload_scene_to_gpu(gpu, cpu);
}

void destroy_gpu_state(GPUState *gpu) {
  destroy_gpu_scene(gpu);
  free_gpu_ptr(gpu->spheres);
  free_gpu_ptr(gpu->camera);
  free_gpu_ptr(gpu->buffer);
  free_gpu_ptr(gpu->tree);
  free_gpu_ptr(gpu->generator);
}

void move_fb_to_cpu(FrameBuffer *b, const FrameBuffer *d_b) {
  CUDA_CHECK(cudaMemcpy(b, d_b, sizeof(FrameBuffer), cudaMemcpyDeviceToHost));
}

void copy_cam_to_gpu(Camera *d_c, const Camera *c) {
  CUDA_CHECK(cudaMemcpy(d_c, c, sizeof(Camera), cudaMemcpyHostToDevice));
}

static __global__ void rng_init(curandState *rng, size_t seed) {
  size_t x{blockDim.x * blockIdx.x + threadIdx.x};
  size_t y{blockDim.y * blockIdx.y + threadIdx.y};
  size_t i{y * WIDTH + x};
  curand_init(seed + i, 0, 0, &rng[i]);
}

void init_gpu_rng(void **state, size_t seed) {
  CUDA_CHECK(cudaMalloc(state, sizeof(curandState) * WIDTH * HEIGHT));
  rng_init<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      static_cast<curandState *>(*state), seed);
}
