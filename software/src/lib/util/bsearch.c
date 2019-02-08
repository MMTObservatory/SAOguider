/* bsearch.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"

void *tabsearch(key, base, length, size, compare)
                void    *key;
                char    *base;
                int      length;
                int      size;
                int     (*compare)();
{
		int	lo = 0;
		int	hi = length;

		int	cut = length / 2;
		int	i;
 
        while ( hi - lo > 1 ) {
                if ( !(i = (*compare)(key, &base[size * cut])) )
		    return (void *) &base[size * cut];

		if ( i < 0 ) {
			hi = cut;
                        cut = (lo + hi) / 2;
		} else {
			lo = cut;
                        cut = (lo + hi) / 2;
                }
        }
 
        return (void *) &base[size * cut];
}

