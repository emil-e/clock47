#include "message.h"

#include "TextWidget.h"
#include "ui.h"

namespace message {
namespace {

TextWidget g_widget;

} // namespace

void display(const char *text, const ui::Color &color) {
  clear();
  g_widget.setText(text);
  g_widget.setColor(color);
  ui::push(&g_widget);
}

void clear() {
  ui::remove(&g_widget);
}

} // namespace message
