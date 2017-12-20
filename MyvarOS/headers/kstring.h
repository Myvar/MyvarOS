#ifndef KSTRING_H
#define KSTRING_H

extern void *memcpy(void *dest, const void *src, int count);
extern void *memset(void *dest, char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(const char *str);
extern char *strcpy(char *s1, const char *s2);
extern char *strcpyc(char *s1, char s2);
extern int strcmp(char *a, char *b);

#endif