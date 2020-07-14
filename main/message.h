#pragma once

#include <cstdint>

#include "Color.h"

namespace message {

constexpr std::size_t MAX_LENGTH = 255;

void display(const char *text, const ui::Color &color);
void clear();

} // namespace message
