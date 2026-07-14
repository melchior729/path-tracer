#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "config.hpp"
#include "cpu_render.hpp"
#include "cuda_render.hpp"
#include "frame_buffer.hpp"
#include "nvtx3/nvtx3.hpp"
#include "scenes.hpp"
#include "sphere_buffer.hpp"
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
  std::unique_ptr<Camera> cpu_camera;
  Camera *gpu_camera;
  std::unique_ptr<std::vector<Material>> cpu_materials;
  Material *gpu_materials;
  std::unique_ptr<SphereBuffer> cpu_spheres;
  SphereBuffer *gpu_spheres;
  std::unique_ptr<FrameBuffer> cpu_buffer;
  FrameBuffer *gpu_buffer;
  void *rng_states; // curandState*
};

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

  SDL_Texture *t{SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT)};

  SDL_SetRenderVSync(r, 0);

  state->window.reset(w);
  state->renderer.reset(r);
  state->texture.reset(t);

  state->cpu_camera = std::make_unique<Camera>();

  state->cpu_materials = std::make_unique<std::vector<Material>>();
  auto spheres{simple_scene(*state->cpu_materials.get())};
  state->cpu_spheres = std::make_unique<SphereBuffer>(spheres);
  state->cpu_buffer = std::make_unique<FrameBuffer>();

  move_to_device(state->cpu_spheres.get(), &state->gpu_spheres,
                 state->cpu_materials.get()->data(), &state->gpu_materials,
                 &state->gpu_camera, &state->gpu_buffer);

  curand_malloc(&state->rng_states);
  cuda_rng_init(state->rng_states, SEED);

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
      state->cpu_camera->move_z(-speed);
      break;
    case SDLK_S:
      state->cpu_camera->move_z(speed);
      break;
    case SDLK_A:
      state->cpu_camera->move_x(-speed);
      break;
    case SDLK_D:
      state->cpu_camera->move_x(speed);
      break;
    case SDLK_R:
      state->cpu_camera->move_y(speed);
      break;
    case SDLK_F:
      state->cpu_camera->move_y(-speed);
      break;
    case SDLK_H:
      state->cpu_camera->place_center();
      break;
    case SDLK_C:
      cuda_on = !cuda_on;
      break;
    case SDLK_I:
      show_overlay = !show_overlay;
      break;
    }
  }

  state->cpu_camera->init();
  cuda_copy_camera_to_device(state->gpu_camera, state->cpu_camera.get());
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
    cuda_render(state->gpu_camera, state->gpu_spheres, state->gpu_materials,
                state->rng_states, state->gpu_buffer, WIDTH, HEIGHT);
    move_fb_to_host(state->cpu_buffer.get(), state->gpu_buffer);
  } else {
    cpu_render(*state->cpu_camera, *state->cpu_spheres, *state->cpu_materials,
               *state->cpu_buffer);
  }

  void *pixels{};
  int pitch{};

  if (SDL_LockTexture(state->texture.get(), NULL, &pixels, &pitch)) {
    std::memcpy(pixels, &state->cpu_buffer->pixels,
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
