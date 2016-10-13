#include <main.h>


void kernel_main(void) 
{
	
    Termianl_Init();
    Termianl_Clear();

    Serial_SetPort(0x3F8);//COM1
    Serial_Init();

    putLog("Started Serial Terminal");

    Idt_Install();

    putLog("IDT Installed");

    Isrs_Install();

    putLog("Isrs Installed");

    Irq_Install();

    putLog("Irq Installed");

    __asm__ __volatile__ ("sti");
    putLog("MyvarOS booted");
    
    putLog("Installing Keybord");
    keyboard_install();

    putLog("Starting Shell");
    Shell_Init();

    while (1 == 1)
    {
       putc(Read_Serial());
    }
}



char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void *memcpy(void *dest, const void *src, int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, int count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}


void itoa(unsigned i,unsigned base,char* buf) 
{
   int pos = 0;
   int opos = 0;
   int top = 0;

   if (i == 0 || base > 16) {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0) {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   for (opos=0; opos<top; pos--,opos++) {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
}

void itoa_s(int i,unsigned base,char* buf)
{
   if (base > 16) return;
   if (i < 0) {
      *buf++ = '-';
      i *= -1;
   }
   itoa(i,base,buf);
}