#ifndef VM_H
#define VM_H

typedef struct vm
{
    int ID;
    char* ram;
    int mmID;
    char executing;
    int stackoffset;
    int stackpointer;
    int pc;//program counter
} VM;

extern VM *vm_new(char *exe);
extern void vm_free(VM *vm);
extern void vm_step(VM *vm);
extern void vm_run_until_done(VM *vm);

#endif
