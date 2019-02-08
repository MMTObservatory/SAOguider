#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int	azcam_not_first_time;

int	azcam_nbytes;


azcam_open ()			/* device open */
{
	return (azcamio_open ());
}
azcam_close ()			/* device close */
{
	azcamio_set ("CloseConnection");
	return (azcamio_close ());
}


azcam_geo2a ()
{
	int	status;
	char	buffer[100];
	
	status = azcamio_set ("Reset");
	if (status)
	    return (status);

	return (status);
}

azcam_geo2b (xbin,colfirst,collast,ybin,rowfirst,rowlast)
int	xbin;
int	colfirst;
int	collast;
int	ybin;
int	rowfirst;
int	rowlast;
{
	int	status;
	char	buffer[100];

	char	hostname[100];
	char	ipname[100];
	char	*clientname;

	clientname = getenv ("AZCAMCLIENT");
	if (clientname != NULL) {
	    strncpy (ipname, clientname, sizeof(ipname));
	} else {
	    gethostname (hostname, sizeof(hostname));
	    host2ip (hostname, ipname);
	}

	/*
	sprintf (buffer, "SetConfiguration(0,0,1,1,0)");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	 */

	sprintf (buffer, "Set('FileHost',\"%s\")",ipname);
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('FilePort',\"%s\")","2403");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('FileFlag',2)");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('FileFolder',\"%s\")","/tmp");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('FileRoot',\"%s\")","image");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('FileType','bin')");
	status = azcamio_set (buffer);
	if (status)
	    return (status);
	sprintf (buffer, "Set('TestImage',0)");
	status = azcamio_set (buffer);
	if (status)
	    return (status);

	sprintf (buffer, "SetRoi %d %d %d %d %d %d",
	  colfirst, collast, rowfirst, rowlast, xbin, ybin);
	status = azcamio_set (buffer);
	if (status)
	    return (status);

	return (status);
}
azcam_light (exptime)
int	exptime;	/* milliseconds */
{
	char	buffer[100];
	double  secs = exptime/1000.0;

	azcamio_lopen ();
	sprintf (buffer, "Expose1(%.3f,\"object\")", secs);
	azcamio_set (buffer);
}

int	azcam_total_pixels;
int	azcam_total_pixels_readout;

azcam_frame_read (npixels)		/* start read out */
int	npixels;
{
	azcam_total_pixels = npixels;
	azcam_total_pixels_readout = 0;

	azcamio_aopen ();
	azcamio_startdataread ();
}
azcam_readdata (buffer, nbytes)
char	*buffer;
int	nbytes;
{
	int nxfer;

	nxfer = azcamio_dataread (buffer, nbytes);

	azcam_total_pixels_readout += nxfer/2;
	if (azcam_total_pixels_readout >= azcam_total_pixels) {
	    azcamio_aclose ();
	    azcamio_lclose ();
	}

	azcam_u2fits (buffer, nxfer);
	azcam_bswap (buffer, nxfer);
	return (nxfer);
}

/* The data from azcam come in little-endian unsigned short.
 * we must change to signed short for fits by subtracting 32768.
 * We can do this by flipping the MSbit of the MSbyte.
 */
azcam_u2fits (buffer, nbytes)
char	*buffer;
int	nbytes;
{
	unsigned char	*bp = (unsigned char *)buffer;
	int	n   = nbytes/sizeof(unsigned short);	/* number of pixels */

	bp++;			/* point to the first MSByte */
	for (; n; n--) {
	    *bp ^= 0x80;
	    bp += sizeof(unsigned short);
	}
}

/* The data from azcam come in little-endian.
 * If the ccdacq server is run on a big-endian host,
 * we must byte swap to the local endian-ness.
 */
azcam_bswap (buffer, nbytes)
char	*buffer;
int	nbytes;
{
	if (htons(1) == 1) {
	    unsigned	short	*bp = (unsigned short *)buffer;
	    int			n   = nbytes/sizeof(*bp);

	    for (; n; n--)
		*bp++ = htons(*bp);
	}
}
