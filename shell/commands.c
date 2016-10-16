#include <main.h>
#include "irq.h"

void Clear(char* args[])
{
    Termianl_Clear();
}

void Dump_Command(char* args[])
{
    register int eax asm("eax");
    register int ebx asm("ebx");
    register int ecx asm("ecx");
    register int edx asm("edx");

    char buf[255];

    itoa(eax, 10, buf);
    puts("Eax: ");
    puts(buf);
    puts("\n");
    
    itoa(ebx, 10, buf);
    puts("Ebx: ");
    puts(buf);
    puts("\n");

    itoa(ecx, 10, buf);
    puts("Ecx: ");
    puts(buf);
    puts("\n");

    itoa(edx, 10, buf);
    puts("Edx: ");
    puts(buf);
    puts("\n");
}

void DevidebyZero(char* args[])
{
    int a = 1;
    int b = 0;

    int c =  a / b;
    putc(c);
}

void Args_Test(char* args[])
{
    int i = 0;
    for(i = 0; i < sizeof(args); i++)
    {
        puts(args[i]);
        puts("\n");
    }
}

void Test_Paging()
{
    putLog("Starting Paging Test");

    char* zero = 0x0;

    int i = 0;
    for(i = 0; i < (1024 * 1024); i++)
    {
        char buf[255];

        char rd = *zero;
        


        itoa((int)zero, 10, buf);
        putLog(buf);

        zero += 4096;
    }

    putLog("Paging passed Sucesfullt");
}


void Test_PageFalt()
{
    int c = *((char*)0x400000);
}

static inline void outb(unsigned short port, unsigned char val)
{
     asm volatile ( "outb %1, %0" : : "a"(val), "Nd"(port) );
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile ( "inb %0, %1"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline void outw(unsigned short port, unsigned short val)
{
     asm volatile ( "outw %1, %0" : : "a"(val), "Nd"(port) );
}

static inline unsigned short inw(unsigned short port)
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

    size = inb(channel + 5) << 8 | inb(channel + 4);
    itoa(size, 10, buf);
    puts("size ");
    puts(buf);
    puts("\n");

    puts("data ");
    for (int x = 0; x < size / 2; ++x) {
        valw = inw(channel);
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

    outb(channel + 6, slave & (1 << 4));    // slave bit
    outb(channel + 1, 0);        // no DMA
    outb(channel + 4, 2048 & 0xff);       // sector count low 
    outb(channel + 5, 2048 >> 8);        // sector count high
    outb(channel + 7, 0xa0);     // packet command

    outw(channel, 0x00a8); // 1 0
    outw(channel, 0x0000); // 3 2
    outw(channel, 0x0000); // 5 4
    outw(channel, 0x0000); // 7 6
    outw(channel, 0x0100); // 9 8
    outw(channel, 0x0000); // 11 10

    while (!atapi_irq_signal);
    atapi_irq_signal = 0;
    
    dump_atapi_data(channel);
    dump_atapi_data(channel);
    //dump_atapi_data(channel);
    //dump_atapi_data(channel);
}

void Test_CDRead() {
    irq_hook(47, &atapi_irq_hook);

    atapi_read(0x170, 0xa0);
}

void Init_Commands()
{

    Shell_Registor_Command("clear", "Clears the screen", Clear);
    Shell_Registor_Command("cls", "Clears the screen", Clear);

    Shell_Registor_Command("debug", "Dumps the registors to the screen", Dump_Command);
    Shell_Registor_Command("dump", "Dumps the registors to the screen", Dump_Command);

    Shell_Registor_Command("dbz", "Devide by zero to test isrs", DevidebyZero);
    
    Shell_Registor_Command("args", "Test args Parsing", Args_Test);

    Shell_Registor_Command("ptest", "Test Paging", Test_Paging);
    Shell_Registor_Command("pfalt", "Cause Page Falt", Test_PageFalt);

    Shell_Registor_Command("cdreadtest", "Test reading CD-ROM", Test_CDRead);
}