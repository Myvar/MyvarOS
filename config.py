'''
	The configuration file.
'''


# Default binary.
NASM_BIN = "nasm"
GCC_BIN = "gcc"
LD_BIN = "ld"
ISO_BIN = 'mkisofs'
QEMU_I386_BIN = 'qemu-system-i386'

KERNEL_QEMU_OPTS = [
	'${QEMU_BIN}',
	'-m', '4G',
	'-cdrom', '${INPUT}',
	'-serial', 'tcp:127.0.0.1:4444,server${QEMU_SERIAL_DEBUG_NOWAIT}',
]

KERNEL_ISO_OPTS = [
	'${ISO_BIN}',
	'-o', '${OUTPUT}',
	'-b', 'isolinux-debug.bin',
	'-c', 'boot.cat',
	'-no-emul-boot',
	'-boot-load-size', '4',
	'-boot-info-table',
	'-input-charset', 'default',
	'-quiet',
	'${ISO_DIR}',
]

LD_OPTS = [
	'${LD_BIN}',
	'-m', 'elf_i386',
	'-T', '${BASE_DIR}/inc/linker.ld',
	'-o', '${OUTPUT}',
	'${INPUT}'
]

GCC_OPTS = [
	'${GCC_BIN}',
	'-m32', 
	'-I', './inc',
	'-std=gnu99',
	'-fno-builtin',
	'-ffreestanding',
	'-w',
	'-Wall',
	'-O2',
	'-Wextra',
	'-nostartfiles',
	'-nostdlib',
	'-masm=intel',
	'-c', '${INPUT}',
	'-o', '${OUTPUT}',
]

NASM_OPTS = [
	'${NASM_BIN}', 
	'-g', 
	'-f', 'elf', 
	'-o', '${OUTPUT}', 
	'${INPUT}'
]

sources = [
	'src/kernel.c',
	'src/terminal.c',
	'src/io.c',
	'src/serial.c',
	'src/idt.c',
	'src/isrs.c',
	'src/irq.c',
	'src/drivers/atapi.c',
	'src/drivers/keybord.c',
	'src/shell/shell.c',
	'src/shell/commands.c',
	'src/string.c',
	'src/paging.c',
	'src/heap.c',
	'src/linklist.c',
	{ 'src': 'src/bootstrap.asm', 'ignore_stderr': True },
	'src/idt.asm',
	'src/isrs.asm',
	'src/irq.asm',
]