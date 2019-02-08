
#include <stdio.h>

quickcenter(data, X, Y, W, CX, CY)
 	    unsigned short	*data;
 	    int		 X;
 	    int		 Y;
 	    int		 W;
 	    int		*CX;
 	    int		*CY;
{
	int	x, y;
	int max   = 0;
	int	w = W;
	unsigned short *prev;
	unsigned short *next; 

	int	sum;

	w = W - X;

	prev  = data;
	data += W;
	next  = data + W;
 
	for ( y = 1; y < Y-1; y++ ) {
	    prev++;
	    data++;
	    next++;
	    for ( x = 1; x < X-1; x++ ) {
		sum = prev[-1] + prev[0] + prev[1]
		    + data[-1] + data[0] + data[1]
		    + next[-1] + next[0] + next[1];

		if ( sum > max ) {
		    max = sum;
		    *CX = x;
		    *CY = y;
		}
		prev++;
		data++;
		next++;
	    }
	    data++;
	    data += w;
	    prev  = data - W;
	    next  = data + W;
	}
}
