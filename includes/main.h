extern void Test_Paging();

extern void Termianl_Init();
extern void Termianl_Clear();
extern void Terminal_SetColor(char color, char bg);
extern void putc(char c);
extern void puts(char * str);
extern void putLog(char* string);
extern void putWarn(char* string);
extern void putError(char* string);
extern int Terminal_Get_X();
extern int Terminal_Get_Y();
extern void Terminal_Set_X(int c);
extern void Terminal_Set_Y(int c);

extern char inportb (short _port);
extern void outportb (short _port, char _data);

extern void Serial_Init();
extern char Read_Serial();
extern void Write_Serial(char a);
extern void Write_Serial_Str(char* a);
extern void Serial_SetPort(int p);

extern void *memcpy(void *dest, const void *src, int count);
extern void *memset(void *dest, char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern void itoa(unsigned i,unsigned base,char* buf);

extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void Idt_Install();

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;  
};

struct Command {
   char  name[50];
   char  helpmsg[255];
   void (*handler)(char* args[])
};  

extern void Isrs_Install();

extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void Irq_Install();

extern void keyboard_install();
extern void kb_install_handler(int index, void (*handler)(char c));
extern void kb_uninstall_handler(int handler);

extern void Shell_Init();
extern void Shell_Registor_Command(char name[50], char helpmsg[255] , void (*handler)(char* args[]));

extern int strlen(const char *str);
extern char *strcpy(char *s1, const char *s2);
extern char *strcpyc(char *s1, char s2);
extern int strcmp(char *a, char *b);
extern void strappend(char* s, char c);
extern void strremovelast(char* s);
extern char ** strsplit(char* a_str, char a_delim);
extern char *strclamp(char *str);

extern void Init_Commands();

extern void Init_Paging();

extern void* kmalloc(unsigned int size);
extern void kfree(void *ptr);