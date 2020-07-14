#pragma once

#include "Widget.h"

namespace mode {

void add(const char *id, ui::Widget *widget);
void switchTo(const char *id);
const char *currentMode();
void prev();
void next();
ui::Widget *widget();

} // namespace mode
