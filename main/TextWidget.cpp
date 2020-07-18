#include "TextWidget.h"

#include <cstring>

#include <esp_log.h>

#include "draw.h"

void TextWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  const auto lock = std::lock_guard(_mutex);

  const auto text = provideText();
  if (text.size() <= n) {
    draw::string(panes, n, text.c_str(), _color);
    return;
  }

  const std::size_t start = (timestamp / 350) % (text.size() + 1);
  char slice[display::NUM_PANES + 1];
  std::size_t slice_index = 0;
  std::size_t text_index = start;
  while (slice_index < n) {
    const auto c = text.c_str()[text_index];
    slice[slice_index++] = (c == '\0') ? ' ' : c;
    text_index = (text_index + 1) % (text.size() + 1);
  }
  slice[slice_index] = '\0';
  draw::string(panes, n, slice, _color);
}

void TextWidget::setColor(const ui::Color &color) {
  const auto lock = std::lock_guard(_mutex);
  _color = color;
}
