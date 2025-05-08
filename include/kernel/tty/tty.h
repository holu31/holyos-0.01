#ifndef TTY_H
#define TTY_H

#include "kernel/video.h"
#include "kernel/cpu/spinlock.h"
#include "kernel/input.h"

#include "stdint.h"
#include "stddef.h"
#include "stdarg.h"

#define TTY_BUFFER_SIZE 128

typedef struct {
	spinlock_t lock;
	char buffer[TTY_BUFFER_SIZE];
	uint32_t buf_pos;
	uint16_t width;
	uint16_t height;
	video_color_t fg;
	video_color_t bg;
	uint16_t xpos;
	uint16_t ypos;
} tty_t;

void tty_init(void);
void tty_handle_keypress(key_event event);
void tty_execute_command(const char* cmd);
void tty_putchar(char c);
void tty_write(const char* str);
void tty_printf(char* fmt, ...);
void tty_clear(void);

#endif // TTY_H
