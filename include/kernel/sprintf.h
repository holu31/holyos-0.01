#ifndef SPRINTF_H
#define SPRINTF_H

#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"

int vsprintf(char* buf, const char* fmt, va_list args);
int sprintf(char* buf, const char* fmt, ...);
int vsnprintf(char* buf, size_t size, const char* fmt, va_list args);
int snprintf(char* buf, size_t size, const char* fmt, ...);

#endif // SPRINTF_H
