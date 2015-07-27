#ifndef __TYPES_H__
#define __TYPES_H__

typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

typedef signed long long int64;
typedef unsigned long long uint64;

typedef char* string;

typedef struct {
	int32 X, Y;
} Point;

typedef struct {
	uint32 X, Y;
} UPoint;

#endif