#define SDL_MAIN_USE_CALLBACKS 1

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "config.hpp"
#include "cpu_render.hpp"
#include "cuda_render.hpp"
#include "frame_buffer.hpp"
#include "sphere_buffer.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <memory>

static int times{0};
static bool cuda_on{true};

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
  std::unique_ptr<std::vector<Material>> materials;
  std::unique_ptr<SphereBuffer> spheres;
  std::unique_ptr<FrameBuffer> buffer;
  void *rng_states; // curandState*
};

void add_scene_one(SphereBuffer &spheres, std::vector<Material> &materials) {
  Lambertian red{Vec3{0.8f, 0.1f, 0.1f}};
  Lambertian dark{Vec3{0.15f, 0.15f, 0.15f}};
  Metal mirror{Vec3{0.9f, 0.9f, 0.9f}, 0.0f};
  Metal brushed{Vec3{0.57f, 0.79f, 0.53f}, 0.7f};
  Dielectric glass{1.5f};

  Material mat0{.type = MaterialType::Lambertian, .lambertian = red};
  Material mat1{.type = MaterialType::Lambertian, .lambertian = dark};
  Material mat2{.type = MaterialType::Metal, .metal = mirror};
  Material mat3{.type = MaterialType::Metal, .metal = brushed};
  Material mat4{.type = MaterialType::Dielectric, .dielectric = glass};

  materials.push_back(mat0); // 0: red diffuse
  materials.push_back(mat1); // 1: dark diffuse
  materials.push_back(mat2); // 2: mirror metal
  materials.push_back(mat3); // 3: brushed metal
  materials.push_back(mat4); // 4: glass

  spheres.add(0.0f, 0.0f, -3.0f, 1.0f, 0);      // red, center
  spheres.add(2.5f, 0.0f, -4.0f, 1.0f, 2);      // mirror, right
  spheres.add(-2.5f, 0.0f, -4.0f, 1.0f, 3);     // brushed, left
  spheres.add(0.0f, 2.5f, -2.0f, 0.5f, 4);      // glass, foreground
  spheres.add(0.0f, -101.0f, -3.0f, 100.0f, 1); // dark, ground sphere
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
  state->materials = std::make_unique<std::vector<Material>>();
  state->spheres = std::make_unique<SphereBuffer>();
  state->buffer = std::make_unique<FrameBuffer>();

  curand_malloc(&state->rng_states);
  cuda_rng_init(state->rng_states, SEED);

  add_scene_one(*state->spheres, *state->materials);
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
      state->camera->move_z(-speed);
      break;
    case SDLK_S:
      state->camera->move_z(speed);
      break;
    case SDLK_A:
      state->camera->move_x(-speed);
      break;
    case SDLK_D:
      state->camera->move_x(speed);
      break;
    case SDLK_R:
      state->camera->move_y(speed);
      break;
    case SDLK_F:
      state->camera->move_y(-speed);
      break;
    case SDLK_C:
      cuda_on = !cuda_on;
      break;
    }
    state->camera->init();
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  auto state{static_cast<AppState *>(appstate)};
  SDL_Log("Frame: %d", times);

  if (cuda_on) {
    cuda_render(state->camera.get(), state->spheres.get(),
                state->materials->data(), state->buffer.get(), WIDTH * HEIGHT);
  } else {
    cpu_render(*state->camera, *state->spheres, *state->materials,
               *state->buffer);
  }

  ++times;

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
