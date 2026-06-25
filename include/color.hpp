#pragma once

#include <cstdint>

struct Color {
  uint32_t val;

  constexpr Color() : val(0xFF000000) {}

  constexpr Color(uint32_t v) : val(v) {}

  constexpr operator uint32_t() const { return val; }

  constexpr uint32_t a() const { return (val >> 24) & 0xFF; };

  constexpr uint32_t r() const { return (val >> 16) & 0xFF; };

  constexpr uint32_t g() const { return (val >> 8) & 0xFF; };

  constexpr uint32_t b() const { return val & 0xFF; };
};

inline constexpr Color WHITE{0xFFFFFFFF};
inline constexpr Color BLACK{0xFF000000};
inline constexpr Color RED{0xFFFF0000};
inline constexpr Color GREEN{0xFF00FF00};
inline constexpr Color BLUE{0xFF0000FF};
