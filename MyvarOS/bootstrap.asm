BITS 32

GLOBAL _Kernel_Start:function
GLOBAL MultibootInfo_Structure
GLOBAL g_ImageRefCall

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_TABLE equ (KERNEL_VIRTUAL_BASE >> 22)	; Constant declaring Page Table index in virtual memory
SECTION .text

; BEGIN - Multiboot Header
MultibootSignature dd 464367618
MultibootFlags dd 3
MultibootChecksum dd -464367621
MultibootGraphicsRuntime_VbeModeInfoAddr dd 2147483647
MultibootGraphicsRuntime_VbeControlInfoAddr dd 2147483647
MultibootGraphicsRuntime_VbeMode dd 2147483647
MultibootInfo_Memory_High dd 0
MultibootInfo_Memory_Low dd 0

MultibootInfo_Structure dd 0

Kernel_Stack_End:
	TIMES 65535 db 0
Kernel_Stack_Start:

GDT_Contents:
db 0, 0, 0, 0, 0, 0, 0, 0			; Offset: 0  - Null selector - required 
db 255, 255, 0, 0, 0, 0x9A, 0xCF, 0	; Offset: 8  - KM Code selector - covers the entire 4GiB address range
db 255, 255, 0, 0, 0, 0x92, 0xCF, 0	; Offset: 16 - KM Data selector - covers the entire 4GiB address range
db 255, 255, 0, 0, 0, 0xFA, 0xCF, 0	; Offset: 24 - UM Code selector - covers the entire 4GiB address range
db 255, 255, 0, 0, 0, 0xF2, 0xCF, 0	; Offset: 32 - UM Data selector - covers the entire 4GiB address range

GDT_Pointer db 39, 0, 0, 0, 0, 0

_Kernel_Start:
	cli
	
	; Make sure a MultiBoot boot loader but this beast
	mov dword ECX, 0x2BADB002
	cmp ECX, EAX
	jne (HandleNoMultiboot - KERNEL_VIRTUAL_BASE)

	mov dword [MultibootInfo_Structure - KERNEL_VIRTUAL_BASE], EBX
	add dword EBX, 0x4
	mov dword EAX, [EBX]
	mov dword [MultibootInfo_Memory_Low - KERNEL_VIRTUAL_BASE], EAX
	add dword EBX, 0x4
	mov dword EAX, [EBX]
	mov dword [MultibootInfo_Memory_High - KERNEL_VIRTUAL_BASE], EAX

	;mov dword [MultibootInfo_Structure - KERNEL_VIRTUAL_BASE], 0x0
	
	;Switch to protected mode 
	mov dword EAX, CR0
	or EAX, 1
	mov dword CR0, EAX

	;Set stack pointer
	mov dword ESP, (Kernel_Stack_Start - KERNEL_VIRTUAL_BASE)
	
	;Tell CPU about GDT
	mov dword [GDT_Pointer  - KERNEL_VIRTUAL_BASE + 2], (GDT_Contents - KERNEL_VIRTUAL_BASE)
	mov dword EAX, (GDT_Pointer - KERNEL_VIRTUAL_BASE)
	lgdt [EAX]

	;Set data segments
	mov dword EAX, 0x10
	mov word DS, EAX
	mov word ES, EAX
	mov word FS, EAX
	mov word GS, EAX
	mov word SS, EAX
	; Force reload of code segment
	jmp 8:(Boot_FlushCsGDT - KERNEL_VIRTUAL_BASE)
Boot_FlushCsGDT:	

	; This will the actual address of image ref onto the stack. A relative
	; call instruction should be emitted.
	call g_ImageRefCall
g_ImageRefCall:

	extern kernel_main
	lea EAX, [kernel_main - KERNEL_VIRTUAL_BASE]
	call EAX

	jmp Halt

HandleNoMultiboot:
	; BEGIN - Set Screen Colour
	mov dword EAX, 0x4F		
	mov dword EBX, 0xB8000 	
	mov dword ECX, 64000		
	.ColourOutput:
	mov byte [EBX], 0
	mov byte [EBX+1], AL
	add EBX, 2
	loop .ColourOutput
	
Halt:
	cli			
	hlt			
	jmp Halt


SECTION .bss

GLOBAL Page_Table1:data
GLOBAL Page_Directory:data

align 4096
Page_Table1: resb (1024 * 4 * 1024)	; Reserve uninitialised space for Page Table -  # of entries/page table * 4 bytes/entry * total # of page tables 
											; actual size = 4194304 bytes = 4MiB, represents 4GiB in physical memory
											; ie. each 4 byte entry represent 4 KiB in physical memory
Page_Directory: resb (1024 * 4 * 1) ; Reserve uninitialised space for Page Directory - # of pages tables * 4 bytes/entry * # of directory (4096 = 4 KiB)