#!/bin/bash

# Configuration
SOURCE_FILES="src/*/*.c"
BIN_DIR="bin"
OUTPUT_BINARY="$BIN_DIR/conways_game"

# Compile the program
compile() {
    mkdir -p $BIN_DIR
    mpicc -o $OUTPUT_BINARY $SOURCE_FILES -lm
    if [ $? -eq 0 ]; then
        echo "Compiled successfully: $OUTPUT_BINARY"
    else
        echo "Compilation failed."
        exit 1
    fi
}

# Clean up binaries
clean() {
    rm -rf $BIN_DIR
    rm -rf "conways_game.out"
    echo "Cleanup complete."
}

# Command selection
if [ "$1" == "compile" ]; then
    compile
elif [ "$1" == "clean" ]; then
    clean
else
    echo "Usage: $0 {compile|clean}"
    exit 1
fi
