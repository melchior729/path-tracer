#include "camera.hpp"
#include "config.hpp"
#include "frame_buffer.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"
#include <__clang_cuda_builtin_vars.h>
#include <__clang_cuda_runtime_wrapper.h>

__device__ static float device_random_float();

__device__ static Vec3 sample_square();

__device__ static Vec3 ray_color(const Ray *ray, const SphereBuffer *spheres,
                                 const Material *materials, size_t depth);

__device__ static float gamma(float f) { return f > 0 ? sqrt(f) : 0; }

__device__ static Vec3 gamma_vec(const Vec3 *v) {
  static constexpr Interval intensity{0.000, 0.999};
  auto r_byte{intensity.clamp(gamma(v->x()))};
  auto g_byte{intensity.clamp(gamma(v->y()))};
  auto b_byte{intensity.clamp(gamma(v->z()))};
  return {r_byte, g_byte, b_byte};
}

__global__ void render(const Camera *camera, const SphereBuffer *spheres,
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
        auto ray{camera->get_ray(sample_square(), i, j)};
        col += ray_color(&ray, spheres, materials, MAX_DEPTH);
      }

      col /= SAMPLE_COUNT;
      auto writtable{gamma_vec(&col)};
      buffer->set(j, i, Color{writtable});
    }
  }
}
