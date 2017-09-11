#include<sys/mman.h>
#include<stdio.h>
#include<string.h>
int main() {
	char *p = (char *)0x23330000;
	int i;
	mmap((void *)0x23330000, 0x1000, 0, 34, 0, 0);
	strcpy(p, "hello \n");
	memcpy(p, "hello", 5);
	for(i = 0; i<3; i++) *p++ = 'c';
	scanf("%s", p);

}
