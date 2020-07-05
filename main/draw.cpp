#include "draw.h"

namespace draw {
namespace {

std::uint64_t charToFrameBits(char c) {
  switch (c) {
  case '0':
    return 0x3FEE07E077BE;
  case '1':
    return 0x1F81F81F81F8;
  case '2':
    return 0x17E2BEFE8EB7;
  case '3':
    return 0x17E0BE17F6BE;
  case '4':
    return 0x7077BF16F007;
  case '5':
    return 0x775EB457F6BE;
  case '6':
    return 0x3F4EB4FFF7BE;
  case '7':
    return 0x376007007007;
  case '8':
    return 0x3FE7BEFFF7BE;
  case '9':
    return 0x3FE7BF16F6BE;
  case 'a':
  case 'A':
    return 0x3FEEB7F6FE07;
  case 'b':
  case 'B':
    return 0x77EEBEF7FEBE;
  case 'c':
  case 'C':
    return 0x3FEE00E057BE;
  case 'd':
  case 'D':
    return 0x77EE07E07EBE;
  case 'e':
  case 'E':
    return 0x775EB7F6AEB7;
  case 'f':
  case 'F':
    return 0x775EB7F6AE00;
  case 'g':
  case 'G':
    return 0x3F4E07E2F7BE;
  case 'h':
  case 'H':
    return 0x707EB7F6FE07;
  case 'i':
  case 'I':
    return 0xF81F81F81F8;
  case 'j':
  case 'J':
    return 0x7007A077BE;
  case 'k':
  case 'K':
    return 0x707EBEF7FE07;
  case 'l':
  case 'L':
    return 0x700E00E00EB7;
  case 'm':
  case 'M':
    return 0x7DFFFFE07E07;
  case 'n':
  case 'N':
    return 0x7FEE07E07E07;
  case 'o':
  case 'O':
    return 0x3FEE07E077BE;
  case 'p':
  case 'P':
    return 0x77EEBEF68E00;
  case 'q':
  case 'Q':
    return 0x3FEE07E077BF;
  case 'r':
  case 'R':
    return 0x77EEBEF7FE07;
  case 's':
  case 'S':
    return 0x3F47B417F6BE;
  case 't':
  case 'T':
    return 0x5FD1F81F81F8;
  case 'u':
  case 'U':
    return 0x707E07E077BE;
  case 'v':
  case 'V':
    return 0x707E077FE3FC;
  case 'w':
  case 'W':
    return 0x707E276F63DC;
  case 'x':
  case 'X':
    return 0x7077BEFFFE07;
  case 'y':
  case 'Y':
    return 0x7077BE1F81F8;
  case 'z':
  case 'Z':
    return 0x5772BEFE8EB7;
  // case 'å':
  // case 'Å':
  //   return 0x2F4FFF7BF;
  // case 'ä':
  // case 'Ä':
  //   return 0x2022B4FFF7BF;
  // case 'ö':
  // case 'Ö':
  //   return 0x2022B4FFF7BE;
  case '.':
    return 0x40;
  case ',':
    return 0x34;
  case ':':
    return 0x40000040;
  case ';':
    return 0x40000034;
  case '-':
    return 0xEB756A000;
  case '+':
    return 0x88FFF5FA148;
  case '"':
    return 0x707000000000;
  case '\'':
    return 0xF8148000000;
  case '!':
    return 0xF81F81680B0;
  case '?':
    return 0x17E0BE1F80F0;
  case '(':
    return 0x3F0E00E007B0;
  case '[':
    return 0x770E00E00EB0;
  case ')':
    return 0x7E0070070BE;
  case ']':
    return 0x770070070B7;
  case '=':
    return 0x28A148A95148;
  case '#':
    return 0xFFFFFFFFFFFFFFFF;
  case ' ':
  default:
    return 0x0;
  }
}

} // namespace



void character(display::Pane &pane, char c, const Color &color) {
  auto bits = charToFrameBits(c);
  for (int i = 0; i < display::LEDS_PER_PANE; i++) {
    if ((bits & 0x1) != 0) {
      pane[rOffset(i)] = color.r;
      pane[gOffset(i)] = color.g;
      pane[bOffset(i)] = color.b;
    }
    bits >>= 1;
  }
}

void string(display::Pane *panes, int numPanes, const char *str, const Color &color) {
  bool ended = false;
  for (int i = 0; i < numPanes; i++) {
    if (str[i] == '\0') {
      ended = true;
    }

    if (!ended) {
      character(panes[i], str[i], color);
    } else {
      std::fill(begin(panes[i]), end(panes[i]), 0);
    }
  }
}

} // namespace draw
