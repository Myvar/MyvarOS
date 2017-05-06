#ifndef KSTDLIB_H
#define KSTDLIB_H

#include "memory.h"


extern void itoa(unsigned i, unsigned base, char *buf);
extern void itoa_s(int i, unsigned base, char *buf);
extern char *itoh(int i, char *buf);

#endif
