#include <SDL3/SDL_events.h>
#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "config.hpp"
#include "frame_buffer.hpp"
#include "render.hpp"
#include "sphere_buffer.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <memory>

struct SDL_Deleter {
  void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); }
  void operator()(SDL_Texture *t) const { SDL_DestroyTexture(t); }
};

struct AppState {
  std::unique_ptr<SDL_Window, SDL_Deleter> window;
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer;
  std::unique_ptr<SDL_Texture, SDL_Deleter> texture;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<SphereBuffer> spheres;
  std::unique_ptr<FrameBuffer> buffer;
};

void add_scene_one(SphereBuffer &spheres) {
  spheres.add(0.0f, 0.0f, -5.0f, 1.0f);
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

  SDL_Texture *t{SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT)};

  SDL_SetRenderVSync(r, 0);

  state->window.reset(w);
  state->renderer.reset(r);
  state->texture.reset(t);
  state->camera = std::make_unique<Camera>();
  state->spheres = std::make_unique<SphereBuffer>();

  add_scene_one(*state->spheres);

  state->buffer = std::make_unique<FrameBuffer>();

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
      state->camera->center.z() += speed;
      break;
    case SDLK_S:
      state->camera->center.z() -= speed;
      break;
    case SDLK_A:
      state->camera->center.x() += speed;
      break;
    case SDLK_D:
      state->camera->center.x() -= speed;
      break;
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto state{static_cast<AppState *>(appstate)};
  render(*state->camera, *state->spheres, *state->buffer);

  void *pixels{};
  int pitch{};

  if (SDL_LockTexture(state->texture.get(), NULL, &pixels, &pitch)) {
    std::memcpy(pixels, &state->buffer->pixels,
                static_cast<size_t>(pitch) * HEIGHT);
    SDL_UnlockTexture(state->texture.get());
  }

  SDL_RenderTexture(state->renderer.get(), state->texture.get(), NULL, NULL);
  SDL_RenderPresent(state->renderer.get());
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result) {
  if (appstate != nullptr) {
    std::unique_ptr<AppState> state{static_cast<AppState *>(appstate)};
  }

  SDL_Quit();
}
