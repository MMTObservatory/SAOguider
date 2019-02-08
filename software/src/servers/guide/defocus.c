#include <malloc.h>
#include <math.h>
#include "guide.h"

/* 
 * Background -- 
 *     Determine the median value in an image 
 *
 */
void
Background( double *image, int xsize, int ysize, int boxsize, int backgroundwidth, int cenx, int ceny, 
	   double *bkg, double *sigma) 
{
    double *pixlist;
    int i,j;
    int count = 0;
    double dist;
    int source_radius = boxsize/2;


    if (backgroundwidth == 0)  {
	*bkg = *sigma = 0.0;
	return;
    }

    Malloc (pixlist,  xsize * ysize * sizeof(double));
    
    for (i=backgroundwidth; i<ysize-backgroundwidth; i++) {
	for (j=backgroundwidth; j<xsize-backgroundwidth; j++) {
	    dist = sqrt(pow(cenx-j, 2) + pow(ceny-i, 2));
	    if (dist > source_radius) {
		pixlist[count++] = image[i*xsize+j]; 
	    }
	}
    }

    ClippedBackground(pixlist, count, 5, bkg, sigma);
    Free(pixlist);
}

/* Moments --
 *	Determine the FWHM estimate using moments
 *      Determine centroids
 */
void Moments(double *image, int imgsize, int boxsize, int backgroundwidth, Box *box)

{
    double  sum    = 0.0;
    double  sumx   = 0.0;
    double  sumxx  = 0.0;
    double  sumy   = 0.0;
    double  sumyy  = 0.0;
    double  val;
    double  rmom;
    int     boxpix = 0;

    int source_radius = boxsize/2;
    int source_area   = source_radius*source_radius*M_PI;

    int prevcenx = 0;
    int prevceny = 0;
    int cenx;
    int ceny;

    double dist;

    int	i, j;
    int iter = 5;

    quickcenter(image + imgsize*backgroundwidth + backgroundwidth
		, imgsize-backgroundwidth*2
		, imgsize-backgroundwidth*2, imgsize, &cenx, &ceny);
    cenx+=backgroundwidth;
    ceny+=backgroundwidth;

    Background(image, imgsize, imgsize, boxsize, backgroundwidth, cenx, ceny, 
				 &box->background, &box->noise);

	
 while ( iter-- && prevcenx != cenx && prevceny != ceny ) { 
	sum    = 0.0;
	sumx   = 0.0;
	sumy   = 0.0;
	sumxx  = 0.0;
	sumyy  = 0.0;
	boxpix = 0;

    for (i=backgroundwidth; i<imgsize-backgroundwidth; i++) {
	for (j=backgroundwidth; j<imgsize-backgroundwidth; j++) {
	  
	  dist = sqrt(pow(cenx-j, 2) + pow(ceny-i, 2));
	  if (dist <= source_radius) {
	    val   =  image[i*imgsize+j] - box->background;
	    sum   += val;
	    sumx  += val*j;
	    sumxx += val*j*j;
	    sumy  += val*i;
	    sumyy += val*i*i;
	    boxpix++;
	  }
	}
    }
     
    if ( sum <= 0.0 ) {
	box->sigmaxy = -1.0;
	box->sigmafwhm = -1.0;
	box->cenx = 0.0;
	box->ceny = 0.0;
	box->fwhm = -1.0;
    } else {

	rmom = ( sumxx - sumx * sumx / sum + sumyy - sumy * sumy / sum ) / sum;

	if ( rmom <= 0 ) {
	    box->fwhm = -1.0;
	} else {
	    box->fwhm = sqrt(rmom)  * 2.354 / sqrt(2.0);
	}

	box->counts = sum;
	box->cenx   = sumx / sum - imgsize/2;
	box->ceny   = sumy / sum - imgsize/2;
	
	box->sigmaxy= box->noise * source_area / box->counts / sqrt(6.0);

	box->sigmafwhm = box->noise * pow(source_area,1.5) / 10. 
	                            / box->fwhm / box->counts 
				    * 2.354 * 2.354 / 2.0;
    }
    cenx     = box->cenx;
    ceny     = box->ceny;
    prevcenx = cenx;
    prevceny = ceny;
  }
}

select_box(image, x, y, nx, ny, box)
	double	**image;
	int	  x;
	int	  y;
	int	  nx;
	int	  ny;
	double	**box;
{
		int	i, j;
		int	yi, xi;

	for ( j = 0; j < ny; j++ ) {
	    for ( i = 0; i < nx; i++ ) {

		yi = y - ((ny-j) - ny/2);
		xi = x - ((nx-i) - nx/2);

		box[j][i] = image[yi][xi];
	    }
	}
}
