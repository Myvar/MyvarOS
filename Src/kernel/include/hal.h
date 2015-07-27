#ifndef __HAL_H__
#define __HAL_H__

#include <types.h>

extern uint8 inportb (uint16 _port);
extern void outportb (uint16 _port, uint8 _data);

#endif
