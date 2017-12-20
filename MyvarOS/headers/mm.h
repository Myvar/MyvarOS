#ifndef MM_H
#define MM_H
#include "vm.h"

extern void  vm_push(VM *vm, void* ptr);
extern void* vm_pop(VM *vm);

#endif