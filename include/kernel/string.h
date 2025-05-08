#ifndef STRING_H
#define STRING_H

#include "stddef.h"
#include "stdint.h"

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memset(void*, int, size_t);
void* memset16(void*, uint16_t, size_t);
size_t strlen(const char*);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t itoa(int32_t n, char* buf);
size_t itoh(int32_t n, char* buf);
int atoi(const char s[]);
void reverse(char s[]);

#endif // STRING_H
