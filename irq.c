#include <main.h>

#include "irq.h"
#include "linklist.h"

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

// Allows multiple handlers to be installed per IRQ and
// the order sets the priority since handlers can prevent
// the event from being propogated to the next handler.
static LLITEM *handlers[64];

void irq_hook(unsigned short irq_num, IRQ_HANDLER handler) {
    LLITEM *item = kmalloc(sizeof(LLITEM));

    char buf[20];
    puts("install irq hook ");
    itoa((unsigned int)item, 10, buf);
    puts(buf);
    puts("\n");

    item->data = (unsigned int)handler;
    
    ll_add_next(&handlers[irq_num], item);
}

void process_hooks(struct regs *r) {
    LLITEM *citem = handlers[r->int_no];
    IRQ_HANDLER handler;

    /*
    char buf[20];
    puts("trying ");
    itoa((unsigned int)citem, 10, buf);
    puts(buf);
    puts(" irq\n");
    */

    while (citem) {
        handler = (IRQ_HANDLER)citem->data;
        puts("irq hook found\n");
        if (handler(r) < 0) {
            // The handler aborted propogation of the event.
            break;
        }
        citem = citem->next;
    }
}


void Irq_Install()
{
    for (int x = 0; x < sizeof(handlers) / sizeof(LLITEM*); ++x) {
        handlers[x] = 0;
    }

    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);

    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

void irq_handler(struct regs *r)
{
    
    void (*handler)(struct regs *r);

    
    handler = irq_routines[r->int_no - 32];

    if (handler)
    {
        handler(r);
    }

    process_hooks(r);

    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    outportb(0x20, 0x20);
}