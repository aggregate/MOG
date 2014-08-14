void exit(int status)
{
	NSYSARGS = 3;
	*((int *)SYSARGBUF) = EXIT;
	*((int *)(SYSARGBUF+sizeof(word_t))) = status;

	__asm__("sys 0");
}

int time(int *t)
{
	NSYSARGS = 3;
	*((int *)SYSARGBUF) = TIME;
	*((int *)SYSARGBUF+sizeof(word_t)) = (int)t;

	__asm__("sys 0");

	return (*((int *)SYSARGBUF));
}

int dup(int fildes)
{
	NSYSARGS = 3;
	*((int *)SYSARGBUF) = DUP;
	*((int *)(SYSARGBUF+sizeof(word_t))) = fildes;

	__asm__("sys 0");

	return (*((int *)SYSARGBUF));
}

void putchar(char c)
{
	NSYSARGS = 2;
	*((int *)SYSARGBUF) = PUTCHAR;
	*((int *)(SYSARGBUF+sizeof(word_t))) = (int)c;

	__asm__("sys 0");
}

int getchar()
{
	NSYSARGS = 2;
	*((int *)SYSARGBUF) = GETCHAR;

	__asm__("sys 0");

	return (*((int *)SYSARGBUF));
}

int open(const char *pathname, int flags)
{
	*((int *)SYSARGBUF) = OPEN;

	//for(NSYSARGS=1; pathname[NSYSARGS]!='\0'; ++NSYSARGS)
	//	*((int *)SYSARGBUF+(NSYSARGS)*4) = (int)pathname[NSYSARGS-1];
	NSYSARGS=1;
	while(pathname[NSYSARGS-1]!='\0')
	{
		*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = (int)pathname[NSYSARGS-1];
		++NSYSARGS;
	}

	*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = (int)'\0'; 
	++NSYSARGS;
	*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = flags;
	++NSYSARGS;

	__asm__("sys 0");

	return (*((int *)SYSARGBUF));
}

int close(int fd)
{
        *((int *)SYSARGBUF) = READ;
        NSYSARGS=1;
	*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = fd; 
	++NSYSARGS;
        return 1;
}

int read(int fd, int count, void *buf)
{
    *((int *)SYSARGBUF) = READ;
    NSYSARGS=1;
    *((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = fd; 
    ++NSYSARGS;
    *((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = count; 
    ++NSYSARGS;

    __asm__("sys 0");

    int i=0;
    while(i<count)
      *((int *)(buf+i*sizeof(int)))=*((int *)(SYSARGBUF+(i+1)*sizeof(word_t)));

    return 1;
}

int write(int fd, int count, const void *buf) 
{
	*((int *)SYSARGBUF) = WRITE;
	NSYSARGS=1;
    *((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = fd;
    ++NSYSARGS;
    *((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = count;
    ++NSYSARGS;

	while(NSYSARGS<(count+3))
	{
		*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = *((int *)(buf+(NSYSARGS-3)*sizeof(int)));
		++NSYSARGS;
	}

	__asm__("sys 0");

	return (*((int *)SYSARGBUF));
}
