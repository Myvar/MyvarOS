#include <stdint.h>
#include "main.h"
#include "irq.h"
#include "sedna.h"

static unsigned char *alloc_null_terminated_string(void *offset, uintptr_t size) {
    unsigned char *out;

    if (size == 0) {
        // The most safe action, but not that efficient.
        out = (unsigned char*)kmalloc(1);
        *out = 0;
        return out;
    }

    if (*((unsigned char*)offset + size - 1) == 0) {
        out = (unsigned char*)kmalloc(size);
        if (out == 0) {
            panic("alloc_null_terminated_string OOM");
        }
        memcpy(out, offset, size);
    } else {
        out = (unsigned char*)kmalloc(size + 1);
        if (out == 0) {
            panic("alloc_null_terminated_string OOM");
        }
        memcpy(out, offset, size);
        out[size] = 0;
    }

    return out;
}

SEDNAMODULE* sedna_load_module(uintptr_t address)
{
    SEDNAMODULE     *mod;
    SEDNAMETHOD     *meth;
    signed int         sz;
    signed int         index;
    unsigned int        offset;
    signed int         x;

    mod = kmalloc(sizeof(SEDNAMODULE));

    sz = READ_SEDNA_INT(address, 0);
    offset += sizeof(SEDNA_INT);

    //kprintf("scope-address: %x scope-length: %x", (uintptr_t)address + offset, sz);
    mod->scope = alloc_null_terminated_string((unsigned char*)address + offset, sz);
    offset += sz;

    //kprintf(" scope: %s\n", mod->scope);


    signed int total_imports = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    //kprintf("total-imports: %x\n", total_imports);

    mod->import_cnt = total_imports;
    mod->import = (unsigned char**)kmalloc(sizeof(unsigned char*) * mod->import_cnt);

    for(index = 0; index < total_imports; index++)
    {   
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->import[index] = alloc_null_terminated_string((unsigned char*)address + offset, sz);
        offset += sz;

        //kprintf("import-address: %x import-size: %x import: %s\n", (uintptr_t)address + offset, sz, mod->import[index]);
    }

    char *s;
    char buf[20];

    signed int total_types = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    mod->type_cnt = total_types;
    mod->type = (unsigned char**)kmalloc(sizeof(unsigned char*) * mod->type_cnt);
    mod->type_base = (unsigned char**)kmalloc(sizeof(unsigned char*) * mod->type_cnt);

    for(index = 0; index < total_types; index++)
    {   
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->type[index] = alloc_null_terminated_string((unsigned char*)address + offset, sz);
        offset += sz;

        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->type_base[index] = alloc_null_terminated_string((unsigned char*)address + offset, sz);
        offset += sz;

        //kprintf("type: %s base: %s\n", mod->type[index], mod->type_base[index]);
    }


    signed int total_fns = READ_SEDNA_INT(address, offset);
    offset += sizeof(SEDNA_INT);

    mod->method_cnt = total_fns;
    mod->method = (SEDNAMETHOD*)kmalloc(sizeof(SEDNAMETHOD) * mod->method_cnt);

    for(index = 0; index < total_fns; index++)
    {   
        // Method name.
        sz = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);
        mod->method[index].name = alloc_null_terminated_string((unsigned char*)address + offset, sz);
        offset += sz;

        signed int total_para = READ_SEDNA_INT(address, offset);
        offset += sizeof(SEDNA_INT);

        mod->method[index].param_cnt = total_para;
        mod->method[index].param_type = (unsigned char**)kmalloc(sizeof(unsigned char*) * mod->method[index].param_cnt);

      //  kprintf("method: %s parameter-count: %x\n", mod->method[index].name, mod->method[index].param_cnt);

        for(x = 0; x < total_para; x++)
        {
            sz = READ_SEDNA_INT(address, offset);
            offset += sizeof(SEDNA_INT);
            mod->method[index].param_type[x] = alloc_null_terminated_string((unsigned char*)address + offset, sz);
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
        memcpy(mod->method[index].bytecode, (unsigned char*)(address + offset), bytecode_sz);
        offset += bytecode_sz;
    }

    return mod;
}