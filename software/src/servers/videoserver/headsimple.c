/* headsimple.c
 */

#include <xos.h>
#include <xfile.h>
#include <fitsy.h>

double msg_clock();

headsimple(mem, x1, y1, x, y, xzero, yzero, xscale, yscale, xunits, yunits, bitpix, frame, frac)
	void *mem;
	int	x1;
	int	y1;
	int	x;
	int	y;
	double	xzero;
	double	yzero;
	double	xscale;
	double	yscale;
	char	*xunits;
	char	*yunits;
	int	bitpix;
	int	frame;
	int	frac;
{
		FITSHead	fits;

	ft_cardclr((FITSCard) mem, FT_CARDS);

	fits = ft_headinit((FITSCard) mem, FT_BLOCK);
	ft_headappl(fits, "SIMPLE" , 0, 1   	, NULL);
	ft_headappi(fits, "BITPIX" , 0, bitpix  , NULL);
	/* ft_headappi(fits, "BZERO"  , 0, bitpix == 16 ? -32767 : 127, NULL); */
	ft_headappr(fits, "BSCALE" , 0, 1.0, 1, NULL);
	ft_headappi(fits, "BZERO"  , 0, 0, NULL); 
	ft_headappi(fits, "NAXIS"  , 0, 2   	, NULL);
	ft_headappi(fits, "NAXIS"  , 1, x 	, NULL);
	ft_headappi(fits, "NAXIS"  , 2, y 	, NULL);

	ft_headappi(fits, "FRAME"  , 0, frame 		, NULL);

	ft_headappr(fits, "EXPTIME", 0, frac/30.0,   3, NULL);
	ft_headappr(fits, "ENDTIME", 0, msg_clock(), 3, NULL);

	ft_loglcs(fits, 1.0, 0.0, 0.0, 1.0, -(double) (x1), -(double) (y1));
	ft_wcslcs(fits	, xscale, 0.0
		      	, 0.0, yscale
			, 0.0, 0.0
			, xzero-x1, yzero-y1
			, xunits, yunits);

	ft_headfree(fits, 0);
}


