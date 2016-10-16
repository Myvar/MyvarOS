#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H
typedef int (*IRQ_HANDLER)(struct regs *r);

void irq_hook(unsigned short irq_num, IRQ_HANDLER handler);
void process_hooks(struct regs *r);
#endif