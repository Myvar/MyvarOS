#include "kstdio.h"
#include "tty.h"
#include <stdarg.h>

void kprintf(const char *fmt, ...)
{
  const char *p;
  va_list argp;
  int i;
  char *s;
  char fmtbuf[256];

  va_start(argp, fmt);

  for (p = fmt; *p != '\0'; p++)
  {
    //kputc('w');
    if (*p != '%')
    {
      tty_putc(*p);
      continue;
    }

    switch (*++p)
    {
    case 'c':
      i = va_arg(argp, int);
      tty_putc(i);
      break;
    case 's':
      s = va_arg(argp, char *);
      tty_puts(s);
      break;
    case 'x':
      i = va_arg(argp, int);
      s = itoh(i, fmtbuf);
      tty_puts(s);
      break;
    case 'n':
      i = va_arg(argp, int);
      itoa(fmtbuf, 10, s);
      tty_puts(s);
      break;
    case '%':
      tty_putc('%');
      break;
    }
  }
  va_end(argp);
}