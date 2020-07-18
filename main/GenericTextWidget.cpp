#include "GenericTextWidget.h"

#include <cstring>

GenericTextWidget::GenericTextWidget(const char *text) { setText(text); }

void GenericTextWidget::setText(std::string text) {
  const auto lock = std::lock_guard(_mutex);
  _text = std::move(text);
}

std::string GenericTextWidget::provideText() const {
  const auto lock = std::lock_guard(_mutex);
  return _text;
}
