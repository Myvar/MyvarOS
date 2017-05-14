#include "vm.h"
#include "memory.h"
#include "kstring.h"

#define MAXRAM 524288
int guid;

VM *vm_new(char *exe)
{
    //create new object
     VM *retval = kmalloc (sizeof(VM));
     retval->ID = guid++; //new guid
     retval->ram = kmalloc(MAXRAM); //init ram

     int n = sizeof(exe) / sizeof(exe[0]);//caulate size of exe
     memcpy(retval->ram, exe, n);//copy the exe into the ram



     return retval;
}

void vm_free(VM *vm)
{
  
}

