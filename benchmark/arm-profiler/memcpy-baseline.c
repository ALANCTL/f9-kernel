int memcpy_baseline(char *dest, const char *src, int len)
{
  int i;
  while (len--) {
	*dest++ = *src++;
	//for (i=100000000; i > 0; i--)
	  ; /* nop */
  }	 
}
