#pragma once

#ifdef __CUDACC__
#define HOSTDEV __host__ __device__
#else
#define HOSTDEV
#endif

#include <limits>

inline constexpr char TITLE[]{"Abhay's Path Tracer"};

inline constexpr auto WIDTH{1280};
inline constexpr auto HEIGHT{720};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{50};

inline constexpr std::size_t MAX_DEPTH{20};
inline constexpr std::size_t SEED{0};

inline constexpr auto INF{std::numeric_limits<float>::infinity()};
