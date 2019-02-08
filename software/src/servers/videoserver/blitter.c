/* blitter.c

 */

#include <stdio.h>


blitpix(dst, src, x, y, sw, dw)
	signed char	*dst;
        unsigned char   *src;
        int              x, y, sw, dw;
{
	int	i, j;

    for ( j = 0; j < y; j++ ) {
        for ( i = 0; i < x; i++ ) {
         dst[i] = src[i];
        }

	src  += sw;
	dst  += dw;
     }

     return 0;
}

blitwrd(dst, src, x, y, sw, dw)
	signed short	*dst;
        unsigned short  *src;
        int              x, y, sw, dw;
{
	int	i, j;

    for ( j = 0; j < y; j++ ) {
        for ( i = 0; i < x; i++ ) {
         dst[i] = src[i];
        }

	src  += sw;
	dst  += dw;
     }

     return 0;
}

procwrd(dst, src, bias, biasscale, sm, x, y, sw, dw)
	signed short	*dst;
        unsigned short  *src;
        unsigned short  *bias;
	double		 biasscale;
	int		 sm;
        int              x, y, sw, dw;
{
	int	i, j;
	int	check = 1;

    if ( sm ) {
	smooth(src, dst, x, y, sw, dw, bias, biasscale);
    } else {
	for ( j = 0; j < y; j++ ) {
	    for ( i = 0; i < x; i++ ) {
	     dst[i] = (src[i] - bias[i] * biasscale);
	    }

	    src  += sw;
	    bias += sw;
	    dst  += dw;
	 }
    }

     return 0;
}
