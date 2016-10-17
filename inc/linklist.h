#ifndef KERNEL_LINKLIST_H
#define KERNEL_LINKLIST_H

struct _LLITEM {
	struct _LLITEM 		*next;
	struct _LLITEM  	*prev;
	unsigned int 		data;
};

typedef struct _LLITEM LLITEM;
void ll_add_next(LLITEM **existing, LLITEM *new);
void ll_rem(LLITEM *item);

#endif