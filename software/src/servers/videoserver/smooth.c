
#include <stdio.h>

smoothbias(src, dst, X, Y, SW, DW, bias, biasscale)
 	    unsigned short	*src;
 	    unsigned short	*dst;
 	    int		 X;
 	    int		 Y;
 	    int		 SW;
 	    int		 DW;
	    unsigned short *bias;
	    double	    biasscale;
{
	int	x, y;
	int	sw, dw;
	unsigned short *prev;
	unsigned short *next; 
	double   tot = 4 * 0.5 + 4 * 0.37 + 1;

    sw = SW - X;
    dw = DW - X;

    prev  =  src;
     src += SW;
    bias += SW;
     dst += DW;
    next  =  src + SW;

    for ( y = 1; y < Y-1; y++ ) {
	prev++;
	 src++;
	bias++;
	 dst++;
	next++;
	for ( x = 1; x < X-1; x++ ) {
	    dst[0] = ( prev[-1] * 0.37 + prev[0] * 0.5 + prev[1] * 0.37
		     +  src[-1] * 0.5  +  src[0] * 1.0 +  src[1] * 0.5
		     + next[-1] * 0.37 + next[0] * 0.5 + next[1] * 0.37)
	       	/ tot - bias[0] * biasscale;

	    prev++;
	     src++;
	    bias++;
	     dst++;
	    next++;
	}
	 src++;
	 bias++;
	 src += sw;

	 dst++;
	 dst += dw;

	prev  =  src - SW;
	next  =  src + SW;
    }
}

smooth(src, dst, X, Y, SW, DW)
 	    unsigned short	*src;
 	    unsigned short	*dst;
 	    int		 X;
 	    int		 Y;
 	    int		 SW;
 	    int		 DW;
{
	int	x, y;
	int	sw, dw;
	unsigned short *prev;
	unsigned short *next; 
	double   tot = 4 * 0.5 + 4 * 0.37 + 1;

    sw = SW - X;
    dw = DW - X;

    prev  =  src;
     src += SW;
     dst += DW;
    next  =  src + SW;

    for ( y = 1; y < Y-1; y++ ) {
	prev++;
	 src++;
	 dst++;
	next++;
	for ( x = 1; x < X-1; x++ ) {
	    dst[0] = ( prev[-1] * 0.37 + prev[0] * 0.5 + prev[1] * 0.37
		     +  src[-1] * 0.5  +  src[0] * 1.0 +  src[1] * 0.5
		     + next[-1] * 0.37 + next[0] * 0.5 + next[1] * 0.37)
	       	/ tot;

	    prev++;
	     src++;
	     dst++;
	    next++;
	}
	 src++;
	 src += sw;

	 dst++;
	 dst += dw;

	prev  =  src - SW;
	next  =  src + SW;
    }
}
