#include "StackWidget.h"

#include <algorithm>

void StackWidget::redraw(display::Pane *panes, int n, std::uint64_t timestamp) {
  if (!_stack.empty()) {
    _stack.back()->redraw(panes, n, timestamp);
  }
}

void StackWidget::onEvent(esp_event_base_t base, std::int32_t id, void *data) {
  if (!_stack.empty()) {
    _stack.back()->onEvent(base, id, data);
  }
}

void StackWidget::push(ui::Widget *widget) { _stack.push_back(widget); }

void StackWidget::pop() { _stack.pop_back(); }

void StackWidget::remove(ui::Widget *widget) {
  _stack.erase(
      std::remove_if(_stack.begin(), _stack.end(), [widget](ui::Widget *w) { return w == widget; }),
      _stack.end());
}
