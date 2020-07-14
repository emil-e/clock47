#pragma once

#include "Widget.h"

class ClockWidget : public ui::Widget {
public:
  ClockWidget();
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
};
