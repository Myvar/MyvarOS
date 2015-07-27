#include <console.h>
#include <hal.h>
#include <system.h>

void kmain (void* MultibootStructure)
{
	Console_Clear();
	Print_Info("Starting boot sequence");
	Gdt_Install();
	Print_Info("GDT Installed Successfully");
	Idt_Install();
	Print_Info("IDT Installed Successfully");
}

void Print_Info(string txt)
{
	Console_Write_String("[");
	Console_Write_String_Colored("Info", ColorGreen);
	Console_Write_String("]");
	Console_Write_String(txt);
	Console_Write_String("\n");
}

void Print_Error(string txt)
{
	Console_Write_String("[");
	Console_Write_String_Colored("Error", ColorRed);
	Console_Write_String("]");
	Console_Write_String(txt);
	Console_Write_String("\n");
}
