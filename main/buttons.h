#pragma once

#include <esp_event.h>

ESP_EVENT_DECLARE_BASE(BUTTON_EVENT);

namespace buttons {

enum { BUTTON_UP, BUTTON_DOWN };

void init();
bool state(std::size_t index);

}  // namespace buttons
