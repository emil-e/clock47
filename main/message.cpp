#include "message.h"

#include <cstring>

#include "TextWidget.h"
#include "property.h"
#include "ui.h"
#include "mode.h"

namespace message {
namespace {

std::mutex g_mutex;
std::string g_message = "HELLO";

class MessageWidget : public TextWidget {
public:
  std::string provideText() const override {
    const auto lock = std::lock_guard(g_mutex);
    return g_message;
  }
};

class MessageProperty : public property::Property {
public:
  void set(const std::string &value) override {
    const auto lock = std::lock_guard(g_mutex);
    g_message = value;
  }

  std::string get() const override {
    const auto lock = std::lock_guard(g_mutex);
    return g_message;
  }
};

MessageWidget g_widget;
MessageProperty g_property;

} // namespace

void init() {
  mode::add("message", &g_widget);
  property::add("message", &g_property);
}

} // namespace message
