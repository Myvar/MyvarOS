'''
	The configuration file.
'''


BIN_PATHS = {
	'NASM_BIN': "nasm",
	'GCC_BIN': "gcc",
	'LD_BIN': "ld",
	'ISO_BIN': 'mkisofs',
	'QEMU_I386_BIN': 'qemu-system-i386',
	'CSHARP_COMPILER_BIN': 'mcs',
}

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

SEDNA_CSHARP_COMPILER_OPTS = [
	'${CSHARP_COMPILER_BIN}',
	'-out:${OUTPUT}',
	'${INPUT}',
]

NASM_OPTS = [
	'${NASM_BIN}', 
	'-g', 
	'-f', 'elf', 
	'-o', '${OUTPUT}', 
	'${INPUT}'
]

SEDNA_SOURCES = [
	'sedna/Program.cs',
	'sedna/Parser.cs',
	'sedna/Compiler.cs',
	'sedna/Error.cs',
	'sedna/ByteCode.cs',
	'sedna/Internals/CompilerScope.cs',
	'sedna/Internals/IAst.cs',
	'sedna/Internals/Token.cs',
	'sedna/Internals/Ast/AttributeStmt.cs',
	'sedna/Internals/Ast/DecStmt.cs',
	'sedna/Internals/Ast/ExpressionStmt.cs',
	'sedna/Internals/Ast/FnStmt.cs',
	'sedna/Internals/Ast/ImportStmt.cs',
	'sedna/Internals/Ast/InvokeStmt.cs',
	'sedna/Internals/Ast/LoopStmt.cs',
	'sedna/Internals/Ast/RetStmt.cs',
	'sedna/Internals/Ast/ScopeStmt.cs',
	'sedna/Internals/Ast/TypeStmt.cs',
	'sedna/Internals/Ast/ValueStmt.cs',
	'sedna/Internals/Ast/WhenStmt.cs',
]

KERNEL_SOURCES = [
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