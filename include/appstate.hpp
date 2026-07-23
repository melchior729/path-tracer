#pragma once

#include "gpu/gpu_memory.hpp"
#include "scenes/complex_scene.hpp" // IWYU pragma: keep
#include "scenes/simple_scene.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

struct SDL_Deleter {
  void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); }
  void operator()(SDL_Texture *t) const { SDL_DestroyTexture(t); }
};

struct AppState {
  std::unique_ptr<SDL_Window, SDL_Deleter> window;
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer;
  std::unique_ptr<SDL_Texture, SDL_Deleter> texture;
  // might want to have these as pointers when
  // number of spheres increase
  CPUState cpu;
  GPUState gpu;
};

inline void init_appstate_cpu(AppState &state) {
  state.cpu.camera = std::make_unique<Camera>();
  state.cpu.materials = std::make_unique<std::vector<Material>>();
  auto spheres{complex_scene(*state.cpu.materials.get())};
  state.cpu.tree = std::make_unique<BVHTree>(spheres);
  state.cpu.spheres = std::make_unique<SphereBuffer>(std::move(spheres));
  state.cpu.buffer = std::make_unique<FrameBuffer>();
  state.cpu.generator = std::make_unique<std::mt19937>();
  state.cpu.mat_size = state.cpu.materials->size();
}

inline void init_appstate_gpu(AppState &state, size_t mat_size) {
  init_gpu_state(&state.gpu, &state.cpu);
  copy_cam_to_gpu(state.gpu.camera, state.cpu.camera.get());
  init_gpu_rng(&state.gpu.generator, SEED);
  state.gpu.mat_size = mat_size;
}
