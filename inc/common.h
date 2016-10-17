#ifndef KERNEL_COMMON_H
#define KERNEL_COMMON_H
void outportb(uint16_t port, uint8_t val);
uint8_t inportb(uint16_t port);
void outportw(uint16_t port, uint16_t val);
uint16_t inportw(uint16_t port);
void *memcpy(void *dest, const void *src, int count);
void *memset(void *dest, char val, int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
void itoa(unsigned i,unsigned base,char* buf);
void itoa_s(int i,unsigned base,char* buf);
#endif