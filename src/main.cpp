#define SDL_MAIN_USE_CALLBACKS 1
#define LOGGING_ENABLED 1

#if LOGGING_ENABLED
#include <SDL3/SDL_log.h>
#endif

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "config.hpp"
#include "cuda_render.hpp"
#include "frame_buffer.hpp"
#include "nvtx3/nvtx3.hpp"
#include "render.cuh"
#include "scenes.hpp"
#include "sphere_buffer.hpp"
#include "state.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <memory>

static bool cuda_on{true};
static bool show_overlay{true};
static uint64_t last_time{};

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

void init_appstate_cpu(AppState &state) {
  state.cpu.camera = std::make_unique<Camera>();
  state.cpu.materials = std::make_unique<std::vector<Material>>();
  auto spheres{simple_scene(*state.cpu.materials.get())};
  state.cpu.spheres = std::make_unique<SphereBuffer>(spheres);
  state.cpu.buffer = std::make_unique<FrameBuffer>();
  state.cpu.generator = std::make_unique<std::mt19937>();
}

void init_appstate_gpu(AppState &state) {
  init_gpu_state(&state.gpu, &state.cpu);
  copy_cam_to_gpu(state.gpu.camera, state.cpu.camera.get());
  init_gpu_rng(&state.gpu.generator, SEED);
}

SDL_AppResult SDL_AppInit(void **appstate, [[maybe_unused]] int argc,
                          [[maybe_unused]] char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  auto state{std::make_unique<AppState>()};
  SDL_Window *w{};
  SDL_Renderer *r{};

  if (!SDL_CreateWindowAndRenderer(TITLE, WIDTH, HEIGHT, 0, &w, &r)) {
    return SDL_APP_FAILURE;
  }

  state->window.reset(w);
  state->renderer.reset(r);

  SDL_Texture *t{SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT)};
  state->texture.reset(t);

  init_appstate_cpu(*state.get());
  init_appstate_gpu(*state.get());

  auto VSYNC_STATUS{false};
  SDL_SetRenderVSync(r, VSYNC_STATUS);

  *appstate = state.release();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  auto state{static_cast<AppState *>(appstate)};

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  constexpr auto speed{0.2f};
  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.key) {
    case SDLK_W:
      state->cpu.camera->move_z(-speed);
      break;
    case SDLK_S:
      state->cpu.camera->move_z(speed);
      break;
    case SDLK_A:
      state->cpu.camera->move_x(-speed);
      break;
    case SDLK_D:
      state->cpu.camera->move_x(speed);
      break;
    case SDLK_R:
      state->cpu.camera->move_y(speed);
      break;
    case SDLK_F:
      state->cpu.camera->move_y(-speed);
      break;
    case SDLK_H:
      state->cpu.camera->move_to_origin();
      state->cpu.camera->look_neg_z();
      break;
    case SDLK_C:
      cuda_on = !cuda_on;
      break;
    case SDLK_I:
      show_overlay = !show_overlay;
      break;
    }

    state->cpu.camera->init_viewport();
    copy_cam_to_gpu(state->gpu.camera, state->cpu.camera.get());
  }

  return SDL_APP_CONTINUE;
}

static void draw_overlay(AppState *state) {
  if (!show_overlay) {
    return;
  }

  uint64_t now{SDL_GetPerformanceCounter()};
  double frame_ms = (double)(now - last_time) * 1000 /
                    static_cast<double>(SDL_GetPerformanceFrequency());
  last_time = now;

  char line[64];
  SDL_snprintf(line, sizeof(line), "%.2f ms | %.0f fps", frame_ms,
               1000.0 / frame_ms);
  SDL_SetRenderDrawColor(state->renderer.get(), 0x0, 0x0, 0x0, 0xFF);
  constexpr float overlay_scale{2.0f};
  SDL_SetRenderScale(state->renderer.get(), overlay_scale, overlay_scale);
  SDL_RenderDebugText(state->renderer.get(), 4.0f, 4.0f, line);
  SDL_SetRenderScale(state->renderer.get(), 1.0f, 1.0f);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto state{static_cast<AppState *>(appstate)};

  if (cuda_on) {
    gpu_render(&state->gpu);
    move_fb_to_cpu(state->cpu.buffer.get(), state->gpu.buffer);
  } else {
    cpu_render(&state->cpu);
  }

  void *pixels{};
  int pitch{};

  if (SDL_LockTexture(state->texture.get(), NULL, &pixels, &pitch)) {
    // want to write to the address of where its coming from
    // this is cpu's buffer @ pixels.

    std::memcpy(pixels, &(state->cpu.buffer->pixels),
                static_cast<size_t>(pitch) * HEIGHT);
    SDL_UnlockTexture(state->texture.get());
  }

  SDL_RenderTexture(state->renderer.get(), state->texture.get(), NULL, NULL);
  draw_overlay(state);
  SDL_RenderPresent(state->renderer.get());

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result) {
  if (appstate != nullptr) {
    std::unique_ptr<AppState> state{static_cast<AppState *>(appstate)};
  }

  SDL_Quit();
}
