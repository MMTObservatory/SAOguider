
int	camaz_initialized = 0;

int
frame_next (xcenter, ycenter, xsize, ysize, xbin, ybin, exptime, buffer)
int	xcenter;
int	ycenter;
int	xsize;
int	ysize;
int	xbin;
int	ybin;
double	exptime;
unsigned short *buffer;
{
	int	expt = exptime*1000;
	int	colfirst = xcenter - xsize/2 + 1;
	int	collast = colfirst + xsize - 1;
	int	rowfirst = ycenter - ysize/2 + 1;
	int	rowlast = rowfirst + ysize - 1;
	int	npix = xsize*ysize;

	if ((colfirst < 1) || (collast > xsize))
	    return 0;
	if ((rowfirst < 1) || (rowlast > ysize))
	    return 0;

	if (azcam_open ())
	    return 0;		/* failure */
	if (!camaz_initialized) {
	    if (azcam_geo2a ())
		return 0;
	    camaz_initialized = 1;
	}
	if (azcam_geo2b (xbin, colfirst, collast, ybin, rowfirst, rowlast))
	    return 0;

	azcam_light (expt);
	azcam_frame_read (npix);
	if (azcam_readdata (buffer, npix*sizeof(buffer[0])) == 0)
	    return 0;
	azcam_close ();

	return 1;	/* success */
}
