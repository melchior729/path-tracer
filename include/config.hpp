#pragma once

#ifdef __CUDACC__
#define HOSTDEV __host__ __device__
#else
#define HOSTDEV
#endif

#include <limits>

inline constexpr char TITLE[]{"Abhay's Path Tracer"};

inline constexpr auto WIDTH{320};
inline constexpr auto HEIGHT{180};
inline constexpr auto ASPECT{static_cast<float>(WIDTH) / HEIGHT};

inline constexpr auto FOV{90.0f};
inline constexpr auto SAMPLE_COUNT{20};

inline constexpr std::size_t MAX_DEPTH{20};
inline constexpr std::size_t SEED{0};

inline constexpr auto INF{std::numeric_limits<float>::infinity()};

static constexpr std::size_t THREADS_PER_BLOCK{256};
static constexpr std::size_t NUM_BLOCKS{WIDTH * HEIGHT / THREADS_PER_BLOCK};
