#include <main.h>

void Clear(char* args)
{
    Termianl_Clear();
}

int test()
{
    
}

void Dump_Command()
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

void Init_Commands()
{
    Shell_Registor_Command("clear", "Clears the screen", Clear);
    Shell_Registor_Command("cls", "Clears the screen", Clear);

    Shell_Registor_Command("debug", "Dumps the registors to the screen", Dump_Command);
    Shell_Registor_Command("dump", "Dumps the registors to the screen", Dump_Command);
}