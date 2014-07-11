void exit(int status)
{
	NSYSARGS = 16;
	*((int *)SYSARGBUF) = EXIT;
	*((int *)(SYSARGBUF+4)) = status;
	*((int *)(SYSARGBUF+2*4)) = 555;

	__asm__("sys 0");
}

int time(int *t)
{
	NSYSARGS = 16;
	*((int *)SYSARGBUF) = TIME;
	*((int *)SYSARGBUF+4) = t;

	__asm__("sys 0");

	return (*((int *)16));
}

int dup(int fildes)
{
	NSYSARGS = 8;
	*((int *)SYSARGBUF) = DUP;
	*((int *)(SYSARGBUF+4)) = fildes;
	*((int *)(SYSARGBUF+2*4)) = 333;

	__asm__("sys 0");

	return (*((int *)16));
}

void putchar(char c)
{
	*((int *)SYSARGBUF) = PUTCHAR;
	*((int *)(SYSARGBUF+4)) = c;

	__asm__("sys 0");
}

int getchar()
{
	*((int *)SYSARGBUF) = GETCHAR;

	__asm__("sys 0");

	return (*((int *)16));
}