#ifndef CPU_H
#define CPU_H

#include <stdint.h>

static inline void cpu_pause() {
	asm volatile("pause");
}

static inline uint32_t atomic_exchange(volatile uint32_t* ptr, uint32_t val) {
	uint32_t ret;
	asm volatile (
		"lock xchg %0, %1"
		: "=r" (ret), "+m" (*ptr)
		: "0" (val)
		: "memory"
	);
	return ret;
}

static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
	asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

static inline void cpu_enable_interrupts(void) {
	asm volatile("sti");
}

static inline void cpu_disable_interrupts(void) {
	asm volatile("cli");
}

#endif // CPU_H
