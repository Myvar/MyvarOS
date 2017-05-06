#ifndef KERNEL_HEAP_H
#define KERNEL_HEAP_H
// These defines can be machine and compiler specific. They should be
// correctly defined and `uintptr` should be pointer sized since it is
// expected to hold pointer/memory-address values.
typedef unsigned int uint32;
// This needs to be 64-bit on a 64-bit machine and not 32-bit.
typedef uint32 uintptr;
typedef unsigned char uint8;
typedef unsigned short uint16;

typedef struct _KHEAPBLOCKBM {
	struct _KHEAPBLOCKBM	*next;
	uint32					size;
	uint32					used;
	uint32					bsize;
    uint32                  lfb;
} KHEAPBLOCKBM;
 
typedef struct _KHEAPBM {
	KHEAPBLOCKBM			*fblock;
} KHEAPBM;

void k_heapBMInit(KHEAPBM *heap);
int k_heapBMAddBlock(KHEAPBM *heap, uintptr addr, uint32 size, uint32 bsize);
void *k_heapBMAlloc(KHEAPBM *heap, uint32 size);
void k_heapBMFree(KHEAPBM *heap, void *ptr);
#endif