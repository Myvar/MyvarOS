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
    retval->executing = 1;

    int n = sizeof(exe) / sizeof(exe[0]);//caulate size of exe
    memcpy(retval->ram, exe, n);//copy the exe into the ram

    retval->stackoffset = n;

    return retval;
}

void vm_step(VM *vm)
{
    unsigned char opcode = vm->ram[vm->pc];

    kprintf("Opcode: %x at offset %x \n", opcode, vm->pc);

    switch (opcode)
    {
        case 0xFF:
            //stop vm
            vm->executing = 0;
            break;
    }


    if(vm->pc > MAXRAM)
    {
        //stop vm
        vm->executing = 0;
    }

    vm->pc++;
}

void vm_run_until_done(VM *vm)
{
    while(vm->executing == 1)
    {
        vm_step(vm);
    }
}

void vm_free(VM *vm)
{
    kfree(vm->ram);
    kfree(vm);
}

