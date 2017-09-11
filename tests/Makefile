.PHONY: all clean

all: sock hello rand sock_redirect_stdin freopen_stdin mmap_hook

sock: sock.c
hello: hello.c
rand: rand.c
sock_redirect_stdin: sock_redirect_stdin.c
mmap_hook: mmap_hook.c
freopen_stdin: freopen_stdin.c

clean:
	rm -f sock hello rand sock_redirect_stdin freopen_stdin mmap_hook
