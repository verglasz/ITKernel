#!/bin/sh

filename="$1"
size=$(stat --printf='%s' "$filename")

python3 -c "import sys; numb = ($size).to_bytes(4, byteorder='little'); sys.stdout.buffer.write(numb);"

cat "$filename"

exit 1

