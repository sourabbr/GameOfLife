#!/bin/bash

# Define the source files
SOURCE_FILES="src/*/*.c"

# Binary directory
BIN_DIR="bin"
# Binary name
OUTPUT_BINARY="$BIN_DIR/game_of_life"

# Compile the program
compile() {
    mkdir -p $BIN_DIR
    # gcc -o $OUTPUT_BINARY $SOURCE_FILES
    mpicc -o $OUTPUT_BINARY $SOURCE_FILES -lm
    if [ $? -eq 0 ]; then
        echo "Compilation successful. Binary created: $OUTPUT_BINARY"
    else
        echo "Compilation failed."
        exit 1
    fi
}

# Clean up binaries
clean() {
    if [ -d $BIN_DIR ]; then
        rm -rf $BIN_DIR
        echo "Cleanup successful. Removed: $OUTPUT_BINARY"
    else
        echo "Nothing to clean. The binary does not exist."
    fi
}

case "$1" in
    compile)
        compile
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {compile|clean}"
        exit 1
        ;;
esac