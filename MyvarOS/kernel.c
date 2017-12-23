#include "kernel.h"
#include "tty.h"
#include "kstdio.h"
#include "kstdlib.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"

extern unsigned char KERNEL_START_VADDR;
extern unsigned char KERNEL_END_VADDR;
unsigned int KERNEL_START_ADDR;
extern unsigned char g_ImageRefCall;

void kernel_main(unsigned int boot_ref_addr)
{

  boot_ref_addr = boot_ref_addr - ((unsigned int)&g_ImageRefCall - (unsigned int)&KERNEL_START_VADDR);

  ((unsigned int *)(boot_ref_addr + ((unsigned int)&KERNEL_START_ADDR - (unsigned int)&KERNEL_START_VADDR)))[0] = boot_ref_addr;

  Init_Paging();

  __asm__(
      "lea ECX, [StartInHigherHalf]\n"
      "jmp ECX\n"
      "StartInHigherHalf:\n"
      "call kernel_HigherHalf\n"
      "nop\n");
}

void kernel_HigherHalf()
{
  tty_init();
  tty_clear();
  tty_log("Init tty0");
  tty_log("MyvarOS is booting.");

  tty_log("Installing IDT.");
  Idt_Install();

  tty_log("Installing Isrs.");
  Isrs_Install();

  tty_log("Installing irq.");
  Irq_Install();

  tty_log("Init memory.");
  init_memory_management();

  //init drivers
  //...

  __asm__ __volatile__("sti");

  tty_clear();
  tty_log("MyvarOS has successfully booted");

  
  while (1);
}
