#!/bin/bash

# Variables
LIBPS4="../ps4-payload-sdk"
TEXT="0x926200000"
DATA="0x926300000"
CC="gcc"
AS="gcc"
OBJCOPY="objcopy"
ODIR="build"
SDIR="source"
IDIRS="-I$LIBPS4/include -I. -Iinclude"
LDIRS="-L$LIBPS4 -L. -Llib"
CFLAGS="$IDIRS -O2 -std=c11 -fno-builtin -nostartfiles -nostdlib -Wall -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=large -DTEXT_ADDRESS=$TEXT -DDATA_ADDRESS=$DATA"
SFLAGS="-nostartfiles -nostdlib -march=btver2 -mtune=btver2"
LFLAGS="$LDIRS -Xlinker -T $LIBPS4/linker.x -Wl,--build-id=none -Ttext=$TEXT -Tdata=$DATA"
LIBS="-lPS4"
TARGET="./debugger.bin"

# Recursive find for source and header files
CFILES=$(find $SDIR -type f -name "*.c")
SFILES=$(find $SDIR -type f -name "*.s")
HFILES=$(find include -type f -name "*.h")

# Create the build directory if it doesn't exist
mkdir -p "$ODIR"

# Function to compile C files
compile_c() {
    for src_file in $CFILES; do
        # Extract the base name (file name without path)
        base_name=$(basename "$src_file" .c)
        
        # Compile the file to the build directory (root of build)
        echo "Compiling: $src_file"
        $CC -c -o "$ODIR/$base_name.o" "$src_file" $CFLAGS
    done
}

# Function to compile Assembly files
compile_s() {
    for src_file in $SFILES; do
        # Extract the base name (file name without path)
        base_name=$(basename "$src_file" .s)
        
        # Compile the file to the build directory (root of build)
        echo "Compiling Assembly: $src_file"
        $AS -c -o "$ODIR/$base_name.o" "$src_file" $SFLAGS
    done
}

# Function to link the compiled object files
link_objects() {
    echo "Linking object files to create $TARGET"
    $CC $LIBPS4/crt0.s "$ODIR"/*.o -o temp.t $CFLAGS $LFLAGS $LIBS
    echo "Converting to binary"
    $OBJCOPY -O binary temp.t "$TARGET"
    echo "Cleaning up temporary files"
    rm -f temp.t
}

# Clean the build directory
clean() {
    echo "Cleaning..."
    rm -f "$TARGET"
    rm -rf "$ODIR"/*
}

# Main script execution
case $1 in
    clean)
        clean
        ;;
    build)
        # Compile C and Assembly files
        compile_c
        compile_s
        
        # Link the object files
        link_objects
        ;;
    *)
        echo "Usage: $0 {clean|build}"
        exit 1
        ;;
esac
