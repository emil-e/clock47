#include "ui.h"

#include <mutex>

#include <esp_log.h>

#include "StackWidget.h"
#include "display.h"

namespace ui {
namespace {

const char *TAG = "ui";

constexpr int FRAMES_PER_SECOND = 20;
constexpr int FRAMES_TICK_INTERVAL = (1000 / FRAMES_PER_SECOND) / portTICK_PERIOD_MS;

std::mutex g_widgetMutex;
StackWidget g_rootWidget;

void eventHandler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                  void *event_data) {
  const auto lock = std::lock_guard(g_widgetMutex);
  g_rootWidget.onEvent(event_base, event_id, event_data);
}

void renderTask(void *params) {
  display::Pane panes[display::NUM_PANES];
  auto nextFrameTime = xTaskGetTickCount();
  for (;;) {
    for (int i = 0; i < display::NUM_PANES; i++) {
      std::fill(begin(panes[i]), end(panes[i]), 0);
    }

    {
      const auto lock = std::lock_guard(g_widgetMutex);
      g_rootWidget.redraw(panes, display::NUM_PANES, nextFrameTime * portTICK_PERIOD_MS);
    }
    display::displayPanes(panes);
    nextFrameTime += FRAMES_TICK_INTERVAL;

    const auto now = xTaskGetTickCount();
    while (now > nextFrameTime) {
      ESP_LOGW(TAG, "Frame skipped, %lu ticks overtime", now - nextFrameTime);
      nextFrameTime += FRAMES_TICK_INTERVAL;
    }

    vTaskDelay(nextFrameTime - now);
  }
}

} // namespace

void init() {
  display::init();

  esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, eventHandler, nullptr,
                                      nullptr);
  xTaskCreate(renderTask, "ui", 1024 * 8, nullptr, 1, nullptr);
}

void push(ui::Widget *widget) {
  const auto lock = std::lock_guard(g_widgetMutex);
  g_rootWidget.push(widget);
}

void remove(ui::Widget *widget) {
  const auto lock = std::lock_guard(g_widgetMutex);
  g_rootWidget.remove(widget);
}

} // namespace ui
