#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define BUF_SIZE 5
int main()
{
        int j;
        char buf[BUF_SIZE];
        for(j = 0; j < BUF_SIZE; j += 1) {
                buf[j] = 'a' + j;
        }

        int fd_in = open("sock_recv.txt", O_RDWR | O_CREAT);
        if (fd_in < 0) {
		perror("open file error.");
		exit(1);
        }

        write(fd_in, buf, BUF_SIZE);
        FILE *f = fdopen(fd_in, "r+");
        rewind(f);
        dup2(fd_in, 0);
	close(fd_in);

	int s = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1024];
	FILE *zomg = fdopen(s, "r+");

	assert(send(s, "HI!\n", 4, 0) == 4);
	assert(recv(s, buffer, 1024, 0) == 5);
	printf("You wrote: %s\n", buffer);
	//fflush(zomg);
}
