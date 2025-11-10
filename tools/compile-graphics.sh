#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-only
#
# Helper script to compile graphics programs for Linux/Wasm
# Usage: ./compile-graphics.sh <source.c> [output.wasm]

set -e

LW_ROOT="$(realpath -s "$(dirname "$0")/..")"

# Use environment variables or defaults
: "${LW_WORKSPACE:=$LW_ROOT/workspace}"
LW_WORKSPACE="$(realpath -sm "$LW_WORKSPACE")"
: "${LW_INSTALL:=$LW_WORKSPACE/install}"
LW_INSTALL="$(realpath -sm "$LW_INSTALL")"
: "${LW_DEBUG_CFLAGS:=-g3}"

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <source.c> [output.wasm]"
    echo ""
    echo "Compiles a graphics program for Linux/Wasm."
    echo ""
    echo "Example:"
    echo "  $0 my-graphics-app.c"
    echo "  $0 my-graphics-app.c my-app.wasm"
    echo ""
    echo "The compiled .wasm file can be copied to the initramfs and run in Linux/Wasm."
    echo ""
    echo "Environment variables:"
    echo "  LW_WORKSPACE     - Workspace directory (default: $LW_WORKSPACE)"
    echo "  LW_INSTALL       - Install directory (default: $LW_INSTALL)"
    echo "  LW_DEBUG_CFLAGS  - Debug flags (default: $LW_DEBUG_CFLAGS)"
    exit 1
fi

SOURCE_FILE="$1"
if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: Source file '$SOURCE_FILE' not found"
    exit 1
fi

# Determine output file name
if [ "$#" -ge 2 ]; then
    OUTPUT_FILE="$2"
else
    # Default: replace .c with .wasm
    OUTPUT_FILE="${SOURCE_FILE%.c}.wasm"
fi

# Check if LLVM is built
if [ ! -f "$LW_INSTALL/llvm/bin/clang" ]; then
    echo "Error: LLVM not found. Please build it first:"
    echo "  ./linux-wasm.sh build-llvm"
    exit 1
fi

# Check if musl is built
if [ ! -d "$LW_INSTALL/musl" ]; then
    echo "Error: musl not found. Please build it first:"
    echo "  ./linux-wasm.sh build-musl"
    exit 1
fi

echo "Compiling graphics program..."
echo "  Source: $SOURCE_FILE"
echo "  Output: $OUTPUT_FILE"
echo "  Sysroot: $LW_INSTALL/musl"
echo "  Debug flags: $LW_DEBUG_CFLAGS"
echo ""

# Compile
"$LW_INSTALL/llvm/bin/clang" \
    --target=wasm32-unknown-unknown \
    "--sysroot=$LW_INSTALL/musl" \
    -fPIC -shared \
    $LW_DEBUG_CFLAGS \
    -o "$OUTPUT_FILE" \
    "$SOURCE_FILE"

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
    echo ""
    echo "To include in initramfs:"
    echo "  1. Copy to busybox: cp $OUTPUT_FILE $LW_INSTALL/busybox/bin/"
    echo "  2. Rebuild initramfs: ./linux-wasm.sh build-initramfs"
    echo ""
    echo "Or copy the runtime directory to your web server and include the .wasm file."
else
    echo "✗ Compilation failed"
    exit 1
fi

