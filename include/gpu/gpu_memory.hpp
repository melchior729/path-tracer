#pragma once

#include "shared/base/state.hpp"

void init_gpu_state(GPUState *gpu, const CPUState *cpu);

void upload_scene_to_gpu(GPUState *gpu, const CPUState *cpu);

void destroy_gpu_state(GPUState *gpu);

void init_gpu_rng(void **state, size_t seed);

void copy_cam_to_gpu(Camera *d_c, const Camera *c);

void move_fb_to_cpu(FrameBuffer *b, const FrameBuffer *d_b);
