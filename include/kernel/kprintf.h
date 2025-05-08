#ifndef KPRINTF_H
#define KPRINTF_H

#ifndef LOG_LEVEL
	#define LOG_LEVEL 2	// 0=min, 1=normal, 2=debug, 3=trace
#endif

#define ANSI_RED	"\x1b[31m"
#define ANSI_GREEN	"\x1b[32m"
#define ANSI_YELLOW	"\x1b[33m"
#define ANSI_BLUE	"\x1b[34m"
#define ANSI_RESET	"\x1b[0m"

#define KERN_OK		"[" ANSI_GREEN "DONE" ANSI_RESET "] "
#define KERN_ERR	"[" ANSI_RED "ERRO" ANSI_RESET "] "
#define KERN_INFO	"[" ANSI_BLUE "INFO" ANSI_RESET "] "
#define KERN_DEBUG	"[" ANSI_YELLOW "DBUG" ANSI_RESET "] "
#define KERN_TRACE	"[" ANSI_YELLOW "TRAC" ANSI_RESET "] "

#define KPRINTF_BUFSIZE	4096

#if LOG_LEVEL >= 1
	#define log_ok(fmt, ...)    kprintf(KERN_OK fmt, ##__VA_ARGS__)
	#define log_err(fmt, ...)   kprintf(KERN_ERR fmt, ##__VA_ARGS__)
	#define log_info(fmt, ...)  kprintf(KERN_INFO fmt, ##__VA_ARGS__)
#else
	#define log_ok(fmt, ...)
	#define log_err(fmt, ...)
	#define log_info(fmt, ...)
#endif

#if LOG_LEVEL >= 2
	#define log_debug(fmt, ...) kprintf(KERN_DEBUG fmt, ##__VA_ARGS__)
#else
	#define log_debug(fmt, ...)
#endif

#if LOG_LEVEL >= 3
	#define log_trace(fmt, ...) kprintf(KERN_TRACE fmt, ##__VA_ARGS__)
#else
	#define log_trace(fmt, ...)
#endif

void kprintf(char* fmt, ...);

#endif // KPRINTF_H
