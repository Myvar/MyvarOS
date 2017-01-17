#include <stdint.h>
#include "main.h"
#include "irq.h"
#include "sedna.h"

void sedna_execute_module(SEDNAMODULE *mod)
{
    signed int x;
    signed int y;

    // kprintf("Loading Scope: %s\n", mod->scope);

    SEDNAMETHOD mainMeth;

    for (x = 0; x < mod->method_cnt; x++)
    {
        if (strcmp(mod->method[x].name, "Main") == 0)
        {
            mainMeth = mod->method[x];
            break;
        }
    } //find entry point

    executeByteCode(mainMeth.bytecode, mainMeth.bytecode_sz);
}

Stack CreateNewStack(unsigned int size)
{
    Stack ret;

    ret.stack = kmalloc(size);

    return ret;
}

void *PopStack(Stack *stack)
{
    return (void *)stack->stack[--stack->count];
}

void PushStack(Stack *stack, void *value)
{
    stack->stack[stack->count++] = value;
}

void executeByteCode(char *bytecode, int size)
{

    //create stack
    Stack stack = CreateNewStack(1024);

    int x, c, j;
    signed int stringLenth;

    for (x = 0; x < size; x++)
    {
        //kprintf("Opcode[%x]\n", bytecode[x]);
        if (bytecode[x] == 0x20) //loadstr
        {
            x++;
            stringLenth = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            char *buf = kmalloc(stringLenth + 1);
            //kprintf("String leng: %x\n", stringLenth);
            for (j = 0; j < stringLenth; j++)
            {
                buf[j] = bytecode[x++];
            }
            buf[stringLenth] = '\0';
            //puts(buf);
            PushStack(&stack, buf);
            x--;
        }
        if (bytecode[x] == 0x30)
        {
            x++;
            stringLenth = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            char buf[stringLenth + 1];// = kmalloc(stringLenth + 1);
            //kprintf("String leng: %x\n", stringLenth);
            for (j = 0; j < stringLenth; j++)
            {
                buf[j] = bytecode[x++];
            }
            buf[stringLenth] = '\0';
            //puts(buf);
            sedna_call(buf, &stack);
            kfree(buf);
            x--;
        }
    }
}

ModuleDef Modules[1024];
unsigned int ModulesIndex;

FuncDef Functions[1024];
unsigned int FunctionsIndex;

void Sedna_Init()
{

    sedna_new_module("Kernel");

    //the reson every function is loaded oneby one is so that,
    //public and private can be inforeced, and error cheacking

    //normaly we whould install a senda module so that a program can load a lib
    //dynamicly, we will patch the Kernel module into a custom handler sotat we might
    //hand system calles
    sedna_install_function("Kernel", "stdio::puts", sedna_system_call);

    //when a new sedna module is loeaded it must be installed
}

void sedna_new_module(char *name)
{
    ModuleDef mod;
    strcpy(mod.name, name);

    Modules[ModulesIndex] = mod;
    ModulesIndex++;
}

int ModuleExsists(char *module)
{
    int i;
    for (i = 0; i < ModulesIndex; i++)
    {
        if (strcmp(Modules[i].name, module))
        {
            return 1;
        }
    }

    return 0;
}

void sedna_install_function(char *module, char *name, void (*handler)(char *modulename, char *func, Stack *args))
{
    //  if (ModuleExsists(module) == 1)
    //{
    FuncDef funcDef;

    strcpy(funcDef.name, name);
    strcpy(funcDef.module, module);
    funcDef.handler = handler;

    Functions[FunctionsIndex] = funcDef;
    FunctionsIndex++;
    // }
}

void sedna_call(char *path, Stack *args)
{

    char *module = strremovefirst(strtok_once(path, "]"));

    char *func = strremovefirstCount(path, strlen(module) + 2);

    int i, j;
    for (i = 0; i < FunctionsIndex; i++)
    {

        if (strcmp(Functions[i].module, module) == 0)
        {
            if (strcmp(Functions[i].name, func))
            {
                Functions->handler(module, func, args);
                break;
            }
        }
    }

    kfree(module);
    kfree(func);
}
