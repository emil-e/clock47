#include "ClockWidget.h"

#include <time.h>

#include "draw.h"
#include "buttons.h"

ClockWidget::ClockWidget() {}

void ClockWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  time_t now;
  struct tm timeinfo;
  char strftime_buf[64];
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%H%M", &timeinfo);
  draw::string(panes, n, strftime_buf, draw::WHITE);
}

void ClockWidget::onEvent(esp_event_base_t base, std::int32_t id, void *data) {}
