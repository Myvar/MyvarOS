#include "vm.h"
#include "memory.h"

int guid;

VM *vm_new(char *exe)
{
     VM *retval = kmalloc (sizeof(VM));
     retval->ID = guid++;

     return retval;
}

