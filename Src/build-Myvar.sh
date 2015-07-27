#!/bin/sh

build=build
compiler=gcc
linker=ld
opt="-w -m32 -Wall -O -fstrength-reduce  -finline-functions -fomit-frame-pointer -nostdinc -fno-builtin -I./kernel/include -c -fno-strict-aliasing -fno-common -fno-stack-protector"

echo "Building..."
echo "==========="

# Build C files

$compiler $opt -o $build/main.o ./kernel/main.c 
$compiler $opt -o $build/console.o ./kernel/console.c 
$compiler $opt -o $build/io.o ./kernel/HAL/io.c 
$compiler $opt -o $build/gdt.o ./kernel/HAL/gdt.c


# Build ASM files
nasm -f elf ./kernel/loader.asm -o $build/loader.o

# Link 
$linker -melf_i386 -T linker.ld $build/*.o

# Put on floppy image
cp Boot/myos.img myos.img
mkdir /media/floppy1
mount myos.img /media/floppy1
cp ./kernel.bin /media/floppy1
sleep 2
umount /media/floppy1
rm -r /media/floppy1

