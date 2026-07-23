#include "shared/base/config.hpp"
#include <curand_kernel.h>

__device__ float gpu_rand_float(curandState *generator) {
  return curand_uniform(generator);
}
