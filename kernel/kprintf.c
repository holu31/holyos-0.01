#include "kernel/kprintf.h"
#include "kernel/serial.h"
#include "kernel/sprintf.h"
#include "stdarg.h"

void kprintf(char* fmt, ...) {
	char buf[KPRINTF_BUFSIZE];
	va_list args;
	int len;

	va_start(args, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	for (int i = 0; i < len; i++) {
		serial_write(buf[i]);
	}
}
