#pragma once

#ifdef __CUDACC__
#define HOSTDEV __host__ __device__
#else
#define HOSTDEV
#endif

#include <limits>

inline constexpr auto WIDTH{1280};
inline constexpr auto HEIGHT{720};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{10};

inline constexpr std::size_t MAX_DEPTH{8};
inline constexpr std::size_t SEED{0};

inline constexpr auto INF{std::numeric_limits<float>::infinity()};
inline constexpr char TITLE[]{"Abhay's Path Tracer"};
