#pragma once

#include "Widget.h"

class EffectWidget : public ui::Widget {
public:
  explicit EffectWidget(ui::Widget &child);
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;

private:
  ui::Widget &_childWidget;
};
