BITS 32

GLOBAL _Kernel_Start:function

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
	jne HandleNoMultiboot
	
	mov dword [MultibootInfo_Structure], EBX
	add dword EBX, 0x4
	mov dword EAX, [EBX]
	mov dword [MultibootInfo_Memory_Low], EAX
	add dword EBX, 0x4
	mov dword EAX, [EBX]
	mov dword [MultibootInfo_Memory_High], EAX	
	
	;Switch to protected mode 
	mov dword EAX, CR0
	or EAX, 1
	mov dword CR0, EAX

	;Set stack pointer
	mov dword ESP, Kernel_Stack_Start
	
	
	;Tell CPU about GDT
	mov dword [GDT_Pointer + 2], GDT_Contents
	mov dword EAX, GDT_Pointer
	lgdt [EAX]
	;Set data segments
	mov dword EAX, 0x10
	mov word DS, EAX
	mov word ES, EAX
	mov word FS, EAX
	mov word GS, EAX
	mov word SS, EAX
	; Force reload of code segment
	jmp 8:Boot_FlushCsGDT
Boot_FlushCsGDT:

	extern kernel_main
	call kernel_main	
	
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


xpos db 0
ypos db 0
msg db "MyvarOS Booted", 0 
