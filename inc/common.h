#ifndef KERNEL_COMMON_H
#define KERNEL_COMMON_H
void outportb(unsigned short port, unsigned char val);
unsigned char inportb(unsigned short port);
void outportw(unsigned short port, unsigned short val);
unsigned short inportw(unsigned short port);
void *memcpy(void *dest, const void *src, int count);
void *memset(void *dest, char val, int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
void itoa(unsigned i,unsigned base,char* buf);
void itoa_s(int i,unsigned base,char* buf);
#endif