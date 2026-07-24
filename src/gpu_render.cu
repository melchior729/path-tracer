#include "gpu/cuda_commands.hpp"
#include "gpu/cuda_constants.cuh"
#include "shared/base/state.hpp"
#include "shared/render/render.hpp"
#include <curand_kernel.h>

__global__ void render_kernel(const Camera *__restrict__ camera,
                              const SphereBuffer *__restrict__ spheres,
                              const Material *__restrict__ materials,
                              FrameBuffer *__restrict__ buffer,
                              const BVHTree *__restrict__ tree,
                              curandState *__restrict__ generator) {
  size_t x{blockDim.x * blockIdx.x + threadIdx.x};
  size_t y{blockDim.y * blockIdx.y + threadIdx.y};
  if (x >= WIDTH || y >= HEIGHT) {
    return;
  }

  size_t i{y * WIDTH + x};
  curandState local_state{generator[i]};

  render(camera, spheres, materials, buffer, x, y, tree, &local_state);

  generator[i] = local_state;
}

void gpu_render(GPUState *gpu) {
  render_kernel<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      gpu->camera, gpu->spheres, gpu->materials, gpu->buffer, gpu->tree,
      (curandState *)gpu->generator);
  CUDA_CHECK(cudaDeviceSynchronize());
}
