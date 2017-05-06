#include "kstdlib.h"

char tbuf[32];
char bchars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
const char *itoh_map = "0123456789ABCDEF";

char *itoh(int i, char *buf)
{
  int n;
  int b;
  int z;
  int s;

  if (sizeof(void *) == 4)
    s = 8;
  if (sizeof(void *) == 8)
    s = 16;

  for (z = 0, n = (s - 1); n > -1; --n)
  {
    b = (i >> (n * 4)) & 0xf;
    buf[z] = itoh_map[b];
    ++z;
  }
  buf[z] = 0;
  return buf;
}
void itoa(unsigned i, unsigned base, char *buf)
{
  int pos = 0;
  int opos = 0;
  int top = 0;

  if (i == 0 || base > 16)
  {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  while (i != 0)
  {
    tbuf[pos] = bchars[i % base];
    pos++;
    i /= base;
  }
  top = pos--;
  for (opos = 0; opos < top; pos--, opos++)
  {
    buf[opos] = tbuf[pos];
  }
  buf[opos] = 0;
}

void itoa_s(int i, unsigned base, char *buf)
{
  if (base > 16)
    return;
  if (i < 0)
  {
    *buf++ = '-';
    i *= -1;
  }
  itoa(i, base, buf);
}