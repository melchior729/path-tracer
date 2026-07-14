#include "config.hpp"
#include "cuda_hit.cuh"
#include "cuda_render.hpp"
#include "frame_buffer.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "sphere_buffer.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include <cassert>
#include <curand_kernel.h>
#include <stdio.h>

static constexpr auto BLOCK_DIM{8};
static constexpr auto BLOCK_WIDTH{WIDTH / BLOCK_DIM};
static constexpr auto BLOCK_HEIGHT{HEIGHT / BLOCK_DIM};

static constexpr dim3 THREADS_PER_BLOCK{BLOCK_DIM, BLOCK_DIM};
static constexpr dim3 NUM_BLOCKS{BLOCK_WIDTH, BLOCK_HEIGHT};

__global__ void rng_init(curandState *rng, size_t seed) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  curand_init(seed, i, 0, &rng[i]);
}

__device__ static float random_float(curandState *rng) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  auto state{rng[i]};
  auto rand{curand_uniform(&state)};
  rng[i] = state;
  return rand;
}

__device__ static Vec3 ray_color(Ray ray, const SphereBuffer *spheres,
                                 const Material *materials, curandState *rng,
                                 size_t depth) {
  Ray incoming{ray};
  Vec3 acc_attenuation{WHITE};

  for (size_t i{}; i < depth; ++i) {
    HitRecord record;
    if (!cuda_hit_spheres(spheres, incoming, Interval{0.001, INF}, &record)) {
      return mix_with_sky(incoming, acc_attenuation);
    }

    Ray scattered;
    Vec3 attenuation;
    auto material{materials[record.mat_idx]};
    auto rand_float{random_float(rng)};
    Vec3 rand_vec{rand_float, random_float(rng), random_float(rng)};

    switch (material.type) {
    case MaterialType::Lambertian:
      material.lambertian.scatter(record, rand_vec, attenuation, scattered);
      break;
    case MaterialType::Metal:
      if (!material.metal.scatter(incoming, record, rand_vec, attenuation,
                                  scattered)) {
        return mix_with_sky(incoming, acc_attenuation);
      }
      break;
    case MaterialType::Dielectric:
      if (!material.dielectric.scatter(incoming, record, rand_float,
                                       attenuation, scattered)) {
        return mix_with_sky(incoming, acc_attenuation);
      }
      break;
    default:
      assert(false);
    }

    incoming = scattered;
    acc_attenuation *= attenuation;
  }

  return {};
}

__global__ void render(const Camera *camera, const SphereBuffer *spheres,
                       const Material *materials, curandState *rng,
                       FrameBuffer *buffer, size_t width, size_t height) {
  size_t i{blockDim.x * blockIdx.x + threadIdx.x};
  size_t j{blockDim.y * blockIdx.y + threadIdx.y};
  if (i >= width || j >= height) {
    return;
  }

  Vec3 col;
  for (size_t s{}; s < SAMPLE_COUNT; ++s) {
    auto ray{camera->get_ray(
        sample_square(random_float(rng), random_float(rng)), i, j)};
    col += ray_color(ray, spheres, materials, rng, MAX_DEPTH);
  }

  col /= SAMPLE_COUNT;
  auto writtable{gamma_vec(col)};
  buffer->set(i, j, Color{writtable});
}

void curand_malloc(void **state) {
  CUDA_CHECK(cudaMalloc(state, sizeof(curandState) * BLOCK_WIDTH *
                                   BLOCK_HEIGHT * BLOCK_DIM));
}

void cuda_rng_init(void *state, size_t seed) {
  rng_init<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>((curandState *)state, seed);
}

void cuda_render(const Camera *camera, const SphereBuffer *spheres,
                 const Material *materials, void *rng, FrameBuffer *buffer,
                 size_t width, size_t height) {
  render<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(
      camera, spheres, materials, (curandState *)rng, buffer, width, height);
  CUDA_CHECK(cudaDeviceSynchronize());
}

template <typename T> static void upload_arr(T **dest, T *src, size_t N) {
  auto T_bytes_per_arr{N * sizeof(T)};
  CUDA_CHECK(cudaMalloc((void **)dest, T_bytes_per_arr));
  CUDA_CHECK(cudaMemcpy(*dest, src, T_bytes_per_arr, cudaMemcpyHostToDevice));
}

static __global__ void set_gpu_sphere_members(SphereBuffer *gpu_spheres,
                                              float *d_x, float *d_y,
                                              float *d_z, float *d_r,
                                              size_t *d_m, size_t size) {
  gpu_spheres->center_x = d_x;
  gpu_spheres->center_y = d_y;
  gpu_spheres->center_z = d_z;
  gpu_spheres->radii = d_r;
  gpu_spheres->materials = d_m;
  gpu_spheres->size = size;
}

void move_to_device(SphereBuffer *cpu_spheres, SphereBuffer **gpu_spheres,
                    FrameBuffer **buffer) {
  float *d_x;
  float *d_y;
  float *d_z;
  float *d_r;
  size_t *d_m;

  auto size{cpu_spheres->size};
  upload_arr(&d_x, cpu_spheres->center_x, size);
  upload_arr(&d_y, cpu_spheres->center_y, size);
  upload_arr(&d_z, cpu_spheres->center_z, size);
  upload_arr(&d_r, cpu_spheres->radii, size);
  upload_arr(&d_m, cpu_spheres->materials, size);

  CUDA_CHECK(cudaMalloc((void **)gpu_spheres, sizeof(SphereBuffer)));
  set_gpu_sphere_members<<<1, 1>>>(*gpu_spheres, d_x, d_y, d_z, d_r, d_m, size);

  CUDA_CHECK(cudaMalloc((void **)buffer, sizeof(FrameBuffer)));
}

void move_fb_to_host(FrameBuffer *b, FrameBuffer *d_b) {
  CUDA_CHECK(cudaMemcpy(b, d_b, sizeof(FrameBuffer), cudaMemcpyDeviceToHost));
}

Camera *cuda_malloc_camera() {
  Camera *camera;
  CUDA_CHECK(cudaMalloc((void **)&camera, sizeof(Camera)));
  return camera;
}

void cuda_copy_camera_to_device(Camera *d_c, Camera *c) {
  CUDA_CHECK(cudaMemcpy(d_c, c, sizeof(Camera), cudaMemcpyHostToDevice));
}
