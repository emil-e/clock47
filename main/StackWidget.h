#pragma once

#include <vector>

#include "Widget.h"

class StackWidget : public ui::Widget {
public:
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
  void onEvent(esp_event_base_t base, std::int32_t id, void *data) override;
  void push(ui::Widget *widget);
  void pop();
  void remove(ui::Widget *widget);

private:
  std::vector<ui::Widget *> _stack;
};
