#include "Color.h"

namespace ui {

Color Color::parseHex(const char *str) { return ui::Color(std::strtoull(str, nullptr, 16)); }

std::string Color::hex() const {
  static const char* digits = "0123456789ABCDEF";

  std::string str;
  str.reserve(6);
  str += digits[r >> 4];
  str += digits[r & 0xF];
  str += digits[g >> 4];
  str += digits[g & 0xF];
  str += digits[b >> 4];
  str += digits[b & 0xF];

  return str;
}

} // namespace ui
