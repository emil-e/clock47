#include "TextWidget.h"

#include <cstring>

#include <esp_log.h>

#include "draw.h"

TextWidget::TextWidget(const char *text) { setText(text); }

void TextWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  const auto lock = std::lock_guard(_mutex);

  const auto len = std::strlen(_text);
  if (len <= n) {
    draw::string(panes, n, _text, _color);
    return;
  }

  const std::size_t start = (timestamp / 350) % (len + 1);
  char slice[display::NUM_PANES + 1];
  std::size_t slice_index = 0;
  std::size_t text_index = start;
  while (slice_index < n) {
    const auto c = _text[text_index];
    slice[slice_index++] = (c == '\0') ? ' ' : c;
    text_index = (text_index + 1) % (len + 1);
  }
  slice[slice_index] = '\0';
  draw::string(panes, n, slice, _color);
}

void TextWidget::setText(const char *text) {
  const auto lock = std::lock_guard(_mutex);
  std::strncpy(_text, text, MAX_LENGTH + 1);
  _text[MAX_LENGTH] = '\0';
}

void TextWidget::setColor(const ui::Color &color) {
  const auto lock = std::lock_guard(_mutex);
  _color = color;
}
