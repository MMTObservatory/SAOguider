
#include <stdio.h>

center__c(data, X, Y, W, mask, R, rBk, rCn, CX, CY, Bk, Sum, Xw, Yw, XXw, YYw, FWHM, Bksum, Min, Max
		, bias, biasscale, rawsum, biassum)
 	    unsigned short	*data;
 	    int		 X;
 	    int		 Y;
 	    int		 W;
 	    unsigned char *mask;
	    int		 R;
	    int		 rBk;
	    int		 rCn;
 	    double	*CX;
 	    double	*CY;
 	    double	 Bk;
 	    double	*Sum;
 	    double	*Xw, *Yw;
 	    double	*XXw, *YYw;
 	    double	*FWHM;
 	    int		*Bksum;
 	    int		*Min;
 	    int		*Max;
	    unsigned short *bias;
	    double	biasscale;
	    int		*rawsum;
	    int		*biassum;
{
	int	Data;
	double	sum, xw, yw, xxw, yyw;
	int	min, max, bksum;

	int bkpix = 0;
	int nipix = 0;

	int	x, y;

	W -= X;
 
	sum   = 0.0;
	xw    = 0.0;
	yw    = 0.0;

	min   = 32767;
	max   = 0;
	bksum = 0;

	*rawsum = 0;
	*biassum = 0;

	for ( y = 0; y < Y; y++ ) {
	    for ( x = 0; x < X; x++ ) {
		if ( *mask <= R ) { 
			double pix;

		    Data = *data - *bias * biasscale;

		    if ( Data < min ) min = Data;
		    if ( Data > max ) max = Data;

		    if ( *mask >= rBk ) {
			bksum   += Data;
			*rawsum  += *data;
			*biassum  += *bias;
			
			bkpix++;
		    }

		    if ( *mask <= rCn ) {
			pix = (Data - Bk);

			if ( pix < 0 ) {
			    	pix = 0;
			}

			nipix++;

			sum +=     pix;
			xw  += x * pix;
			yw  += y * pix;
			xxw += x * xw;
			yyw += y * yw;
		    }
		}
		mask++;
		data++;
		bias++;
	    }
	    data += W;
	    bias += W;
	}

	*Sum   = sum;
	*Xw    = xw;
	*Yw    = yw;

	if ( sum == 0 ) {
	    *CX    = 0.0;
	    *CY    = 0.0;
	} else {
	    *CX    = (double) xw/sum;
	    *CY    = (double) yw/sum;
	}

	*Min   = min;
	*Max   = max;
	*Bksum = bksum;
}

centermsk(data, X, Y, W, mask, R, rBk, rCn)
 	    unsigned char	*data;
 	    int		 X;
 	    int		 Y;
 	    int		 W;
 	    unsigned char *mask;
	    int		 R;
	    int		 rBk;
	    int		 rCn;
{
	int	x, y;

	W -= X;
 
	for ( y = 1; y <= Y; y++ ) {
	    for ( x = 1; x <= X; x++ ) {
		if ( *mask <= R ) { 
			double pix;

		    *data = 0;;

		    if ( *mask >= rBk ) {
			*data = 255 - *mask;
		    }

		    if ( *mask <= rCn ) {
			*data = 255 - *mask;
		    }
		}
		mask++;
		data++;
	    }
	    data += W;
	}
}
