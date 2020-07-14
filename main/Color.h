#pragma once

#include <cstdint>

namespace ui {

struct Color {
  constexpr Color() = default;
  constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : r(r), g(g), b(b) {}
  constexpr Color(std::uint32_t rgb) : r(rgb >> 16), g(rgb >> 8), b(rgb) {}

  std::uint8_t r = 0;;
  std::uint8_t g = 0;
  std::uint8_t b = 0;
};

namespace color {

constexpr Color WHITE = Color(255, 255, 255);
constexpr Color BLACK = Color(0, 0, 0);

constexpr Color RED = Color(255, 0, 0);
constexpr Color GREEN = Color(0, 255, 0);
constexpr Color BLUE = Color(0, 0, 255);

constexpr Color YELLOW = Color(255, 255, 0);
constexpr Color CYAN = Color(0, 255, 255);
constexpr Color MAGENTA = Color(255, 0, 255);

} // namespace color

} // namespace ui
