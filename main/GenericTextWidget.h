#pragma once

#include <mutex>

#include "TextWidget.h"

class GenericTextWidget : public TextWidget {
public:
  explicit GenericTextWidget(const char *text = "");

  void setText(std::string text);

protected:
  std::string provideText() const override;

private:
  mutable std::mutex _mutex;
  std::string _text;
};
