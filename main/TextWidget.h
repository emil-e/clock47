#pragma once

#include <mutex>

#include "Widget.h"

class TextWidget : public ui::Widget {
public:
  static constexpr int MAX_LENGTH = 255;

  explicit TextWidget(const char *text = "");

  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
  void setText(const char *text);

private:
  mutable std::mutex _mutex;
  char _text[MAX_LENGTH + 1];
};
