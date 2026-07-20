#pragma once

#include <curand_kernel.h>

__device__ float gpu_rand_float(curandState *generator);
