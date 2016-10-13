#include <main.h>

char* InputBuf[255];

void Kb_Handler(char c)
{
    if(c == '\b')        
    {
        strremovelast(InputBuf);
        putc(c);            
    }
    else
    {
        if(c == '\n')
        {
            ExecuteCommand(InputBuf);

            int i = 0;
            for(i = 0; i < 255; i++)
            {
                InputBuf[i] = 0;
            }
        }
        else
        {
            putc(c);
            strappend(InputBuf, c);
        }
    }
}

struct Command CommandTable[1024];
int CommandTableIndex = 0;

void Help(char* args[])
{
    puts("Welcome to MyvarOS\n");
    puts("Here is a list of commands:\n\n");

    int i = 0;
    for(i = 0; i < CommandTableIndex; i++)
    {     
        puts("\t");   
        puts(CommandTable[i].name);
        puts(" - ");
        puts(CommandTable[i].helpmsg);
        puts("\n");
    }
}

void Shell_Init()
{
    kb_install_handler(1, Kb_Handler);
    Shell_Registor_Command("help", "displays all the commands registored in the shell", Help);
    Init_Commands();
    putc('\n');
    putc('>');
}


void Shell_Registor_Command(char name[50], char helpmsg[255], void (*handler)(char* args[]))
{
    struct Command newcmd;
    strcpy(newcmd.name, name);
    strcpy(newcmd.helpmsg, helpmsg);
    newcmd.handler = handler;

    CommandTable[CommandTableIndex] = newcmd;
    CommandTableIndex++;
}

void ExecuteCommand(char* Command)
{
    int found = 0;

    int i = 0;
    for(i = 0; i < CommandTableIndex; i++)
    {       
        if(strcmp(CommandTable[i].name, Command) == 0)
        {
            found = 1;
            putc('\n');

            char* args[0];

            CommandTable[i].handler(args);

            putc('\n');
            putc('>');
        }        
    }

    if(found == 0)
    {
        putc('\n');
        putc('\n');   
        putc('"');     
        puts(Command);
        puts("\"");
        Terminal_SetColor(0xC, 0x0);
        puts(" Does not exist\n");
        Terminal_SetColor(0xF, 0x0);
        putc('>');
    }
}