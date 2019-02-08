
#include "guide.h"

void CalcBox(guide, box, imgsize, boxsize, data2d)
    Guide 	*guide;
    int	   	 box;
    int	 	 imgsize;
    int		 boxsize;
    double     **data2d;
{
    double	  pixsize;

    int img = guide->box[box].image-1;
    int cam = guide->image[img].camera-1;

	Moments(data2d[0], imgsize, boxsize, guide->backgroundwidth, &guide->box[box]);

	guide->box[box].cenx      *= guide->camera[cam].binning;
	guide->box[box].ceny      *= guide->camera[cam].binning;

	guide->box[box].cenx +=  (int)(guide->box[box].x+0.5)- guide->box[box].x - guide->box[box].xoff;
	guide->box[box].ceny +=  (int)(guide->box[box].y+0.5)- guide->box[box].y - guide->box[box].yoff;

	xytoazel(guide, box
		,  guide->box[box].cenx ,  guide->box[box].ceny
		, &guide->box[box].cenaz, &guide->box[box].cenel);

	pixsize = (guide->camera[cam].xpixelsize + guide->camera[cam].ypixelsize) / 2;

	guide->box[box].fwhm      *= guide->camera[cam].binning * pixsize;
	guide->box[box].sigmaxy   *= guide->camera[cam].binning * pixsize;
	guide->box[box].sigmafwhm *= guide->camera[cam].binning * pixsize;
}

void xyitoazel(guide, img, x, y, az, el)
    Guide 	*guide;
    int	   	 img;
    double	 x;
    double	 y;
    double	*az;
    double	*el;
{
	    int cam = guide->image[img].camera-1;

	double angle = (guide->rotang + guide->camera[cam].rotation) / DEGSPERRAD;

 	x *= guide->camera[cam].parity;

	*az =   x * guide->camera[cam].xpixelsize * cos(angle) 
             +  y * guide->camera[cam].ypixelsize * sin(angle);
	                       
	*el =  -x * guide->camera[cam].xpixelsize * sin(angle)
             +  y * guide->camera[cam].ypixelsize * cos(angle);
}

void xytoazel(guide, box, x, y, az, el)
    Guide 	*guide;
    int	   	 box;
    double	 x;
    double	 y;
    double	*az;
    double	*el;
{
	    int img = guide->box[box].image-1;
	xyitoazel(guide, img, x, y, az, el);
}

void azeltoxy(guide, box, az, el, x, y)
    Guide 	*guide;
    int	   	 box;
    double	 az;
    double	 el;
    double	*x;
    double	*y;
{
	    int img = guide->box[box].image-1;
	    int cam = guide->image[img].camera-1;

	 double angle = -(guide->rotang + guide->camera[cam].rotation) / DEGSPERRAD;
	*x = (  az * cos(angle) + el * sin(angle)) / guide->camera[cam].xpixelsize;
	*y = ( -az * sin(angle) + el * cos(angle)) / guide->camera[cam].ypixelsize;

	*x *= guide->camera[cam].parity;
}
