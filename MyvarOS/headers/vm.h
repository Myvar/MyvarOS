#ifndef VM_H
#define VM_H

typedef struct vm
{
    int ID;
} VM;

extern VM *vm_new(char *exe);


#endif
