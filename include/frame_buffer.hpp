#pragma once

#include "color.hpp"
#include "config.hpp"
#include <array>

struct FrameBuffer {
  std::array<Color, WIDTH * HEIGHT> pixels{};

  constexpr FrameBuffer() { clear(); }

  constexpr void set(std::size_t i, Color color) {
    if (!in_bounds(i)) {
      return;
    }

    pixels[i] = color;
  }

  constexpr void set(std::size_t x, std::size_t y, Color color) {
    if (x >= WIDTH || y >= HEIGHT) {
      return;
    }

    set(y * WIDTH + x, color);
  }

  constexpr void clear() { pixels.fill(BLACK); }

  constexpr bool in_bounds(std::size_t i) const { return i < WIDTH * HEIGHT; }
};
