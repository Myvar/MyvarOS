#include <main.h>

char* LFB;
int LFB_X;
int LFB_Y;
int VGA_WIDTH = 80;
int VGA_HEIGHT = 25;

char BG = 0x0f;

void Terminal_SetColor(char color, char bg)
{
    BG = (char) color | (char) bg << 8;
}

void Termianl_Init()
{
    LFB = (char*)0xB8000;
    LFB_X = 0;
    LFB_Y = 0;
}

int Terminal_Get_X()
{
    return LFB_X;
}

int Terminal_Get_Y()
{
    return LFB_Y;
}

void Terminal_Set_X(int c)
{
    LFB_X = c;
}

void Terminal_Set_Y(int c)
{
    LFB_Y = c;
}

void Termianl_Clear()
{
    int i = 0;
    
    for(i = 0; i < 80 * 25 * 2; i++)
    {
        LFB[i] = 0;
    }

    LFB_X = 0;
    LFB_Y = 0;
}

void putLog(char* string)
{
    BG = 0x0f;
    putc('[');
    BG = 0x0A;
    puts("Log");
    BG = 0x0f;
    putc(']');
    puts(string);
    putc('\n');

    //serial
    Write_Serial('[');
    Write_Serial_Str("\e[32mLog\e[0m");
    Write_Serial(']');
    Write_Serial_Str(string);
    Write_Serial('\n');
}

void putWarn(char* string)
{
    //terminal
    BG = 0x0f;
    putc('[');
    BG = 0x0E;
    puts("Warn");
    BG = 0x0f;
    putc(']');
    puts(string);
    putc('\n');

    //serial
    Write_Serial('[');
    Write_Serial_Str("\e[33mWarn\e[0m");
    Write_Serial(']');
    Write_Serial_Str(string);
    Write_Serial('\n');
}

void putError(char* string)
{
    BG = 0x0f;
    putc('[');
    BG = 0x0C;
    puts("Error");
    BG = 0x0f;
    putc(']');
    puts(string);
    putc('\n');

    //serial
    Write_Serial('[');
    Write_Serial_Str("\e[31mError\e[0m");
    Write_Serial(']');
    Write_Serial_Str(string);
    Write_Serial('\n');
}

void putc(char c)
{
    if(c == '\n')
    {
        LFB_X = 0;
        LFB_Y++;
    }
    else
    {
        if(c == '\t')
        {
            LFB_X += 4;
        }
        else
        { 
            if(c == '\b')
            {
                int offset = ((LFB_Y * VGA_WIDTH) + LFB_X - 1) * 2;

                LFB[offset] = ' ';
                LFB[offset + 1] = 0x0F;

                LFB_X--;
            
            }
            else
            { 
                int offset = ((LFB_Y * VGA_WIDTH) + LFB_X) * 2;

                LFB[offset] = c;
                LFB[offset + 1] = BG;

                LFB_X++;

                if(LFB_X >= 80)
                {
                    LFB_X = 0;
                    LFB_Y++;
                }
            }
        }
    }
}

void puts(char * str)
{
    while( *str != 0 )
    {
        putc(*str++);
    }
}