#!/usr/bin/env -S python3 -u

import sys, os, time

chunk = 64 # bytes to transfer at at time
chunk_delay = 0.03 # delay between chunk transfers, in seconds

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
        time.sleep(chunk_delay)
        bs = f.read(chunk)

