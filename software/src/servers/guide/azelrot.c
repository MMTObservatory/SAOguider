
#include "guide.h"

azelrot(guide, errors)
    Guide	*guide;
    double	*errors;
{
	double  (*xye)[2];
	double  (*xym)[2];
	double	coeffs[6];

	double	xz, yz, xs, ys, perp, orient;
	double	t;
	int	i;

    Malloc(xye, sizeof(double) * guide->nactiveboxes * 2);
    Malloc(xym, sizeof(double) * guide->nactiveboxes * 2);

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;

	double t = d2r(guide->rotang + guide->camera[cam].camangle);

	xye[i][0] = guide->camera[cam].radius * sin(t);
	xye[i][1] = guide->camera[cam].radius * cos(t);
	xym[i][0] = xye[i][0] - guide->box[box].cenaz;
	xym[i][1] = xye[i][1] - guide->box[box].cenel;
    }

    fitxy(guide->nactiveboxes, xye, xym, coeffs);
    Free(xye);
    Free(xym);

    slaDcmpf(coeffs, &xz, &yz, &xs, &ys, &perp, &orient);

    errors[0] =  xz;
    errors[1] =  yz;
    errors[2] =  orient * 180 / 3.14159;
}






#ifdef SVD
azelrot(guide, errors)
    Guide	*guide;
    double	*errors;
{
    	double	 rotator = guide->rotang;
    	int	nrow = guide->nactiveboxes * 2;
	int	ncol =                       3;

	double	*A, *W, *V, *Work, *CVM;
	double  *Y, *C;
	double  *design;
	double  *vector;
	int	i, j;

	Malloc(   A, nrow * ncol * sizeof(double));
	Malloc(   W,        ncol * sizeof(double));
	Malloc(   V, nrow * ncol * sizeof(double));
	Malloc(Work,        ncol * sizeof(double));
	Malloc( CVM, ncol * ncol * sizeof(double));

	Malloc(   Y, nrow * sizeof(double));
	Malloc(   C, ncol * sizeof(double));

	design = A;
	vector = Y;


    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;

	    double boxrot = (rotator + guide->camera[cam].rotation) / DEGSPERRAD;
	    double boxrad =            guide->camera[cam].radius;

	    *(design++) =  cos(boxrot);
	    *(design++) =  sin(boxrot);
	    *(design++) = -boxrad;

	    *(design++) = -sin(boxrot);
	    *(design++) =  cos(boxrot);
	    *(design++) =     0.0;

	    *(vector++) = guide->box[box].cenx;
	    *(vector++) = guide->box[box].ceny;
    }

    slaSvd(nrow, ncol, nrow, ncol, A, W, V, Work, &j);
    slaSvdcov(ncol, ncol, ncol, W, V, Work, CVM);
    slaSvdsol(nrow, ncol, nrow, ncol, Y, A, W, V, Work, C);

    errors[0] = C[0];
    errors[1] = C[1];
    errors[2] = C[2];
}
#endif
