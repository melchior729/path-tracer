#pragma once

#include "vec3.hpp"
#include <cstdint>

static constexpr auto BASE{255.0f};

struct Color {
  uint32_t val;

  constexpr HOSTDEV Color() : val(0xFF000000) {}

  constexpr HOSTDEV Color(float r_ch, float g_ch, float b_ch) {
    // adding 0.5f as float to int conversion will truncate, not round
    // 127.6 + 0.5 = 128 => rounds to 128, correct.
    auto r{static_cast<uint32_t>(BASE * r_ch + 0.5f)};
    auto g{static_cast<uint32_t>(BASE * g_ch + 0.5f)};
    auto b{static_cast<uint32_t>(BASE * b_ch + 0.5f)};
    val = 0xFFU << 24 | (r << 16) | (g << 8) | b;
  }

  constexpr Color(const Vec3 &v) : Color(v.x(), v.y(), v.z()) {}

  constexpr Color(uint32_t v) : val(v) {}

  constexpr operator uint32_t() const { return val; }

  constexpr uint32_t a() const { return (val >> 24) & 0xFF; };

  constexpr uint32_t r() const { return (val >> 16) & 0xFF; };

  constexpr uint32_t g() const { return (val >> 8) & 0xFF; };

  constexpr uint32_t b() const { return val & 0xFF; };
};

inline constexpr Color BLACK{0xFF000000};
