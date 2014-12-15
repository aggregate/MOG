#define SYSARGBUF 16

#include "mylib.h"

/* syscall macros mapping syscall names to numbers representing available syscalls*/
#define EXIT 0
#define TIME 1
#define DUP 2
#define PUTCHAR 3
#define GETCHAR 4
#define OPEN 5
#define CLOSE 6
#define READ 7
#define WRITE 8
#define PUTINT 9

typedef union word_union {
        float                   f;
        int                     i;
        unsigned int            u;
        short                   h[2];
        signed char             b[4];
} word_t;

//byte addressing?
int NSYSARGS;
int IPROC;
int NPROC;

void exit(int status);
int time(int *t);
int dup(int fildes);
void putchar(char c);
void putint(int i); //for testing purposes
int getchar();
int open(const char *pathname, int flags, int mode);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, const void *buf, int count); 
