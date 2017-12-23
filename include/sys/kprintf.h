#ifndef __KPRINTF_H
#define __KPRINTF_H

#define VIDEO_MEM 0xFFFFFFFF800B8000

void kprintf(const char *fmt, ...);
void clear_screen();
void print_at(int cursor_x, int cursor_y, int c);
int is_newline(char c);
#endif
