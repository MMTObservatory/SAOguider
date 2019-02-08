/* bswap.c
 */

bswap(f, t, n)
	char *f, *t;
	int    n;
{
	n /= 2;

	while ( n-- ) {
		char b;

	    b = *f; *t = *(f+1); *(t+1) = b;
	    f += 2;
	    t += 2;
	}
}


