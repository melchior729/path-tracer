#include "camera.hpp"
#include "config.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"
#include "vec3.hpp"
#include <curand_kernel.h>

__global__ void cuda_rng_init(curandState *state, size_t seed) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  curand_init(seed, i, 0, &state[i]);
}

// __device__ static float device_random_float() { return 0.0f; }
//
// __device__ static float device_random_float(float min, float max) {
//   return min < max ? min : max;
// }

__device__ static Vec3 device_sample_square() { return {}; }

__device__ static Vec3
device_ray_color([[maybe_unused]] const Ray *ray,
                 [[maybe_unused]] const SphereBuffer *spheres,
                 [[maybe_unused]] const Material *materials,
                 [[maybe_unused]] size_t depth) {
  return {};
}

__device__ static float device_gamma(float f) { return f > 0 ? sqrt(f) : 0; }

__device__ static Vec3 device_gamma_vec(const Vec3 *v) {
  static constexpr Interval intensity{0.000, 0.999};
  auto r_byte{intensity.clamp(device_gamma(v->x()))};
  auto g_byte{intensity.clamp(device_gamma(v->y()))};
  auto b_byte{intensity.clamp(device_gamma(v->z()))};
  return {r_byte, g_byte, b_byte};
}

__global__ void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                            const Material *materials, FrameBuffer *buffer,
                            size_t buff_size) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  if (i >= buff_size) {
    return;
  }

  for (size_t j{}; j < HEIGHT; ++j) {
    for (size_t i{}; i < WIDTH; ++i) {
      Vec3 col;
      for (size_t s{}; s < SAMPLE_COUNT; ++s) {
        auto ray{camera->get_ray(device_sample_square(), i, j)};
        col += device_ray_color(&ray, spheres, materials, MAX_DEPTH);
      }

      col /= SAMPLE_COUNT;
      auto writtable{device_gamma_vec(&col)};
      buffer->set(j, i, Color{writtable});
    }
  }
}
