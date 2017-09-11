#define _GNU_SOURCE

#include <stdarg.h>
#include <asm/processor-flags.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <poll.h>
#include <linux/memfd.h>
#include <sys/syscall.h>
#include "logging.h"

#define LEAK_ADDR 0x23330000
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE);  } while (0)


int (*original_puts)(const char *str);
int (*original_fputs)(const char *str, FILE *stream);
int (*original_printf)(const char *format, ...);
int (*original_fprintf)(FILE *stream, const char *format, ...);
ssize_t (*original_write)(int fd, const void *buf, size_t count);
size_t (*original_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);

__attribute__((constructor)) void preeny_mmap_hook_orig()
{
	original_puts = dlsym(RTLD_NEXT, "puts");
	original_fputs = dlsym(RTLD_NEXT, "fputs");
	original_printf = dlsym(RTLD_NEXT, "printf");
	original_fprintf = dlsym(RTLD_NEXT, "fprintf");
	original_write = dlsym(RTLD_NEXT, "write");
	original_fwrite = dlsym(RTLD_NEXT, "fwrite");
}

__attribute__((destructor)) void preeny_mmap_hook_destructor()
{
}

int puts(const char *str)
{
	if(str == (char *)LEAK_ADDR) raise(SIGSEGV);
	else original_puts(str);
}

int fputs(const char *str, FILE *stream)
{
	if(str == (char *)LEAK_ADDR) raise(SIGSEGV);
	else original_fputs(str, stream);
}
int printf(const char *format, ...)
{
	va_list ap;
	void *p;
	int ret;

	va_start(ap, format);/* 从可变参数的第一个参数开始遍历 */
	p = va_arg(ap, void *);/* 得到第一个参数 */
	va_end(ap);  /* 参数处理结束，做一些清理工作 */
	if(p == (void *)LEAK_ADDR) raise(SIGSEGV);
	else {
		va_start(ap, format);
		ret = vprintf(format, ap);
		va_end(ap);
		return ret;
	}
}

int fprintf(FILE *stream, const char *format, ...)
{
	va_list ap;
	void *p;
	int ret;

	va_start(ap, format);/* 从可变参数的第一个参数开始遍历 */
	p = va_arg(ap, void *);/* 得到第一个参数 */
	va_end(ap);  /* 参数处理结束，做一些清理工作 */
	if(p == (void *)LEAK_ADDR) raise(SIGSEGV);
	else {
		va_start(ap, format);
		ret = vfprintf(stream, format, ap);
		va_end(ap);
		return ret;
	}
}

ssize_t write(int fd, const void *buf, size_t count)
{
	if(buf == (void *)LEAK_ADDR) raise(SIGSEGV);
	else return original_write(fd, buf, count);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	if(ptr == (void *)LEAK_ADDR) raise(SIGSEGV);
	else return original_fwrite(ptr, size, nmemb, stream);
}

//TODO: 
