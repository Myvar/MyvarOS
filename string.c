#include <main.h>

int strlen(const char *str)
{
    int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while ((*s1++ = *s2++));
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
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a - *b;
}

void strappend(char* s, char c)
{

    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

void strremovelast(char* s)
{

    int len = strlen(s);
    s[len - 1] = '\0';
}