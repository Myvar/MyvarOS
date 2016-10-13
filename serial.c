#include <main.h>
int Port;

void Serial_Init()
 {
   outportb(Port + 1, 0x00);   
   outportb(Port + 3, 0x80);    
   outportb(Port + 0, 0x03);    
   outportb(Port + 1, 0x00);    
   outportb(Port + 3, 0x03);   
   outportb(Port + 2, 0xC7);    
   outportb(Port + 4, 0x0B);
}

void Serial_SetPort(int p)
{
    Port = p;
}

int Serial_Received() 
{
   return inportb(Port + 5) & 1;
}
 
char Read_Serial()
{
   while (Serial_Received() == 0);
 
   return inportb(Port);
}

int is_transmit_empty()
{
   return inportb(Port + 5) & 0x20;
}
 
void Write_Serial(char a) 
{
   while (is_transmit_empty() == 0);
 
   outportb(Port,a);
}

void Write_Serial_Str(char* a) 
{
  
   while( *a != 0 )
    {
        Write_Serial(*a++);
    }
}