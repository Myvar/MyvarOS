#include <main.h>
#include "irq.h"


static inline void outportw(unsigned short port, unsigned short val)
{
     asm volatile ( "outw %1, %0" : : "a"(val), "Nd"(port) );
}

static inline unsigned short inportw(unsigned short port)
{
    unsigned short ret;
    asm volatile ( "inw %0, %1"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

#define CHANNEL 0x170

void dump_atapi_data(unsigned short channel) {
    char buf[20];
    unsigned short size;
    unsigned short valw;

    size = inportb(channel + 5) << 8 | inportb(channel + 4);
    itoa(size, 10, buf);
    puts("size ");
    puts(buf);
    puts("\n");

    puts("data ");
    for (int x = 0; x < size / 2; ++x) {
        valw = inportw(channel);
        itoa(valw, 10, buf);
        puts(buf);
        puts(" ");        
    }

    puts("\n");
}

static int volatile atapi_irq_signal = 0;

int atapi_irq_hook(struct regs *r) {
    puts("irq hook called\n");
    atapi_irq_signal = 1;
    return 0;
}

void atapi_read(unsigned short channel, unsigned char slave) {
    char buf[20];
    unsigned char val;

    puts("sending atapi command\n");

    outportb(channel + 6, slave & (1 << 4));    // slave bit
    outportb(channel + 1, 0);        // no DMA
    outportb(channel + 4, 2048 & 0xff);       // sector count low 
    outportb(channel + 5, 2048 >> 8);        // sector count high
    outportb(channel + 7, 0xa0);     // packet command

    outportw(channel, 0x00a8); // 1 0
    outportw(channel, 0x0000); // 3 2
    outportw(channel, 0x0000); // 5 4
    outportw(channel, 0x0000); // 7 6
    outportw(channel, 0x0100); // 9 8
    outportw(channel, 0x0000); // 11 10

    while (!atapi_irq_signal);
    atapi_irq_signal = 0;
    
    dump_atapi_data(channel);
    dump_atapi_data(channel);
    //dump_atapi_data(channel);
    //dump_atapi_data(channel);
}