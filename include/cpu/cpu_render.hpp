#pragma once

#include "shared/base/config.hpp"
#include "shared/base/state.hpp"
#include "shared/render/render.hpp"

inline void cpu_render(CPUState *cpu) {
  for (std::size_t j{}; j < HEIGHT; ++j) {
    for (std::size_t i{}; i < WIDTH; ++i) {
      render(cpu->camera.get(), cpu->spheres.get(),
             cpu->materials.get()->data(), cpu->buffer.get(), i, j,
             cpu->generator.get());
    }
  }
}
