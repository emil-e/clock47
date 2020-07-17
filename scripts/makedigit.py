#!/usr/bin/env python3

import sys

y = 0
for x in sys.argv[1:]:
    y = y | (1 << (int(x) - 1))

print("0x{0:X}".format(y))
