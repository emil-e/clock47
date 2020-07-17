#include "effect.h"

#include <cmath>
#include <mutex>
#include <random>

#include "draw.h"
#include "fastmath.h"
#include "property.h"
#include "ui.h"

namespace effect {
namespace {

std::mutex g_mutex;

class ColorProperty : public property::Property {
public:
  explicit ColorProperty(const ui::Color &color) : _color(color) {}

  void set(const std::string &value) override {
    const auto lock = std::lock_guard(g_mutex);
    _color = ui::Color::parseHex(value.c_str());
  }

  std::string get() const override {
    const auto lock = std::lock_guard(g_mutex);
    return _color.hex();
  }

  const ui::Color &operator*() const { return _color; }
  const ui::Color *operator->() const { return &_color; }

private:
  ui::Color _color;
};

ColorProperty g_color0(ui::color::WHITE);
ColorProperty g_color1(ui::color::BLUE);

template <typename T>
constexpr T lerp(T a, T b, T t) {
  if (t < 0.0) {
    t = 0;
  }
  if (t > 1.0) {
    t = 1;
  }

  return a * (1.0 - t) + b * t;
}

} // namespace

void init() {
  property::add("color0", &g_color0);
  property::add("color1", &g_color1);
}

void glow(display::Pane *panes, std::size_t n, std::uint64_t timestamp) {
  const auto lock = std::lock_guard(g_mutex);
  std::minstd_rand random;
  for (int p = 0; p < n; p++) {
    auto &pane = panes[p];
    for (int i = 0; i < display::LEDS_PER_PANE; i++) {
      const auto r = random();
      const double phase = std::uint8_t(r) / 255.0;
      const double speedVar = ((std::uint8_t(r >> 8) / 255.0) - 0.5) * 500;
      const double t = (2 * M_PI * timestamp) / (5000 + speedVar);
      const double tp = t + (2 * M_PI * phase);
      const auto x = (fastmath::sin(tp) + 1.0) / 2.0;
      pane[draw::rOffset(i)] *= lerp<float>(g_color0->rf(), g_color1->rf(), x);
      pane[draw::gOffset(i)] *= lerp<float>(g_color0->gf(), g_color1->gf(), x);
      pane[draw::bOffset(i)] *= lerp<float>(g_color0->bf(), g_color1->bf(), x);
    }
  }
}

} // namespace effect
