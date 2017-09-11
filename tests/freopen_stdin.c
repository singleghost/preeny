#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 5
int main()
{
	int r;
	FILE *p;
	p = freopen("test.txt", "w+", stdin);
	if(p == NULL) {
		perror("freopen error.");	
	}
        int j;
        char buf[BUF_SIZE];
        for(j = 0; j < BUF_SIZE; j += 1) {
                buf[j] = 'a' + j;
        }

	int nb;
	nb = fwrite(buf, 1, BUF_SIZE, stdin);
	if(nb != BUF_SIZE) {
		perror("fwrite error.");	
	}
	fflush(stdin);
	
	r = fseek(stdin, 0, SEEK_SET);
	if(r != 0) {
		  perror("fseek()");
		  fprintf(stderr,"fseek() failed in file %s at line # %d\n", __FILE__,__LINE__-5);
		  exit(EXIT_FAILURE);
	}
	char buffer[1024];
	memset(buffer, 0, 1024);
	fread(buffer, 1, BUF_SIZE, stdin);
	printf("buffer:%s", buffer);
	
	/*
        int fd_in = open("sock_recv.txt", O_RDWR | O_CREAT);
        if (fd_in < 0) {
		perror("open file error.");
		exit(1);
        }
        write(fd_in, buf, BUF_SIZE);
        FILE *f = fdopen(fd_in, "r+");
        rewind(f);
        dup2(fd_in, 0);

	int s = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1024];
	FILE *zomg = fdopen(s, "r+");

	assert(send(s, "HI!\n", 4, 0) == 4);
	assert(recv(s, buffer, 1024, 0) == 5);
	fprintf(zomg, "You wrote: %s\n", buffer);
	fflush(zomg);
	*/
}
