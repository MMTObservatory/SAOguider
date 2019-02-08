
#include <stdio.h>

quickcenter(data, X, Y, W, CX, CY)
 	    double	*data;
 	    int		 X;
 	    int		 Y;
 	    int		 W;
 	    int		*CX;
 	    int		*CY;
{
	int	x, y;
	double max   = 0;
	int	w = W;
	int     i;
	double *prev;
	double *next; 

	double	sum;
	double  top;

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

		/* 
		 * Exclude the brightest pixel from each group of 3 to avoid single 
		 *  hot pixels 
		 */
		top=prev[-1];
		for ( i = -1; i <= 1; i++ ) {
		    top = (prev[i] > top) ? prev[i] : top;
		    top = (data[i] > top) ? data[i] : top;
		    top = (next[i] > top) ? next[i] : top;
		}
		sum -= top;

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
