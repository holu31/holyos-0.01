#include "kernel/tty/tty.h"
#include "kernel/cpu/spinlock.h"
#include "kernel/video.h"
#include "kernel/string.h"
#include "kernel/sprintf.h"
#include "kernel/kprintf.h"

static tty_t current_tty;

static void tty_reset_line(void);
static void tty_show_prompt(void);

static void cmd_help(void);
static void cmd_clear(void);
static void cmd_echo(const char* args);

void tty_init() {
	struct video_driver* driver = video_get_current_driver();	

	memset(&current_tty, 0, sizeof(tty_t));		

	spinlock_init(&current_tty.lock);
	current_tty.buf_pos = 0;
	current_tty.width = driver->width;
	current_tty.height = driver->height;
	current_tty.fg = (video_color_t){ 255, 255, 255 };
	current_tty.bg = (video_color_t){ 0, 0, 0 };
	current_tty.xpos = 0;
	current_tty.ypos = 0;

	video_clear(current_tty.bg);
	tty_show_prompt();
}

void tty_execute_command(const char* cmd) {
	if (strcmp(cmd, "help") == 0) {
		cmd_help();
	}
	else if (strcmp(cmd, "clear") == 0) {
		cmd_clear();
	}
	else if (strncmp(cmd, "echo ", 5) == 0) {
		cmd_echo(cmd + 5);
	}
	else {
		tty_printf("unknown command: '%s'\n", cmd);
	}
}


void tty_handle_keypress(key_event event) {
	char ascii = event.ascii;
	
	if (ascii == '\0') return;
	else if (ascii == '\n') {
		tty_putchar('\n');
		current_tty.buffer[current_tty.buf_pos] = '\0';

		if (current_tty.buf_pos > 0)
			tty_execute_command(current_tty.buffer);

		tty_reset_line();
		tty_show_prompt();
	} else if (ascii == '\b') {
		if (current_tty.buf_pos > 0) {
			current_tty.buf_pos--;
			tty_putchar('\b');
		}
	} else if (current_tty.buf_pos < sizeof(current_tty.buffer)) {
		current_tty.buffer[current_tty.buf_pos++] = ascii;
		tty_putchar(ascii);
	}
}

void tty_putchar(char c) {
	spinlock_lock(&current_tty.lock);

	if (c == 0) {
		spinlock_unlock(&current_tty.lock);
		return;
	}

	if (c == '\n') {
		current_tty.xpos = 0;
		current_tty.ypos++;
	} else if (c == '\b') {
		if (current_tty.xpos > 0) {
			if (current_tty.xpos > 0)
				current_tty.xpos--;
			else {
				current_tty.xpos = current_tty.width - 1;
				current_tty.ypos--;
			}

			video_write(' ', current_tty.xpos, current_tty.ypos,
					current_tty.fg, current_tty.bg);
		}
	} else if (c == '\t') {
		current_tty.xpos = (current_tty.xpos + 4) & ~3;
	} else if (c == '\r') {
		current_tty.xpos = 0;
	} else {
		video_write(c, current_tty.xpos, current_tty.ypos, current_tty.fg, current_tty.bg);
		current_tty.xpos++;
	}

	if (current_tty.xpos >= current_tty.width) {
		current_tty.xpos = 0;
		current_tty.ypos++;
	}

	if (current_tty.ypos >= current_tty.height) {
		video_scroll();
		current_tty.ypos = current_tty.height - 1;
	}

	spinlock_unlock(&current_tty.lock);
}

void tty_write(const char* str) {
	while (*str != '\0') {
		tty_putchar(*str++);
	}
}

void tty_printf(char* fmt, ...) {
	char buf[4096];
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	tty_write(buf);
}

void tty_clear(void) {
	video_clear(current_tty.bg);
	current_tty.xpos = 0;
	current_tty.ypos = 0;
}

static void tty_reset_line(void) {
	current_tty.buf_pos = 0;
	memset(current_tty.buffer, 0, sizeof(current_tty.buffer));
}

static void tty_show_prompt(void) {
    tty_write("$ ");
}

static void cmd_help(void) {
	tty_write("available commands:\n");
	tty_write("	help 	- Show this help\n");
	tty_write("	clear 	- Clear the screen\n");
	tty_write("	echo 	- Print arguments\n");
}

static void cmd_clear(void) {
	tty_clear();
}

static void cmd_echo(const char* args) {
	tty_write(args);
	tty_putchar('\n');
}
