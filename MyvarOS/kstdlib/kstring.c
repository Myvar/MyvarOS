#include "kstring.h"

void *memcpy(void *dest, const void *src, int count)
{
  const char *sp = (const char *)src;
  char *dp = (char *)dest;
  for (; count != 0; count--)
    *dp++ = *sp++;
  return dest;
}

void *memset(void *dest, char val, int count)
{
  char *temp = (char *)dest;
  for (; count != 0; count--)
    *temp++ = val;
  return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
  unsigned short *temp = (unsigned short *)dest;
  for (; count != 0; count--)
    *temp++ = val;
  return dest;
}

int strlen(const char *str)
{
    int retval;
    for (retval = 0; *str != '\0'; str++)
        retval++;
    return retval;
}

char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while ((*s1++ = *s2++))
        ;
    return s1_p;
}

char *strcpyc(char *s1, char s2)
{
    char *s1_p = s1;
    *s1++ = s2;
    return s1_p;
}

int strcmp(char *a, char *b)
{
    while (*a && *b && *a == *b)
    {
        ++a;
        ++b;
    }
    return *a - *b;
}