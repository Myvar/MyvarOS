#ifndef KERNEL_SEDNA_H
#define KERNEL_SEDNA_H

#define READ_SEDNA_INT(base, byte_offset) *((signed int *)(base + byte_offset))
typedef unsigned char SEDNA_OP_WORD;
typedef signed int SEDNA_INT;

typedef struct
{
    unsigned char *name;
    unsigned short param_cnt;
    unsigned char **param_type;
    unsigned int op_cnt;
    unsigned int bytecode_sz;
    unsigned char *bytecode;
} SEDNAMETHOD;

typedef struct
{
    unsigned char *scope;
    unsigned int import_cnt;
    unsigned char **import;
    unsigned int type_cnt;
    unsigned char **type;
    unsigned char **type_base;
    unsigned int method_cnt;
    SEDNAMETHOD *method;
} SEDNAMODULE;

typedef struct
{
    unsigned char *name;
} ModuleDef;

typedef struct
{
    unsigned int count;
    int * stack;
} Stack;

typedef struct
{
    unsigned char *name;
    unsigned char *module;
    void (*handler)(char* modulename, char* func, Stack* args);
} FuncDef;

extern void *PopStack(Stack *stack);
extern void PushStack(Stack *stack, void *value);

extern void sedna_system_call(char* modulename, char* func, Stack* args);

extern void sedna_execute_module(SEDNAMODULE *mod);
extern void Sedna_Init();

#endif