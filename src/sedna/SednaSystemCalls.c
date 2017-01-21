#include <stdint.h>
#include "main.h"
#include "irq.h"
#include "sedna.h"

void sedna_system_call(char *modulename, char *func, Stack *args)
{
    if (strcmp("stdio::puts", func) == 0)
    {
        char *arg = (char *)PopStack(args);
        puts(arg);
        kfree(arg);
    }
    else if (strcmp("stdio::itos", func) == 0)
    {

        int *arg = (int *)PopStack(args);
        char buf[255];
        
        itoa(arg, 10, buf);
        PushStack(args, buf, 0);//push string onto stack

        kfree(arg);
    }
    else if (strcmp("stdio::putc", func) == 0)
    {
        char arg = (char)PopStack(args);
        putc(arg);
        kfree(arg);
    }
    else if (strcmp("display::setx", func) == 0)
    {
        int *arg = (int *)PopStack(args);
        Terminal_Set_X(arg);
        kfree(arg);
    }
    else if (strcmp("display::sety", func) == 0)
    {
        int *arg = (int *)PopStack(args);
        Terminal_Set_Y(arg);
        kfree(arg);
    }
    else if (strcmp("display::setcolorfg", func) == 0)
    {
        int *arg = (int *)PopStack(args);
        Terminal_SetColorFG((char)arg);
        kfree(arg);
    }
    else if (strcmp("display::setcolorbg", func) == 0)
    {
        int *arg = (int *)PopStack(args);
        Terminal_SetColorBG((char)arg);
        kfree(arg);
    }
    else if (strcmp("display::clear", func) == 0)
    {
        Termianl_Clear();
    }
}