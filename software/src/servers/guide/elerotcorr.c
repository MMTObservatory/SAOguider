
#include "guide.h"

/* This file impliments a guide correction based on the 
   telescope elevation and rotation.  It moves the guide boxes
   to account for instrument flexure.
*/


void el_corr(s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
	int	i;

	for ( i = 1; i <= argc; i++ ) {
	    guide->elcoeff[i-1] = atof(argv[i]);
	}
}

void az_corr(s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
	int	i;

	for ( i = 1; i <= argc; i++ ) {
	    guide->azcoeff[i-1] = atof(argv[i]);
	}
}

void flex(s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
	int	i;

	guide->flex = 1;

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;

	guide->x_zero[box] = guide->box[box].x;
	guide->y_zero[box] = guide->box[box].y;
    }
}

void elerot_err(guide)
	Guide	*guide;
{
	int	i;

	double	az;
	double	el;
	double	x;
	double	y;

	double	*elcoeff = guide->elcoeff;
	double	*azcoeff = guide->azcoeff;

	double	sinrot = sin(guide->rotang);
	double	cosele = cos(guide->eleang);
	
	el = cosele * ( elcoeff[0] + elcoeff[1] * sinrot + elcoeff[2] * sinrot * sinrot );
	az = cosele * ( azcoeff[0] + azcoeff[1] * sinrot + azcoeff[2] * sinrot * sinrot );

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;

	azeltoxy(guide, box, az, el, &x, &y);

	guide->box[box].x = guide->x_zero[box] + x;
	guide->box[box].y = guide->y_zero[box] + y;

	if ( Abs(guide->box[box].x - (guide->x_zero[box] + x)) > 1 
	  || Abs(guide->box[box].y - (guide->y_zero[box] + y)) > 1 ) {
		int img = guide->box[box].image-1;

	    guide->image[img].dirty = 1;
	}
    }
}

