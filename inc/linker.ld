ENTRY(_Kernel_Start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
	. = 0xC0100000;

	KERNEL_START_VADDR = .;

	.text : AT(ADDR(.text) - 0xC0000000) {
		*(.text);
	}
		
	. = ALIGN(0x1000);
	.bss : AT(ADDR(.bss) - 0xC0000000) {
		*(.bss);
	}

	.data : AT(ADDR(.data) - 0xC0000000) {
		*(.data);
	}

	.rodata : AT(ADDR(.rodata) - 0xC0000000) {
		*(.rodata);
	}

	SednaStart = .;

	.sedna : AT(ADDR(.sedna) - 0xC0000000) {
		*(.sedna);
	}

	KERNEL_END_VADDR = .;
}
