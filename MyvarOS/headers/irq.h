#ifndef IRQ_H
#define IRQ_H

#include "isrs.h"
typedef int (*IRQ_HANDLER)(struct regs *r);

extern void Irq_Install();
extern void irq_hook(unsigned short irq_num, IRQ_HANDLER handler);

#endif
