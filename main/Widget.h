#pragma once

#include <esp_event.h>

#include "display_types.h"

namespace display {

class Widget {
public:
  virtual void redraw(Pane *panes, int n, std::uint64_t timestamp) = 0;
  virtual void onEvent(esp_event_base_t base, std::int32_t id, void *data);

  virtual ~Widget() = default;
};

} // namespace display
