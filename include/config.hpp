#pragma once

#include <limits>

inline constexpr char TITLE[]{"Abhay's Path Tracer"};

inline constexpr auto WIDTH{1280};
inline constexpr auto HEIGHT{720};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{10};

inline constexpr auto MAX_DEPTH{50};

inline constexpr auto INFINITY{std::numeric_limits<float>::infinity()};
