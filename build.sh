clear

gcc_opts="-m32 -I ./includes -std=gnu99 -fno-builtin -ffreestanding  -w -O2 -Wall -Wextra -nostartfiles -nostdlib"

cp boot.txt ./bin/boot.txt

echo "compiling ..."
nasm -g -f elf -o ./tmp/bootstrap.o bootstrap.asm
nasm -g -f elf -o ./tmp/idt_asm.o idt.asm
nasm -g -f elf -o ./tmp/isrs_asm.o isrs.asm
nasm -g -f elf -o ./tmp/irq_asm.o irq.asm

gcc $gcc_opts -masm=intel -c kernel.c -o ./tmp/kernel.o 
gcc $gcc_opts -masm=intel -c terminal.c -o ./tmp/terminal.o
gcc $gcc_opts -c io.c -o ./tmp/io.o
gcc $gcc_opts -masm=intel -c serial.c -o ./tmp/serial.o
gcc $gcc_opts -masm=intel -c idt.c -o ./tmp/idt.o
gcc $gcc_opts -masm=intel -c isrs.c -o ./tmp/isrs.o
gcc $gcc_opts -masm=intel -c irq.c -o ./tmp/irq.o
gcc $gcc_opts -masm=intel -c ./drivers/keybord.c -o ./tmp/driver_ketybord.o
gcc $gcc_opts -masm=intel -c ./shell/shell.c -o ./tmp/shell.o
gcc $gcc_opts -masm=intel -c ./shell/commands.c -o ./tmp/commands.o
gcc $gcc_opts -masm=intel -c ./string.c -o ./tmp/string.o
gcc $gcc_opts -masm=intel -c ./paging.c -o ./tmp/paging.o
gcc $gcc_opts -masm=intel -c ./heap.c -o ./tmp/heap.o

echo "linking"
ld -m elf_i386 -T linker.ld -o ./bin/krnlld.bin ./tmp/*.o 

echo "building iso"
mkisofs -o myvaros.iso \
   -b isolinux-debug.bin -c boot.cat \
   -no-emul-boot -boot-load-size 4 -boot-info-table \
   bin

qemu-system-i386 -m 4G -cdrom myvaros.iso -serial tcp:127.0.0.1:4444,server,nowait
