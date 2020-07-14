#include "effect.h"

#include <random>

#include "draw.h"

namespace effect {

void glow(display::Pane *panes, std::size_t n, std::uint64_t timestamp) {
  std::minstd_rand random;
  for (int p = 0; p < n; p++) {
    auto &pane = panes[p];
    for (int i = 0; i < display::LEDS_PER_PANE; i++) {
      const auto r = random();
      const double phase = std::uint8_t(r) / 255.0;
      const double speedVar = ((std::uint8_t(r >> 8) / 255.0) - 0.5) * 500;
      const double t = (2 * M_PI * timestamp) / (5000 + speedVar);
      const double tp = t + (2 * M_PI * phase);
      const auto x = (std::cos(tp) + 1.0) / 2.0;
      pane[draw::rOffset(i)] *= (std::sqrt(x) / 2.0) + 0.5;
      pane[draw::gOffset(i)] *= x;
      pane[draw::bOffset(i)] *= 0;
    }
  }
}

} // namespace effect
