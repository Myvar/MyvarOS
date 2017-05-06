#ifndef TTY_H
#define TTY_H

extern void tty_set_color(char color, char bg);
extern void tty_reset_color();
extern void tty_init();
extern void tty_clear();
extern void tty_log(char *string);
extern void tty_warn(char *string);
extern void tty_error(char *string);
extern void tty_scroll_up();
extern void tty_putc(char c);
extern void tty_puts(char *str);

#endif
