#!/bin/bash

main="binary_search.c"
output="output.o"

if gcc "$main" -o "$output"; then
    ./"$output"
else
    echo "Compilation failed."
    exit 1
fi

# Clean up the generated binary
rm -f "$output"
# rm -f "$filename"
echo "Compiled files was removed."
