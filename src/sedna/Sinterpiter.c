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

void *PopStackWithType(Stack *stack, int *outType)
{
    stack->count = stack->count - 1;
    *outType = stack->type[stack->count];
    return (void *)stack->stack[stack->count];
}

void PushStack(Stack *stack, void *value, int type)
{
    //kprintf("PUSHINGTOSTACK: type: %x value: %s\n", type, (char*)value);
    stack->type[stack->count] = type;
    //kprintf("PUSHINGTOSTACK(assert): type: %x\n", stack->type[stack->count]);
    stack->stack[stack->count++] = value;
}

void **Variables;
int VarTypes[1024];

void executeByteCode(char *bytecode, int size)
{

    int debug = 0;

    //create stack
    Stack stack = CreateNewStack(1024);
    Variables = kmalloc(1024 * 4);

    int x, c, j;
    signed int stringLenth, variable;

    for (x = 0; x < size; x++)
    {
        if (debug == 1)
        {
            //  kprintf("[%x:%x]\n", x, bytecode[x]);
        }
        if (bytecode[x] == 0x20) //loadstr
        {
            x++;
            stringLenth = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            char *buf = kmalloc(stringLenth + 1);
            for (j = 0; j < stringLenth; j++)
            {
                buf[j] = bytecode[x++];
            }
            buf[stringLenth] = '\0';
            if (debug == 1)
            {
                puts("LoadString:\n");
                kprintf("[LoadString]Loading %s onto stack\n", buf);
            }
            PushStack(&stack, buf, 0);
            x--;
        }
        else if (bytecode[x] == 0x30) //call
        {
            x++;
            stringLenth = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            char *buf = kmalloc(stringLenth + 1);

            for (j = 0; j < stringLenth; j++)
            {
                buf[j] = bytecode[x++];
            }
            buf[stringLenth] = '\0';

            if (debug == 1)
            {
                puts("Call:\n");

                int j = 0;
                for (j = 0; j < stack.count; j++)
                {
                    int type = stack.type[j];

                    if (type == 0)
                    {
                        kprintf("[Call]Stack[%x] == %s\n", j, (char*)stack.stack[j]);
                    }
                    if (type == 1)
                    {
                        kprintf("[Call]Stack[%x] == %x\n", j, (int)(int*)stack.stack[j]);
                    }
                }

                kprintf("[Call] \"%s\" with stack size of: %x\n", buf, stack.count);
            }
            sedna_call(buf, &stack);
            kfree(buf);
            x--;
        }
        else if (bytecode[x] == 0x21) //load int
        {
            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            if (debug == 1)
            {
                puts("LoadInt:\n");
                kprintf("[LoadInt] Pusing %x onto the stack\n", variable);
            }
            PushStack(&stack, variable, 1);
            x--;
        }
        else if (bytecode[x] == 0x40) //create variable
        {
            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            if (debug == 1)
            {
                puts("CreateLocal:\n");
                kprintf("[CreateLocal]Creating variable at %x\n", variable);
            }
            Variables[variable] = kmalloc(1);
            x--;
        }
        else if (bytecode[x] == 0x41) //set local variable
        {
            int varIndex = (int *)PopStack(&stack);
            int type;
            void *varValue = PopStackWithType(&stack, &type);
            if (debug == 1)
            {
                puts("SetLocal:\n");
                if (type == 0)
                {
                    kprintf("[SetLocal]Loading value\"%s\" into variable %x\n", varValue, varIndex);
                }
                if (type == 1)
                {
                    kprintf("[SetLocal]Loading value %x into variable %x\n", varValue, varIndex);
                }
            }

            kfree(Variables[varIndex]);
            Variables[varIndex] = varValue;
            VarTypes[varIndex] = type;
        }
        else if (bytecode[x] == 0x42) //load local variable
        {

            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth
            if (debug == 1)
            {
                puts("LoadLocal:\n");
                if (VarTypes[variable] == 0)
                {
                    kprintf("[LoadLocal]Pusing \"%s\" onto the stack type: %x\n", Variables[variable], VarTypes[variable]);
                }
                if (VarTypes[variable] == 1)
                {
                    kprintf("[LoadLocal]Pusing %x onto the stack type: %x\n", Variables[variable], VarTypes[variable]);
                }
            }
            PushStack(&stack, Variables[variable], VarTypes[variable]);
            x--;
        }

        else if (bytecode[x] == 0x50) //load local variable
        {
            int tA, tB;
            if (debug == 1)
            {
                puts("Add:\n");
            }

            void *varB = (void *)PopStackWithType(&stack, &tA);
            void *varA = (void *)PopStackWithType(&stack, &tB);

            if (tA == tB) //cheack if we doing this to tipes of the same kind
            {
                if (tA == 0) //string
                {
                    PushStack(&stack, strJoin((char *)varA, (char *)varB), 0);
                    if (debug == 1)
                    {
                        kprintf("%s + %s = %s\n", (char *)varA, (char *)varB, strJoin((char *)varA, (char *)varB));
                    }
                }
                if (tA == 1) //int
                {
                    PushStack(&stack, ((int)(signed int *)varA + (int)(signed int *)varB), 1);
                    if (debug == 1)
                    {
                        kprintf("%x + %x = %x\n", (int)(signed int *)varA, (int)(signed int *)varB, (int)(signed int *)varA + (int)(signed int *)varB);
                    }
                }
            }
        }
    }

    if (debug == 1)
    {
        kprintf("Final stack size: %x\n", stack.count);
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
    sedna_install_function("Kernel", "stdio::itos", sedna_system_call);

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

    // kprintf("[Call]Calling module: \"%s\", function: \"%s\"\n", module, func);

    int i, j;
    for (i = 0; i < FunctionsIndex; i++)
    {
        if (strcmp(Functions[i].module, module))
        {
            //  puts("[Call] FoundModule\n");
            if (strcmp(Functions[i].name, func))
            {
               // kprintf("Found Func: \"%s\"\n", func);
                Functions->handler(module, func, args);
                break;
            }
        }
    }

    kfree(module);
    kfree(func);
    kfree(path);
}
