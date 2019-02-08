/* Interface to the guide camera detector.
 */

#include <except.h>
 
#include <xos.h>
#include <xfile.h>

#include <msg.h>
/* #include <table.h> */
#include <fitsy.h>

#include "guide.h"

int Detector(guide, type)
	Guide	*guide;
	char	*type;
{
    int i;
    char errstr[256];

    errstr[0] = '\0';

    for ( i = 0 ; i < guide->nboxes ; i++ ) {
	int box = i;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;

	int imagepixsize = guide->imagesize / 
	  Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize);
	
	if (   guide->dirty 
	    || guide->box[box].dirty 
	    || guide->camera[cam].dirty 
	    || guide->image[img].dirty ) {

	    guide->box[box].lockedposcount = 0;
            guide->box[box].lockedpossumx  = 0;
            guide->box[box].lockedpossumy  = 0;
	  

	    if ( guide->box[box].y-imagepixsize/2 < 0 || guide->box[box].y+imagepixsize/2 > guide->camera[cam].ysize 
	      || guide->box[box].x-imagepixsize/2 < 0 || guide->box[box].x+imagepixsize/2 > guide->camera[cam].xsize ) {
		char	boxstr[256];

		guide->box[box].x = guide->camera[cam].xsize/2;
		sprintf(boxstr, "Box%d_X", box+1);
		msg_postval(guide->guideserv, boxstr, &guide->box[box].x);

		guide->box[box].y = guide->camera[cam].ysize/2;
		sprintf(boxstr, "Box%d_Y", box+1);
		msg_postval(guide->guideserv, boxstr, &guide->box[box].y);

		sprintf(boxstr, "%d ", i);
		strcat(errstr, boxstr);
	    } else {
	        if ( msg_command(guide->camerserv, MsgNulType, NULL, 0, MsgWait, 5.0
			     , "imageconfig %d %d %d %d %d"
			     , img+1
			     , (int)(guide->box[box].x + .5)
			     , (int)(guide->box[box].y + .5)
			     , imagepixsize
			     , guide->camera[cam].binning) <= 0 ) return 0;
	        guide->image[img].dirty = 0; 
	        guide->box[box].dirty = 0;
	        guide->box[box].xoff = 0;
	        guide->box[box].yoff = 0;
	        guide->camera[cam].dirty = 0;
	    }
	}
    }

    guide->dirty = 0;
    if ( !strcmp(type, "box") && *errstr ) {
	error("guide box positioned off chip : %s", errstr);
    }


    return 1;
}

int DetExpose(guide, type, time)
	Guide   *guide;
	char	*type;
	double	 time;
{
    int i;

    printf("                              Expose    %f\n", msg_clock());

    if ( !Detector(guide, type) )
	error("detector server not connected");
    if ( !strcmp(type, "box") ) {
	char buffer[256];

	if ( !guide->state )
		msg_seti(guide->camerserv, "guide", 1, 1.0);

	sprintf(buffer, "%s.%05d", guide->guidefile, guide->count);

	if ( guide->count ) {
	    guide->count++;
	    if ( !(guide->count % 10000) ) {
		guide->count = 1;
	    }
	}

	msg_setstr(guide->camerserv, "FILENAME", buffer, 1.0);
    } else {
	for ( i = 0 ; i < guide->nfullimages ; i++ ) {
	    int img = guide->fulllist[i]-1;
	    int cam = guide->image[img].camera-1;
	    msg_seti(guide->camerserv, "SPLITMODE", guide->camera[cam].split,   1.0);
	    msg_seti(guide->camerserv, "ROWBIN",    guide->camera[cam].binning, 1.0);
	    msg_seti(guide->camerserv, "COLBIN",    guide->camera[cam].binning, 1.0);
	    msg_setstr(guide->camerserv, "FILENAME", "acquire",                 1.0);
	}
    }

    msg_command(guide->camerserv, MsgNulType, NULL, 0, 52, 90.0
		, "expose %s %f", type, time);

    if ( msg_waitloop(guide->S, 5, 52, 0, 90.) < 0 ) return 0;

    return 1;
}

double **DetGetImage(guide, img, endtime, exptime)
	Guide 	*guide;
	int	 img;
	double	*endtime;
	double	*exptime;
{
	char	imname[256];
	void	*fitsdata;
	void	*realdata;
	FITSHead	fits;

	int     cam = guide->image[img].camera-1;
	int	pixsize = Abs(guide->camera[cam].bitpix/8);
	int     imrside = guide->imagesize / 
	  Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
	    guide->camera[cam].binning;


	int	impixels = imrside * imrside;
	int 	realsize = impixels * sizeof(double);
	int	fitssize = (1 + ft_fitsbloks(impixels * pixsize))
			   * FT_BLOCK;

    Malloc(fitsdata, fitssize);
    Malloc(realdata, realsize);

    sprintf(imname, "Image%d", img+1);

    if ( msg_getblk(guide->camerserv, imname, fitsdata, fitssize, 20.0) <= 0 ) {
	Free(fitsdata);
	Free(realdata);
	return NULL;
    }

    if ( (fits = ft_headinit(fitsdata, fitssize)) == NULL )
	return NULL;

   /* 
    * The Megacam guidecam server has been returning some corrupted fits
    * frames so guard against that.  19 Mar 04 BAM
    */
    if ( strncmp((char *)fitsdata, "SIMPLE", 6) ) return NULL;

    *endtime = ft_headgetr(fits, "ENDTIME", 0, 1.0, NULL);
    *exptime = ft_headgetr(fits, "EXPTIME", 0, 0.0, NULL);

    ft_dataswap( (char *) fitsdata+2880
		,(char *) fitsdata+2880
		, ft_databytes(fits), ft_bitpix(fits));
    ft_acht(-64, realdata, ft_bitpix(fits)
	    , (char *) fitsdata+2880, impixels, 1, ft_hasscaling(fits), ft_bscale(fits), ft_bzero(fits));

    ft_headfree(fits, 0);
    Free(fitsdata);

    return (double **) ft_make2d(realdata, sizeof(double), 0, 0, imrside, imrside);
}

DetFreeImage(guide, camera, data2d)
	Guide 	  *guide;
	int	  camera;
	double	**data2d;
{
	/* Free the image memory
	 */
	Free(data2d[0]);
	Free(data2d);
}

