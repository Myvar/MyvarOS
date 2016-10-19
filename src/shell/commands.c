#include <stdint.h>
#include "main.h"
#include "irq.h"
#include "sedna.h"


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

void PutString(char *s, int lenght )
{    
    int i = 0;
    for(i = 0; i < lenght; i++)
    {
        putc(s[i]);       
    }

}



/*
    Copies a string value specified by offset and length into a newly allocated
    string sequence terminated by a null byte.
*/


extern uint8_t KERNEL_START_VADDR;

unsigned int            SednaStart;

void Test_Sedna() {
    SEDNAMODULE *mod;
    int32_t x;
    int32_t y;

    /*
        Use specialized function for loading a Sedna module. The
        `mod` variable is a pointer to an allocated object in the
        heap. This allows anything located at the address to be
        removed afterwards if fully loaded into the SEDNAMODULE
        object above.
    */
    mod = sedna_load_module((uintptr_t)&SednaStart);

  

    kprintf("mod-scope-name: %s\n", mod->scope);
    kprintf(" import-count: %x type-count: %x method-count: %x\n",
        mod->import_cnt,
        mod->type_cnt,
        mod->method_cnt
    );

    for (x = 0; x < mod->import_cnt; ++x) {
        kprintf(" import[%x]: %s\n", x, mod->import[x]);
    }

    for (x = 0; x < mod->type_cnt; ++x) {
        kprintf(" type[%x]: %s base: %s\n", 
            x,
            mod->type[x], mod->type_base[x]
        );
    }

    for (x = 0; x < mod->method_cnt; ++x) {
        kprintf(" method[%x]: %s param-count: %x op-count: %x bytecode-sz: %x\n",
            x,
            mod->method[x].name,
            mod->method[x].param_cnt,
            mod->method[x].op_cnt,
            mod->method[x].bytecode_sz
        );

        for (y = 0; y < mod->method[x].param_cnt; ++y) {
            kprintf("  param-type[%x]: %s\n", y, mod->method[x].param_type[y]);
        }
    }

    kprintf("end of module\n");

    kfree(mod);
}

void Test_Panic() {
    panic("Panic Test");
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

    Shell_Registor_Command("tsedna", "Sedna testing command", Test_Sedna);

    Shell_Registor_Command("panic", "Panic the kernel", Test_Panic);
    
}