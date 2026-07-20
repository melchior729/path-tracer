#pragma once

#include "shared/base/config.hpp"

static constexpr auto BLOCK_DIM{8};
static constexpr auto BLOCK_WIDTH{WIDTH / BLOCK_DIM};
static constexpr auto BLOCK_HEIGHT{HEIGHT / BLOCK_DIM};

static constexpr dim3 THREADS_PER_BLOCK{BLOCK_DIM, BLOCK_DIM};
static constexpr dim3 NUM_BLOCKS{BLOCK_WIDTH, BLOCK_HEIGHT};
