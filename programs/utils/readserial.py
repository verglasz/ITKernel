#!/usr/bin/env -S python3 -u

import sys, os

fname = sys.argv[1]
with open(fname, "wb", 0) as f:
    try:
        data = sys.stdin.buffer.read(100)
        while len(data) > 0:
            f.write(data)
            data = sys.stdin.buffer.read(100)
    except KeyboardInterrupt:
        f.write(data)

