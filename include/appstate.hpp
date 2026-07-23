#pragma once

#include "gpu/gpu_memory.hpp"
#include "scenes/bvh_stress_scene.hpp"      // IWYU pragma: keep
#include "scenes/massive_sphere_scenes.hpp" // IWYU pragma: keep
#include "scenes/three_spheres.hpp"
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
  CPUState cpu;
  GPUState gpu;
};

using SceneFactory = std::vector<Sphere> (*)(std::vector<Material> &);

inline void load_scene_cpu(CPUState &cpu, SceneFactory scene) {
  auto materials{std::make_unique<std::vector<Material>>()};
  auto spheres{scene(*materials)};
  auto tree{std::make_unique<BVHTree>(spheres)};
  auto sphere_buffer{std::make_unique<SphereBuffer>(std::move(spheres))};

  cpu.mat_size = materials->size();
  cpu.materials = std::move(materials);
  cpu.tree = std::move(tree);
  cpu.spheres = std::move(sphere_buffer);
}

inline void set_scene(AppState &state, SceneFactory scene) {
  load_scene_cpu(state.cpu, scene);
  upload_scene_to_gpu(&state.gpu, &state.cpu);
}

inline void init_appstate_cpu(AppState &state) {
  state.cpu.camera = std::make_unique<Camera>();
  state.cpu.buffer = std::make_unique<FrameBuffer>();
  state.cpu.generator = std::make_unique<std::mt19937>();
  load_scene_cpu(state.cpu, three_spheres);
}

inline void init_appstate_gpu(AppState &state) {
  init_gpu_state(&state.gpu, &state.cpu);
  copy_cam_to_gpu(state.gpu.camera, state.cpu.camera.get());
  init_gpu_rng(&state.gpu.generator, SEED);
}
