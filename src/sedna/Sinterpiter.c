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

void *PopStackWithType(Stack *stack, int* outType)
{
   outType = (int *)stack->type[--stack->count];
    return (void *)stack->stack[stack->count]; 
}

void PushStack(Stack *stack, void *value, int type)
{
    stack->stack[stack->count++] = value;
}

void **Variables;
int VarTypes[1024];

void executeByteCode(char *bytecode, int size)
{

    //create stack
    Stack stack = CreateNewStack(1024);
    Variables = kmalloc(1024 * 4);

    int x, c, j;
    signed int stringLenth, variable;

    for (x = 0; x < size; x++)
    {
        //kprintf("[%x:%x] ", x, bytecode[x]);
        if (bytecode[x] == 0x20) //loadstr
        {
            //puts("LoadStr\n");
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
            //kprintf("Loading %s onto stack\n", buf);
            PushStack(&stack, buf, 0);
            x--;
        }
        else if (bytecode[x] == 0x30) //call
        {
           // puts("Call\n");
            x++;
            stringLenth = *((signed int *)(bytecode + x));
            x += 4;                    //skip the string lenth
            char buf[stringLenth + 1]; // = kmalloc(stringLenth + 1);
            //kprintf("String leng: %x\n", stringLenth);
            for (j = 0; j < stringLenth; j++)
            {
                buf[j] = bytecode[x++];
            }
            buf[stringLenth] = '\0';
            
            sedna_call(buf, &stack);
            kfree(buf);
            x--;
        }
        else if (bytecode[x] == 0x21) //load int
        {
            //puts("loadint\n");
            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth

            PushStack(&stack, variable, 1);
            x--;

        }
        else if (bytecode[x] == 0x40) //create variable
        {
           // puts("createlocl\n");
            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth

            Variables[variable] = kmalloc(1);
            x--;

        }
        else if (bytecode[x] == 0x41) //set local variable
        {
           // puts("setlocal\n");
            int varIndex = (int *)PopStack(&stack);
            int type;
            void *varValue = PopStackWithType(&stack, &type);

            //kprintf("Loading value\"%s\" into variable %x\n", varValue, varIndex);
            kfree(Variables[varIndex]);
            Variables[varIndex] = varValue;
            VarTypes[varIndex] = type;

        }
        else if (bytecode[x] == 0x42) //load local variable
        {
           // puts("ldlocal\n");
            x++;
            variable = *((signed int *)(bytecode + x));
            x += 4; //skip the string lenth

            PushStack(&stack, Variables[variable], VarTypes[variable]);
            x--;
        }

        else if (bytecode[x] == 0x50) //load local variable
        {
            //puts("Tring to add");
            int tA,tB;

            void *varB = (void *)PopStackWithType(&stack, &tA);
            void *varA = (void *)PopStackWithType(&stack, &tB);

            if(tA == tB)
            {
                if(tA == 0)//string
                {
                    PushStack(&stack, strJoin((char*)varA, (char*)varB), tA);
                    //kprintf("%s + %s = %s", (char*)varA, (char*)varB, strJoin((char*)varA, (char*)varB));
                }
            }

           

            
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
