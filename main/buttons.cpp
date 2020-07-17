#include "buttons.h"

#include <mutex>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_log.h>

ESP_EVENT_DEFINE_BASE(BUTTON_EVENT);

namespace buttons {
namespace {

const char *TAG = "buttons";

constexpr int DEBOUNCE_MS = 10;
constexpr int DEBOUNCE_TICKS = DEBOUNCE_MS / portTICK_PERIOD_MS;

class Button {
public:
  enum class UpdateStatus { NO_CHANGE = -1, UP = 0, DOWN = 1 };

  Button(gpio_num_t pin) : _pin(pin) {}

  void init() {
    gpio_config_t ioConf;
    ioConf.pin_bit_mask = 1 << _pin;
    ioConf.intr_type = GPIO_INTR_DISABLE;
    ioConf.mode = GPIO_MODE_INPUT;
    ioConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&ioConf));
  }

  UpdateStatus update() {
    const auto now = xTaskGetTickCount();
    if (now < (_lastTime + DEBOUNCE_TICKS)) {
      return UpdateStatus::NO_CHANGE;
    }
    _lastTime = now;

    const auto newState = !bool(gpio_get_level(_pin));
    {
      const auto lock = std::lock_guard(_mutex);
      const auto oldState = _state;
      _state = newState;
      if (oldState != _state) {
        return _state ? UpdateStatus::DOWN : UpdateStatus::UP;
      } else {
        return UpdateStatus::NO_CHANGE;
      }
    }
  }

  bool state() const {
    const auto lock = std::lock_guard(_mutex);
    return _state;
  }

private:
  mutable std::mutex _mutex;
  gpio_num_t _pin;
  TickType_t _lastTime = 0;
  bool _state = false;
};

Button g_buttons[] = {Button(GPIO_NUM_27), Button(GPIO_NUM_2)};
constexpr int NUM_BUTTONS = sizeof(g_buttons) / sizeof(Button);

void onButtonChanged(int buttonIndex, bool state) {
  ESP_LOGI(TAG, "Button %d changed state to %d", buttonIndex, int(state));
  std::uint8_t idx = buttonIndex;
  ESP_ERROR_CHECK(esp_event_post(BUTTON_EVENT, state ? BUTTON_DOWN : BUTTON_UP, &idx, 1,
                                 10 / portTICK_PERIOD_MS));
}

void buttonTask(void *params) {
  ESP_LOGI(TAG, "Buttons initialized");
  for (;;) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
      const auto status = g_buttons[i].update();
      if (status != Button::UpdateStatus::NO_CHANGE) {
        onButtonChanged(i, bool(int(status)));
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

} // namespace

void init() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    g_buttons[i].init();
  }

  // TODO: task priority, what should it be?
  xTaskCreate(buttonTask, "buttons", 2048, nullptr, 1, nullptr);
}

bool state(std::size_t index) {
  if (index >= NUM_BUTTONS) {
    return false;
  }

  return g_buttons[index].state();
}

} // namespace buttons
