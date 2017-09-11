#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>


int main()
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	char buf[1024];
	//FILE *zomg = fdopen(s, "r+");

	sleep(1);
	assert(send(s, "HI!\n", 4, 0) == 4);
	assert(recv(s, buf, 1024, 0) == 5);
	assert(send(s, "HHI!\n", 5, 0) == 5);
	fprintf(stderr, "You wrote: %s\n", buf);
	//fflush(zomg);
	assert(send(s, "HHI!\n", 5, 0) == 5);
	//close(s);
	//while(1);
}
