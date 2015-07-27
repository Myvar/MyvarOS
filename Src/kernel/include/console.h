#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <types.h>

#define ConsoleWidth 80
#define ConsoleHeight 25
#define ConsoleDefaultColor 0xF

enum Colors {
	ColorBlack			=	0x0,
	ColorBlue			=	0x1,
	ColorGreen			=	0x2,
	ColorCyan			=	0x3,
	ColorRed			=	0x4,
	ColorMagenta		=	0x5,
	ColorBrown			=	0x6,
	ColorLightGray		=	0x7,
	ColorDarkGray		=	0x8,
	ColorLightBlue		=	0x9,
	ColorLightGreen		=	0xA,
	ColorLightCyan		=	0xB,
	ColorLightRed		=	0xC,
	ColorLightMagenta	      =	0xD,
	ColorYellow			=	0xE,
	ColorWhite			=	0xF
};

extern void Console_Clear();
extern void Console_Write_Char(char c);
extern void Console_Write_Char_Colored (char c, uint8 color);
extern void Console_Write_String (string s);
extern void Console_Write_String_Colored (string s, uint8 color);

#endif
