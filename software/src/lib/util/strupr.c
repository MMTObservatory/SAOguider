/* strupr.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include <ctype.h>

char *strupr(str)
	char	*str;
{
	char *s = str;

	while ( *s ) {
	    *s = toupper(*s);
	     s++;
	}

	return str;
}

char *strlwr(str)
	char	*str;
{
	char *s = str;

	while ( *s ) {
	    *s = tolower(*s);
	     s++;
	}

	return str;
}
