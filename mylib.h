void *memcpy(void *dest, void *src, int count)
{
  int ibyte,tmp;
  ibyte=0;
  while(ibyte<count)
  {
  	tmp = (*((char *)(src+ibyte+3))<<24) + 
  		  (*((char *)(src+ibyte+2))<<16) +
  		  (*((char *)(src+ibyte+1))<<8) +
  		  (*((char *)(src+ibyte)));
  	*((int *)(dest+ibyte)) = tmp;
    ibyte=ibyte+4;
  }

  return dest;
}