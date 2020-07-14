#pragma once

#include <esp_event.h>

#include "display_defines.h"

namespace ui {

class Widget {
public:
  virtual void redraw(display::Pane *panes, int n, std::uint64_t timestamp);
  virtual void onEvent(esp_event_base_t base, std::int32_t id, void *data);

  virtual ~Widget() = default;
};

} // namespace ui
