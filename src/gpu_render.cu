#include "gpu/cuda_commands.hpp"
#include "gpu/cuda_constants.cuh"
#include "shared/base/state.hpp"
#include "shared/render/render.hpp"
#include <curand_kernel.h>

__global__ void render_kernel(const Camera *camera, const SphereBuffer *spheres,
                              const Material *materials, FrameBuffer *buffer,
                              curandState *generator) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  size_t j{blockDim.y * blockIdx.y + threadIdx.y};
  if (i >= WIDTH || j >= HEIGHT) {
    return;
  }

  render(camera, spheres, materials, buffer, i, j, generator);
}

void gpu_render(GPUState *gpu) {
  render_kernel<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      gpu->camera, gpu->spheres, gpu->materials, gpu->buffer,
      (curandState *)gpu->generator);
  CUDA_CHECK(cudaDeviceSynchronize());
}
