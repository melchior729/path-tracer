#include <curand_kernel.h>

__device__ float gpu_rand_float(curandState *generator) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  auto state{generator[i]};
  auto rand{curand_uniform(&state)};
  generator[i] = state;
  return rand;
}
