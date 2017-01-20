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

        //kfree(arg);
    }
}