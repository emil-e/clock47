#pragma once

#include "Widget.h"

class EffectWidget : public display::Widget {
public:
  explicit EffectWidget(display::Widget &child);
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;

private:
  display::Widget &_childWidget;
};
