#!/bin/bash

# Compile the assembly code to produce an executable
clang --target=mips64el-linux-gnu -nostdlib -static -fuse-ld=lld -o test test.s

# Run the executable using QEMU
qemu-mips64el test

# Print the exit status of the last command (qemu-mips64el)
echo $?

