#!/bin/bash

main="main.c"
libs="bst.c"
output="output"

# Compile both main.c and atom.c
if gcc "$main" "$libs" -o "$output"; then
    echo "Compilation successful. Running the program..."
    ./"$output"
else
    echo "Compilation failed."
    exit 1
fi

# Clean up the generated binary
rm -f "$output"
echo "Compiled binary was removed."
