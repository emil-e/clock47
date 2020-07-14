#include "DateWidget.h"

#include <time.h>

#include "draw.h"
#include "effect.h"

DateWidget::DateWidget() {}

void DateWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  time_t now;
  time(&now);

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[64];
  strftime(buffer, sizeof(buffer), "%d%b", &timeinfo);

  draw::string(panes, n, buffer, ui::color::WHITE);
  effect::glow(panes, n, timestamp);
}
