#include "mode.h"

#include <algorithm>
#include <mutex>
#include <vector>

#include <esp_log.h>

#include "buttons.h"

namespace mode {
namespace {

struct Mode {
  std::string id;
  ui::Widget *widget;
};

std::mutex g_mutex;
std::vector<Mode> g_modes;
std::size_t g_currentMode = 0;

bool isButton2Down(esp_event_base_t base, std::int32_t id, void *data) {
  return (base == BUTTON_EVENT) && (id == buttons::BUTTON_DOWN) &&
         (*static_cast<std::uint8_t *>(data) == 1);
}

class ModeWidget : public ui::Widget {
public:
  void redraw(display::Pane *panes, int n, std::uint64_t timestamp) override {
    const auto lock = std::lock_guard(g_mutex);
    if (!g_modes.empty()) {
      g_modes[g_currentMode].widget->redraw(panes, n, timestamp);
    }
  }

  void onEvent(esp_event_base_t base, std::int32_t id, void *data) override {
    if (isButton2Down(base, id, data)) {
      next();
    } else {
      const auto lock = std::lock_guard(g_mutex);
      if (!g_modes.empty()) {
        g_modes[g_currentMode].widget->onEvent(base, id, data);
      }
    }
  }
};

ModeWidget g_widget;

} // namespace

void add(const char *id, ui::Widget *widget) {
  const auto lock = std::lock_guard(g_mutex);
  auto &mode = g_modes.emplace_back();
  mode.id = id;
  mode.widget = widget;
}

const char *currentMode() {
  const auto lock = std::lock_guard(g_mutex);
  if (g_modes.empty()) {
    return "";
  } else {
    return g_modes[g_currentMode].id.c_str();
  }
}

void switchTo(const char *id) {
  const auto lock = std::lock_guard(g_mutex);
  const auto it = std::find_if(g_modes.begin(), g_modes.end(),
                               [id](const Mode &mode) { return mode.id == id; });
  if (it != g_modes.end()) {
    g_currentMode = it - g_modes.begin();
  }
}

void prev() {
  const auto lock = std::lock_guard(g_mutex);
  g_currentMode = (g_currentMode == 0) ? (g_modes.size() - 1) : (g_currentMode - 1);
}

void next() {
  const auto lock = std::lock_guard(g_mutex);
  g_currentMode = (g_currentMode + 1) % g_modes.size();
}

ui::Widget *widget() { return &g_widget; }

} // namespace mode
