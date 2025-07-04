#pragma once

#include <array>
#include <cstdint>

namespace display {

constexpr int NUM_PANES = 5;
constexpr int LEDS_PER_PANE = 47;
constexpr std::size_t PANE_BUFFER_SIZE = 190;

using Pane = std::array<std::uint8_t, PANE_BUFFER_SIZE>;

} // namespace display
