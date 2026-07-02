#pragma once

#include <limits>

inline constexpr char TITLE[]{"Abhay's Path Tracer"};

inline constexpr auto WIDTH{320};
inline constexpr auto HEIGHT{180};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{20};

inline constexpr std::size_t MAX_DEPTH{20};

inline constexpr auto INF{std::numeric_limits<float>::infinity()};
