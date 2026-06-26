#pragma once

#include <limits>

inline constexpr char TITLE[]{"Abhay's Path Tracer"};

inline constexpr auto WIDTH{640};
inline constexpr auto HEIGHT{360};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{10};

inline constexpr auto MAX_DEPTH{5};

inline constexpr auto INFINITY{std::numeric_limits<float>::infinity()};
