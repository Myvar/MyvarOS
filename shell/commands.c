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



void Test_CDRead() {
    irq_hook(47, &atapi_irq_hook);

    atapi_read(0x170, 0xa0);
}


void Test_Heap()
{
    void* mem = kmalloc(8);
    putLog("Allocating 8 bytes of memmory");

    char buf[255];

    itoa(mem, 16, buf);
    puts("Addres: 0x");
    puts(buf);
    puts("\n");
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

    Shell_Registor_Command("theap", "Test Heap", Test_Heap);    

    Shell_Registor_Command("cdreadtest", "Test reading CD-ROM", Test_CDRead);
}