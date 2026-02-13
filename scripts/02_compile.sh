#!/bin/bash
# Compile CIS server and client

echo "Compiling CIS programs..."

# Detect compiler
if command -v gcc &> /dev/null; then
    CC=gcc
elif command -v clang &> /dev/null; then
    CC=clang
else
    echo "✗ No C compiler found (need gcc or clang)"
    exit 1
fi

echo "Using compiler: $CC"

$CC -o server server.c -lutil
if [ $? -ne 0 ]; then
    echo "✗ Server compilation failed!"
    exit 1
fi

$CC -o client client.c
if [ $? -ne 0 ]; then
    echo "✗ Client compilation failed!"
    exit 1
fi

echo "Compilation successful"
ls -lh server client