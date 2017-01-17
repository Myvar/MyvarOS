#include <stdint.h>
#include "main.h"
#include "irq.h"
#include "sedna.h"

void sedna_system_call(char *modulename, char *func, Stack* args)
{
    if (strcmp("stdio::puts", func) == 0)
    {
        char* arg = (char *)PopStack(args);
        puts(arg);
        kfree(arg);
    }
}