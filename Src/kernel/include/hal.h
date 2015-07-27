#ifndef __HAL_H__
#define __HAL_H__

#include <types.h>

extern uint8 inportb (uint16 _port);
extern void outportb (uint16 _port, uint8 _data);

extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void Gdt_Install();
#endif
