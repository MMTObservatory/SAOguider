/* process__c.c

 */

#include <stdio.h>


process_add(pixel, buffr, accum, x, y, w)
        unsigned char   *pixel;
	unsigned char	*buffr;
        unsigned short  *accum;
        int              x, y, w;
{
	int	i, j;
	int	v;

    for ( j = 0; j < y; j++ ) {
        for ( i = 0; i < x; i++ ) {
	 *accum = ( *accum - *buffr ) + *pixel;
	 *buffr = *pixel; 

	    accum++;
	    buffr++;
	    pixel++;
        }

	pixel += w-x;
	accum += w-x;
     }

     return 0;
}

processxadd(pixel, buffr, accum, x, y, w)
        unsigned char   *pixel;
	unsigned char	*buffr;
        unsigned short  *accum;
        int              x, y, w;
{
	int	i, j;
	int	v;

    for ( j = 0; j < y; j++ ) {
        for ( i = 0; i < x; i++ ) {
	 *accum = ( *accum - *buffr ) + *pixel;
	 *buffr = *pixel; 

	    accum++;
	    buffr++;
	    pixel++;
        }

	pixel += w-x;
	accum += w-x;
     }

     return 0;
}
