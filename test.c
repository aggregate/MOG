#include "mog_syscall.h"

int
fact(int i)
{
	if (i < 2) return(1);
	return(i * fact(i-1));
}

int main()
{

  if(IPROC==2)
  {
  	char pathname[] = "afile";

    int fd=open(pathname,2);
    char buf[] = "a test";

   	dup(fd);
   	write(fd,buf,7);
   	close(fd);

	  fd=open(pathname,2);
	  char _buf[7];
	  read(fd,_buf,7);
	  close(fd);
	
	  char _pathname[] = "bfile";
		fd=open(_pathname,2);
		write(fd,_buf,7);
		close(fd);
  	exit(0);
  }
  else
    exit(0);
}

#include "mog_syscall.c"
