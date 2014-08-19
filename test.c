#include "mog_syscall.h"

int main()
{
//  if(IPROC==20)
//  {
//    dup(IPROC);
//    exit(0);
//  }
//  else
//    exit(0);

  if(IPROC==2)
  {
    char pathname[6];
    pathname[0]='a';
    pathname[1]='f';
    pathname[2]='i';
    pathname[3]='l';
    pathname[4]='e';
    pathname[5]='\0'; 
  
    int fd;
    fd=open(pathname,2);
    char buf[7];
    buf[0]='a';
    buf[1]=' ';
    buf[2]='t';
    buf[3]='e';
    buf[4]='s';
    buf[5]='t';
    buf[6]='\0';

   dup(fd);
   write(fd,buf,7);
   close(fd);

   fd=open(pathname,2);
   char _buf[7];
   read(fd,_buf,7);
   close(fd);

   char _pathname[6];
	_pathname[0]='b';
	_pathname[1]='f';
	_pathname[2]='i';
	_pathname[3]='l';
	_pathname[4]='e';
	_pathname[5]='\0';
	fd=open(_pathname,2);
	write(fd,_buf,7);
	close(fd);
   exit(0);
  }
  else
    exit(0);
}

#include "mog_syscall.c"
