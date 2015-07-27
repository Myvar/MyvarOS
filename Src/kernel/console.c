#include <console.h>

UPoint ConsoleCursor = {0, 0};
UPoint ConsoleSize = {ConsoleWidth, ConsoleHeight};

uint8* VideoRam = (uint8*)0xb8000;

void Console_Clear()
{
	uint32 i;
	for (i = 0; i < ConsoleSize.X * ConsoleSize.Y * 2; i++)
		VideoRam[i] = 0;
}

void Console_Write_Char(char c)
{
	Console_Write_Char_Colored(c, ConsoleDefaultColor);
}

void Console_Write_Char_Colored (char c, uint8 color)
{
	uint32 location = ConsoleCursor.X + ConsoleCursor.Y * ConsoleSize.X;
	
	VideoRam[2*location] = c;
	VideoRam[2*location + 1] = color;
			
      ConsoleCursor.X++;
	
	if (ConsoleCursor.X >= ConsoleSize.X) {
		ConsoleCursor.X = 0;
		ConsoleCursor.Y ++;
	}
}

void Console_Write_String (string s)
{
	Console_Write_String_Colored (s, ConsoleDefaultColor);
}

void Console_Write_String_Colored (string s, uint8 color)
{
	while (*s != 0)
	{
		Console_Write_Char_Colored (*s, color);
		s = s + 1;
	}
}
