#pragma once

#include <mutex>

#include "Widget.h"
#include "Color.h"

class TextWidget : public ui::Widget {
public:
  static constexpr int MAX_LENGTH = 255;

  explicit TextWidget(const char *text = "");

  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
  void setText(const char *text);
  void setColor(const ui::Color &color);

private:
  mutable std::mutex _mutex;
  char _text[MAX_LENGTH + 1];
  ui::Color _color = ui::color::WHITE;
};
