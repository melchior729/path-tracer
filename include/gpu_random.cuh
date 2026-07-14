#pragma once

#include <curand_kernel.h>

__device__ float rand_float(curandState *generator);
