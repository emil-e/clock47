#pragma once

#include "Widget.h"

class DateWidget : public ui::Widget {
public:
  DateWidget();
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
};
