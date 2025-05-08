#include "kernel/string.h"

int memcmp(const void* aptr, const void* bptr, size_t size) {
	const unsigned char* a = aptr, *b = bptr;
	while (size--) {
		if (*a != *b)
			return (*a < *b) ? -1 : 1;
		a++, b++;
	}

	return 0;
}

void* memcpy(void* restrict destptr, const void* restrict srcptr, size_t size) {
	char* dest = destptr;
	const char* src = srcptr;
	while(size--)
		*dest++ = *src++;

	return destptr;
}

void* memset(void* ptr, int value, size_t size) {
	char* buf = ptr;
	while (size--)
		*buf++ = value;

	return ptr;
}

void* memset16(void* ptr, uint16_t value, size_t size) {
	uint16_t* buf = ptr;
	while (size--)
		*buf++ = value;

	return ptr;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (*str++)
		len++;

	return len;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}

	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	if (n == 0) return 0;

	while (--n && *s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}

	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

size_t itoa(int32_t n, char* buf) {
	int i, sign;
	
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		buf[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	
	if (sign < 0)
		buf[i++] = '-';
	
	buf[i] = '\0';
	reverse(buf);

	return i;
}

size_t itoh(int32_t n, char* buf) {
	const char hex_chars[] = "0123456789ABCDEF";
	uint32_t i, d = 0x10000000;
	char* ptr = buf;

	while ((n / d == 0) && (d >= 0x10))
		d /= 0x10;
	i = n;

	while (d >= 0xF) {
		*ptr++ = hex_chars[i / d];
		i = i % d;
		d /= 0x10;
	}

	*ptr++ = hex_chars[i];
	*ptr = 0;

	return strlen(buf);
}

int atoi(const char s[]) {
	int val = 0, sign = 1;
	
	if (*s == '-') {
		sign = -1;
		s++;
	}
	
	while (*s >= '0' && *s <= '9') {
		val = val * 10 + (*s - '0');
		s++;
	}
	
	return sign * val;
}

void reverse(char s[]) {
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
