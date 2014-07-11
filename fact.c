int IPROC;
int ten = 3;

float a = 42;

int
fact(int i)
{
	if (i < 2) return(1);
	return(i * fact(i-1));
}

int
main()
{
	int i;

	i = 0;
	while (i < 2) {
		a = fact((i ^ IPROC) % ten);
		i = i + 1;
	}
}
