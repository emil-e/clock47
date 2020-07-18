#pragma once

#include "display.h"

namespace effect {

void init();
void apply(display::Pane *panes, std::size_t n, std::uint64_t timestamp);

} // namespace effect
