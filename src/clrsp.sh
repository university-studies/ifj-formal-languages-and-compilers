#!/usr/bin/env bash

for file in *.c *.h; do
    (rm -f "$file"; sed 's/[[:space:]]*$//' > "$file") < "$file"
done
