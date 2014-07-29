#include "mog_syscall.h"

int IPROC;
int a,b;

int main()
{
	char pathname[5];
	pathname[0]='a';
	pathname[1]='b';
	pathname[2]='c';
	pathname[3]='\0';
	open(pathname,3);
	exit(0);
}

#include "mog_syscall.c"
