#include <stdint.h>
#include "main.h"
#include "irq.h"


void Clear(char* args[])
{
    Termianl_Clear();
}

void Dump_Command(char* args[])
{
    register int eax asm("eax");
    register int ebx asm("ebx");
    register int ecx asm("ecx");
    register int edx asm("edx");

    char buf[255];

    itoa(eax, 10, buf);
    puts("Eax: ");
    puts(buf);
    puts("\n");
    
    itoa(ebx, 10, buf);
    puts("Ebx: ");
    puts(buf);
    puts("\n");

    itoa(ecx, 10, buf);
    puts("Ecx: ");
    puts(buf);
    puts("\n");

    itoa(edx, 10, buf);
    puts("Edx: ");
    puts(buf);
    puts("\n");
}

void DevidebyZero(char* args[])
{
    int a = 1;
    int b = 0;

    int c =  a / b;
    putc(c);
}

void Args_Test(char* args[])
{
    int i = 0;
    for(i = 0; i < sizeof(args); i++)
    {
        puts(args[i]);
        puts("\n");
    }
}

void Test_Paging()
{
    putLog("Starting Paging Test");

    char* zero = 0x0;

    int i = 0;
    for(i = 0; i < (1024 * 1024); i++)
    {
        char buf[255];

        char rd = *zero;
        


        itoa((int)zero, 10, buf);
        putLog(buf);

        zero += 4096;
    }

    putLog("Paging passed Sucesfullt");
}


void Test_PageFalt()
{
    int c = *((char*)0x400000);
}



void Test_CDRead() {
    irq_hook(47, &atapi_irq_hook);

    atapi_read(0x170, 0xa0);
}


void Test_Heap()
{
    void* mem = kmalloc(8);
    putLog("Allocating 8 bytes of memmory");

    char buf[255];

    itoa(mem, 16, buf);
    puts("Addres: 0x");
    puts(buf);
    puts("\n");
}

void PutString(char *s, int lenght )
{    
    int i = 0;
    for(i = 0; i < lenght; i++)
    {
        putc(s[i]);       
    }

}

#define READ_SEDNA_INT(base, byte_offset) *((int32_t*)(base + byte_offset))
typedef uint8_t SEDNA_OP_WORD;
typedef int32_t SEDNA_INT;

typedef struct {
    uint8_t             *name;
    uint16_t            param_cnt;
    uint8_t             **param_type;
    uint32_t            op_cnt;
    uint32_t            bytecode_sz;
    uint8_t             *bytecode;
} SEDNAMETHOD;

typedef struct {
    uint8_t             *scope;
    uint32_t            import_cnt;
    uint8_t             **import;
    uint32_t            type_cnt;
    uint8_t             **type;
    uint8_t             **type_base;
    uint32_t            method_cnt;
    SEDNAMETHOD         *method;
} SEDNAMODULE;

/*
    Copies a string value specified by offset and length into a newly allocated
    string sequence terminated by a null byte.
*/
static uint8_t *alloc_null_terminated_string(void *offset, uintptr_t size) {
    uint8_t *out;

    if (size == 0) {
        // The most safe action, but not that efficient.
        out = (uint8_t*)kmalloc(1);
        *out = 0;
        return out;
    }

    if (*((uint8_t*)offset + size - 1) == 0) {
        out = (uint8_t*)kmalloc(size);
        if (out == 0) {
            panic("alloc_null_terminated_string OOM");
        }
        memcpy(out, offset, size);
    } else {
        out = (uint8_t*)kmalloc(size + 1);
        if (out == 0) {
            panic("alloc_null_terminated_string OOM");
        }
        memcpy(out, offset, size);
        out[size] = 0;
    }

    return out;
}

extern uint8_t KERNEL_START_VADDR;

SEDNAMODULE* sedna_load_module(uintptr_t address)
{
    SEDNAMODULE     *mod;
    SEDNAMETHOD     *meth;
    int32_t         sz;
    int32_t         index;
    uint32_t        offset;
    int32_t         x;

    mod = kmalloc(sizeof(SEDNAMODULE));

    sz = READ_SEDNA_INT(address, 0);
    offset += sizeof(SEDNA_INT);

    //kprintf("scope-address: %x scope-length: %x", (uintptr_t)address + offset, sz);
    mod->scope = alloc_null_terminated_string((uint8_t*)address + offset, sz);
    offset += sz;

    //kprintf(" scope: %s\n", mod->scope);


    int32_t total_imports = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    //kprintf("total-imports: %x\n", total_imports);

    mod->import_cnt = total_imports;
    mod->import = (uint8_t**)kmalloc(sizeof(uint8_t*) * mod->import_cnt);

    for(index = 0; index < total_imports; index++)
    {   
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->import[index] = alloc_null_terminated_string((uint8_t*)address + offset, sz);
        offset += sz;

        //kprintf("import-address: %x import-size: %x import: %s\n", (uintptr_t)address + offset, sz, mod->import[index]);
    }

    char *s;
    char buf[20];

    int32_t total_types = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    mod->type_cnt = total_types;
    mod->type = (uint8_t**)kmalloc(sizeof(uint8_t*) * mod->type_cnt);
    mod->type_base = (uint8_t**)kmalloc(sizeof(uint8_t*) * mod->type_cnt);

    for(index = 0; index < total_types; index++)
    {   
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->type[index] = alloc_null_terminated_string((uint8_t*)address + offset, sz);
        offset += sz;

        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->type_base[index] = alloc_null_terminated_string((uint8_t*)address + offset, sz);
        offset += sz;

        //kprintf("type: %s base: %s\n", mod->type[index], mod->type_base[index]);
    }


    int32_t total_fns = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    mod->method_cnt = total_fns;
    mod->method = (SEDNAMETHOD*)kmalloc(sizeof(SEDNAMETHOD) * mod->method_cnt);

    for(index = 0; index < total_fns; index++)
    {   
        // Method name.
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->method[index].name = alloc_null_terminated_string((uint8_t*)address + offset, sz);
        offset += sz;

        int32_t total_para = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);

        mod->method[index].param_cnt = total_para;
        mod->method[index].param_type = (uint8_t**)kmalloc(sizeof(uint8_t*) * mod->method[index].param_cnt);

        //kprintf("method: %s parameter-count: %x\n", mod->method[index].name, mod->method[index].param_cnt);

        for(x = 0; x < total_para; x++)
        {
            sz = READ_SEDNA_INT(address, offset);
            offset += sizeof(SEDNA_INT);
            mod->method[index].param_type[x] = alloc_null_terminated_string((uint8_t*)address + offset, sz);
            //kprintf("   method-parameter-type: %s\n", mod->method[index].param_type[x]);
            offset += sz;
        }
        
        // This was added to keep from interpreting the bytecode
        // at this very moment.
        SEDNA_INT bytecode_sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);

        SEDNA_INT op_cnt = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);

        //kprintf("   op-count: %x bytecode-sz: %x\n", op_cnt, bytecode_sz);

        mod->method[index].op_cnt = op_cnt;
        mod->method[index].bytecode_sz = bytecode_sz;
        mod->method[index].bytecode = kmalloc(bytecode_sz);
        memcpy(mod->method[index].bytecode, (uint8_t*)(address + offset), bytecode_sz);
    }

    return mod;
}

unsigned int            SednaStart;

void Test_Sedna() {
    SEDNAMODULE *mod;
    int32_t x;
    int32_t y;

    /*
        Use specialized function for loading a Sedna module. The
        `mod` variable is a pointer to an allocated object in the
        heap. This allows anything located at the address to be
        removed afterwards if fully loaded into the SEDNAMODULE
        object above.
    */
    mod = sedna_load_module((uintptr_t)&SednaStart);

    kprintf("mod-scope-name: %s\n", mod->scope);
    kprintf(" import-count: %x type-count: %x method-count: %x\n",
        mod->import_cnt,
        mod->type_cnt,
        mod->method_cnt
    );

    for (x = 0; x < mod->import_cnt; ++x) {
        kprintf(" import[%x]: %s\n", x, mod->import[x]);
    }

    for (x = 0; x < mod->type_cnt; ++x) {
        kprintf(" type[%x]: %s base: %s\n", 
            x,
            mod->type[x], mod->type_base[x]
        );
    }

    for (x = 0; x < mod->method_cnt; ++x) {
        kprintf(" method[%x]: %s param-count: %x op-count: %x bytecode-sz: %x\n",
            x,
            mod->method[x].name,
            mod->method[x].param_cnt,
            mod->method[x].op_cnt,
            mod->method[x].bytecode_sz
        );

        for (y = 0; y < mod->method[x].param_cnt; ++y) {
            kprintf("  param-type[%x]: %s\n", y, mod->method[x].param_type[y]);
        }
    }

    kprintf("end of module\n");
}

void Init_Commands()
{

    Shell_Registor_Command("clear", "Clears the screen", Clear);
    Shell_Registor_Command("cls", "Clears the screen", Clear);

    Shell_Registor_Command("debug", "Dumps the registors to the screen", Dump_Command);
    Shell_Registor_Command("dump", "Dumps the registors to the screen", Dump_Command);

    Shell_Registor_Command("dbz", "Devide by zero to test isrs", DevidebyZero);
    
    Shell_Registor_Command("args", "Test args Parsing", Args_Test);

    Shell_Registor_Command("ptest", "Test Paging", Test_Paging);
    Shell_Registor_Command("pfalt", "Cause Page Falt", Test_PageFalt);

    Shell_Registor_Command("theap", "Test Heap", Test_Heap);    

    Shell_Registor_Command("cdreadtest", "Test reading CD-ROM", Test_CDRead);

    Shell_Registor_Command("tsedna", "Sedna testing command", Test_Sedna);
}