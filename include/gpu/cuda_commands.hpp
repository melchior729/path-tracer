#pragma once

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
