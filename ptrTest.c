#include <stdlib.h>

int main()
{
  char myArray[10][10];
  int testInt = *((int*)(myArray[3]));

  return 0;
}
