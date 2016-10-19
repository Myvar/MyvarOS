#ifndef KERNEL_SEDNA_H
#define KERNEL_SEDNA_H

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



#endif