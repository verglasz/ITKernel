#!/usr/bin/env -S python3 -u

import sys, os, time

chunk = 64

fname = sys.argv[1]
size = os.stat(fname).st_size
numb = (size).to_bytes(4, byteorder='little')
sys.stdout.buffer.write(numb)
sys.stdout.flush()
with open(fname, "rb") as f:
    bs = f.read(chunk)
    while (len(bs) > 0):
        sys.stdout.buffer.write(bs)
        sys.stdout.flush()
        time.sleep(0.3)
        bs = f.read(chunk)

