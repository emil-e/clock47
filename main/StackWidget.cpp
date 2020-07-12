#include "StackWidget.h"

#include <algorithm>

void StackWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  if (_top != 0) {
    _stack[_top - 1]->redraw(panes, n, timestamp);
  }
}

void StackWidget::onEvent(esp_event_base_t base, std::int32_t id, void *data) {
  if (_top != 0) {
    _stack[_top - 1]->onEvent(base, id, data);
  }
}

void StackWidget::push(ui::Widget *widget) {
  if (_top == STACK_SIZE) {
    return;
  }

  _stack[_top++] = widget;
}

void StackWidget::pop() { _top--; }

void StackWidget::remove(ui::Widget *widget) {
  const auto end =
      std::remove_if(_stack, _stack + _top, [widget](ui::Widget *w) { return w == widget; });
  _top = end - _stack;
}
