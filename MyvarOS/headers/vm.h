#ifndef VM_H
#define VM_H

typedef struct vm
{
    int ID;
    char* ram;
    int mmID;
} VM;

extern VM *vm_new(char *exe);


#endif
