#define SYSARGBUF 16

/* syscall macros mapping syscall names to numbers representing available syscalls*/
#define EXIT 0
#define TIME 1
#define DUP 2
#define PUTCHAR 3
#define GETCHAR 4
#define OPEN 5
#define READ 6

typedef union word_union {
        float                   f;
        int                     i;
        unsigned int            u;
        short                   h[2];
        signed char             b[4];
} word_t;

//byte addressing?
int NSYSARGS;

void exit(int status);
int time(int *t);
int dup(int fildes);
void putchar(char c);
int getchar();
int open(const char *pathname, int flags);
int read(int fd, void *buf, int count);
