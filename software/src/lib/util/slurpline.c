/* SlurpLine.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"
#include "xfile.h"


char *SlurpLine(file, line, n, max)
	File	file;
	char	*line;
	int	*n;
	int	*max;
{
	*n = 0;

	if ( line == NULL ) Malloc(line, *max);

	while ( 1 ) {
		line[*n] = GetC(file);

		if ( line[*n] == '\n' ) break;
		if ( line[*n] == EOF  ) break;

		*n += 1;
		if ( *n >= *max ) {
			*max += 128;
			ReAlloc(line, *max);
		}
	}

	if ( *n >= *max ) {
		*max += 128;
		ReAlloc(line, *max);
	}
	line[*n+1] = '\0';

	return line;
}


