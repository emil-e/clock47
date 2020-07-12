#pragma once

#include <mutex>

#include "Widget.h"

class ClockWidget : public ui::Widget {
public:
  ClockWidget();
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override;
  void onEvent(esp_event_base_t base, std::int32_t id, void *data) override;
};
