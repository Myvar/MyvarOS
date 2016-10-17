#include <main.h>


void *kb_handlers[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);

   
    if (scancode & 0x80)
    {
 
    }
    else
    {       
        //putc(kbdus[scancode]);
        int i  = 0;
        for(i = 0; i < 16; i++)
        {
            void (*handler)(char c) = kb_handlers[i];

            if(kb_handlers[i] != 0)
            {
                handler(kbdus[scancode]);
            }
        }
    }
}

void Kb_Sendkey(char c)
{
    int i  = 0;
    for(i = 0; i < 16; i++)
    {
        void (*handler)(char c) = kb_handlers[i];

        if(kb_handlers[i] != 0)
        {
            handler(c);
        }
    }
}

char convert_scancode(int c)
{
    return kbdus[c];
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}


void kb_install_handler(int index, void (*handler)(char c))
{
    kb_handlers[index] = handler;
}

void kb_uninstall_handler(int handler)
{
    kb_handlers[handler] = 0;
}