#include "mog_syscall.h"

int MAXFAC = 9;
int MAXITER = 3;

int
fact(int i)
{
	if (i < 2) return(1);
	return(i * fact(i-1));
}

int main()
{
	//do some random operations on the first few PEs 
	//plus a syscall to view results
  if(IPROC==3)
  {
		char achar;
		achar = 'b';
		achar = achar - 1;
		putchar(achar);
  }
  else if(IPROC==3)
	{
		char achar;
		achar = 'a';
		achar = achar + 1;
		putchar(achar);
	}
	else if(IPROC==4)
	{
		char achar;
		int anint;
		achar = 'a';
		achar = achar + 5*2;
		achar = achar - 8;
		putchar(achar);
	}
	else if(IPROC==5)
	{
		int anint;
		anint = (42+3*2)%7;
		putint(anint);
	}
	else
	{//do some recursion(still MIMD) on remaining PEs
		int tmp;
		int k;
		k = 0;
		while (k < MAXITER) {
			tmp = fact((k ^ IPROC) % MAXFAC);
			k = k + 1;
		}
		putint(tmp);
	}
	exit(0);
}

#include "mog_syscall.c"
