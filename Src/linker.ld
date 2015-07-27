ENTRY (loader)
OUTPUT ("kernel.bin")

addr = 0x100000;

SECTIONS
{
      .text addr :
      ALIGN(0x1000)
      {
            *(.text*);
            *(.rodata*);
      }
      
      .data :
      ALIGN(0x1000)
      {
            *(.data*);
      }
      
      .bss :
      ALIGN(0x1000)
      {
            *(.bss*);
      }

}

