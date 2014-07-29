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

	return (*((int *)16));
}

int dup(int fildes)
{
	NSYSARGS = 3;
	*((int *)SYSARGBUF) = DUP;
	*((int *)(SYSARGBUF+sizeof(word_t))) = fildes;

	__asm__("sys 0");

	return (*((int *)16));
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

	return (*((int *)16));
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

	return (*((int *)16));
}

int read(int fd, void *buf, int count)
{
	*((int *)SYSARGBUF) = READ;
	NSYSARGS=1;
	*((int *)(SYSARGBUF+NSYSARGS*sizeof(word_t))) = fd; 
	++NSYSARGS;

	__asm__("sys 0");

	return (*((int *)16));
}
