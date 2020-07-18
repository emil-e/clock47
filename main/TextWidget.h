#pragma once

#include <mutex>

#include "Widget.h"
#include "Color.h"

class TextWidget : public ui::Widget {
public:
  static constexpr int MAX_LENGTH = 255;

  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
  void setColor(const ui::Color &color);

protected:
  virtual std::string provideText() const = 0;

private:
  mutable std::mutex _mutex;
  ui::Color _color = ui::color::WHITE;
};
