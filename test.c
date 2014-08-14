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
    dup(fd);
   int buf[7];
   buf[0]=(int)'a';
   buf[1]=(int)' ';
   buf[2]=(int)'t';
   buf[3]=(int)'e';
   buf[4]=(int)'s';
   buf[5]=(int)'t';
   buf[6]=(int)'\0';
   write(fd,7,buf);
 
   close(fd);

    exit(0);
  }
  else
    exit(0);
}

#include "mog_syscall.c"
