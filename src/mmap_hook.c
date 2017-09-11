#define _GNU_SOURCE

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

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE);  } while (0)

/*
static void handler(int sig, struct sigcontext ctx)
{
	preeny_debug("cr2 is 0x%lx.\n", ctx.cr2);	
	if(ctx.cr2 >= 0x23330000 && ctx.cr2 < 0x23331000) {
		preeny_debug("read or write access to addr 0x%lx. Exit.", ctx.cr2);	
		//preeny_debug("si_code: 0x%lx, si_errno: 0x%lx", si->si_code, si->si_errno);	
		exit(EXIT_FAILURE);
	} else {
		preeny_debug("SIGSEGV caused by other reasons. Exit.");
		exit(EXIT_FAILURE);
	}
}
*/

static void trap_handler(int sig, siginfo_t *si, void *ucontext)
{
	ucontext_t *ctx = ucontext;
	ctx->uc_mcontext.gregs[REG_EFL] &= ~X86_EFLAGS_TF; //clear trap flag.
	if(sig == SIGTRAP) {
		preeny_debug("trap_handler() set 0x2333000 page protection bit to none.\n");
		mprotect((void *)0x23330000, 0x1000, 0);
		return;
	}
}

static void handler(int sig, siginfo_t *si, void *ucontext)
{
	ucontext_t *ctx = ucontext;
	printf("reg_err is 0x%lld\n", ctx->uc_mcontext.gregs[REG_ERR]);
	if(si->si_addr >= (void *)0x23330000 && si->si_addr < (void *)0x23331000 ) {
		if(ctx->uc_mcontext.gregs[REG_ERR] & 0x2ull) {
			preeny_debug("write access to addr 0x%lx. Allow it\n", si->si_addr);	
			ctx->uc_mcontext.gregs[REG_EFL] |= X86_EFLAGS_TF;
			mprotect((void *)0x23330000, 0x1000, 3);
		} else {
			preeny_debug("read access to addr 0x%lx. Deny it and exit.\n", si->si_addr);	
			raise(SIGUSR1);
			//exit(EXIT_FAILURE);
		}
	} else {
		preeny_debug("SIGSEGV caused by other reasons. Exit.");
		exit(EXIT_FAILURE);
	}
}

void *(*original_mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
__attribute__((constructor)) void preeny_mmap_hook_orig()
{
	original_mmap = dlsym(RTLD_NEXT, "mmap");
	//注册 SIGSEGV 的信号处理函数
	preeny_debug("registering handler for SIGSEGV and SIGTRAP\n");
	struct sigaction sa;
	memset(&sa, 0, sizeof(sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NODEFER | SA_SIGINFO;	//TODO: SA_NODEFER到底是干嘛的
	sa.sa_sigaction = handler;
	if(sigaction(SIGSEGV, &sa, NULL) == -1) {
		handle_error("sigaction()");
	}

	memset(&sa, 0, sizeof(sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;	//TODO: SA_NODEFER到底是干嘛的
	sa.sa_sigaction = trap_handler;
	if(sigaction(SIGTRAP, &sa, NULL) == -1) {
		handle_error("sigaction()");
	}
	preeny_debug("successfully registered handler for SIGSEGV and SIGTRAP\n");
}

__attribute__((destructor)) void preeny_mmap_hook_destructor()
{
	signal(SIGSEGV, SIG_DFL);
	signal(SIGTRAP, SIG_DFL);
	preeny_debug("successfully unregister handler for SIGSEGV and SIGTRAP\n");
}
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	preeny_debug("intercepting mmap()\n");
	if(addr == (void *)0x23330000) {
		preeny_debug("address is 0x%08x, set the page protection bit to PROT_NONE\n", addr);
		return original_mmap(addr, length, PROT_NONE, flags, fd, offset);
	} else {
		return original_mmap(addr, length, prot, flags, fd, offset);
	}
}


