#!/usr/bin/env python3

import math

for x in range(0, 256):
    print(f"{math.sin((2 * math.pi * x) / 256)},")
