#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "config.hpp"
#include "frame_buffer.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <memory>

struct SDL_Deleter {
  void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); }
  void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); }
  void operator()(SDL_Texture *t) const { SDL_DestroyTexture(t); }
};

struct AppState {
  std::unique_ptr<SDL_Window, SDL_Deleter> window{};
  std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer{};
  std::unique_ptr<SDL_Texture, SDL_Deleter> texture{};
  std::unique_ptr<FrameBuffer> buffer{};
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
                                   SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT)};

  SDL_SetRenderVSync(r, 0);

  state->window.reset(w);
  state->renderer.reset(r);
  state->texture.reset(t);
  state->buffer = std::make_unique<FrameBuffer>();

  *appstate = state.release();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto state{static_cast<AppState *>(appstate)};
  state->buffer->clear();

  SDL_RenderPresent(state->renderer.get());
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result) {
  if (appstate != nullptr) {
    std::unique_ptr<AppState> state{static_cast<AppState *>(appstate)};
  }

  SDL_Quit();
}
