#include "mm.h"


void vm_push(VM *vm, void* ptr)
{
    ((int*)vm->ram)[vm->stackoffset + vm->stackpointer++] = ptr;
}

void* vm_pop(VM *vm)
{
    return (void*)((int*)vm->ram)[vm->stackoffset + --vm->stackpointer];
}