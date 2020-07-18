#include "ClockWidget.h"

#include <time.h>

#include "draw.h"

ClockWidget::ClockWidget() {}

void ClockWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  time_t now;
  time(&now);

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[64];
  strftime(buffer, sizeof(buffer), "%H:%M", &timeinfo);

  draw::string(panes, n, buffer, ui::color::WHITE);
}
