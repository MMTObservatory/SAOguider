/* A solid body fit without scale
 */

#include <math.h>

fitxy(nrow, xye, xym, coeffs)
	int	nrow;
	double	(*xye)[2], (*xym)[2];
	double  coeffs[6];
{
    	int	i;

	double xx  = 0.0;
	double xy  = 0.0;
	double yy  = 0.0;
	double yx  = 0.0;

	double aex  = 0.0;
	double aey  = 0.0;
	double amy = 0.0;
	double amx = 0.0;

	double	th, dx, dy;

	for ( i = 0; i < nrow; i++ ) {
	    xx += xye[i][0] * xym[i][0];
	    xy += xye[i][0] * xym[i][1];
	    yy += xye[i][1] * xym[i][1];
	    yx += xye[i][1] * xym[i][0];

	    aex += xye[i][0];
	    aey += xye[i][1];
	    amx += xym[i][0];
	    amy += xym[i][1];
	}

	xx /= nrow;
	xy /= nrow;
	yy /= nrow;
	yx /= nrow;

	aex /= nrow;
	aey /= nrow;
	amx /= nrow;
	amy /= nrow;

	th = atan2(xy - amy * aex - yx + amx * aey
		 , xx - amx * aex + yy - aey * amy);

	dx = aex - cos(th) * amx - sin(th) * amy;
	dy = aey + sin(th) * amx - cos(th) * amy;

	coeffs[0] =  dx;
	coeffs[1] =  cos(th);
	coeffs[2] =  sin(th);
	coeffs[3] =  dy;
	coeffs[4] = -sin(th);
	coeffs[5] =  cos(th);
}


avgxy(nrow, xye, xym, coeffs)
	int	nrow;
	double	(*xye)[2], (*xym)[2];
	double  coeffs[6];
{
    	int	i;

	double aex  = 0.0;
	double aey  = 0.0;

	for ( i = 0; i < nrow; i++ ) {
	    aex += xye[i][0] - xym[i][0];
	    aey += xye[i][1] - xym[i][1];
	}

	coeffs[0] =  aex / nrow;
	coeffs[1] =  1.0;
	coeffs[2] =  0.0;
	coeffs[3] =  aey / nrow;
	coeffs[4] =  0.0;
	coeffs[5] =  1.0;
}

