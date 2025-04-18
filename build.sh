#!/bin/bash

# Exit immediately on error
set -e

# Function to clean a specific target directory
clean_target() {
    echo "Cleaning Up $1"
    if [[ -d "$1" ]]; then
        cd "$1" || { echo "Failed to enter directory $1"; exit 1; }
        make clean
        cd ..
    else
        echo "Directory $1 does not exist!"
        exit 1
    fi
}

# Clean the targets if the first argument is "clean"
if [[ $# -eq 1 && $1 == "clean" ]]; then
    echo "Cleanup Option Selected..."
    clean_target "ps4-ksdk"
    clean_target "ps4-payload-sdk"
    clean_target "installer"
    clean_target "debugger"
    clean_target "kdebugger"
    echo "done..."
    exit 0
fi

# Function to build a specific target directory
build_target() {
    echo "=> Building $1"
    if [[ -d "$1" ]]; then
        cd "$1" || exit 1
        make && cd ..
    else
        echo "Directory $1 does not exist!"
        exit 1
    fi
}

# Begin building components
echo "Building PS4Debug Components"
build_target "ps4-ksdk"
build_target "ps4-payload-sdk"
build_target "debugger"
build_target "kdebugger"
build_target "installer"

# Copy the compiled binary to the root folder, renaming it
if [[ -f "./installer/installer.bin" ]]; then
    cp ./installer/installer.bin ./PS4Debug.bin
else
    echo "installer.bin not found!"
    exit 1
fi

echo ""
echo "PS4Debug Successfully Built!"
