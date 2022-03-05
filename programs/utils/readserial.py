#!/usr/bin/env -S python3 -u

import sys, os

fname = sys.argv[1]
with open(fname, "wb", 0) as f:
    size = int.from_bytes(sys.stdin.buffer.read(4), byteorder='little')
    data = sys.stdin.buffer.read(size)
    f.write(data)

