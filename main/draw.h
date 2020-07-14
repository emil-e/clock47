#pragma once

#include "Color.h"
#include "display_defines.h"

namespace draw {

constexpr std::size_t bOffset(int led) {
  const int col = led % 12;
  const std::uint8_t colAddr = (col <= 5) ? col : (col + 2);
  const int row = (led / 12) * 3 + 0;
  return (row << 4) + colAddr;
}

constexpr std::size_t gOffset(int led) { return bOffset(led) + 0x10; }

constexpr std::size_t rOffset(int led) { return bOffset(led) + 0x20; }

void character(display::Pane &pane, char c, const ui::Color &color);
void string(display::Pane *panes, int numPanes, const char *str, const ui::Color &color);

} // namespace draw
