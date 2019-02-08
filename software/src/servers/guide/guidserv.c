#include <except.h>
 
#include <xos.h>
#include <xfile.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <time.h>
#include <sys/time.h>

#include <parameter.h>
#include <fitsy.h>
/* #include <table.h> */
#include <msg.h>

double sqrt();
double pow();

#include "guide.h"

#define BOXBORDER	3

void transfer();
void flex();
void el_corr();
void az_corr();

void ComputeCorrections();

void initlog(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *data;
        int              argc;
        char            *argv[];
{
	if ( argc != 2 ) {
		msg_nak(c, msgid, "No log file name");
		return;
	}

	if ( data->log != NULL ) Close(data->log);

	if ( data->log = Open(argv[1], "r") ) {
		Close(data->log);
		data->log = Open(argv[1], "a");
	} else {
		if ( !(data->log = Open(argv[1], "a")) ) {
		    msg_nak(c, msgid, argv[1]);
		    return;
		}
		fprintf(data->log, "Guide Log file\n");
		fprintf(data->log, "\n");
		fprintf(data->log, "Date\tBoxlist\tCenX\tCenY\tFWHM\tSigmaXY\tSigmaFWHM\tErrors\tClipped\tImage\tCount\tSend\n");
		fprintf(data->log, "----\t-------\t----\t----\t----\t-------\t----------\t-----\t-------\t-----\t-----\tSend\n");
		Flush(data->log);
	}
	msg_ack(c, msgid, NULL);
}

void guidelog(guide, errors, clipped)
     Guide      *guide;
     double	*errors;	
     double	*clipped;	
{
    File	file = guide->log;
    int	        i;
    time_t	clock;
    struct tm   *btime;
    struct timeval tv;
    float frac_sec;
    char	xtime[26];
    
    if ( file == NULL ) return;
    
    clock = time(NULL);
    gettimeofday(&tv, NULL);
    btime = gmtime(&clock);
    frac_sec = tv.tv_usec/1000000.0;
    
    sprintf(xtime, "%4d %02d %02d %02d:%02d:%07.4f\0", btime->tm_year+1900, 
	    btime->tm_mon+1, btime->tm_mday, btime->tm_hour, btime->tm_min, 
	    btime->tm_sec+frac_sec);
    
    fprintf(file, "%s", xtime);
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%d", i ? ' ' : '\t', box );
    }
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%g", i ? ' ' : '\t', guide->box[box].cenx);
    }
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%g", i ? ' ' : '\t', guide->box[box].ceny);
    }
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%g", i ? ' ' : '\t', guide->box[box].fwhm);
    }
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%g", i ? ' ' : '\t', guide->box[box].sigmaxy);
    }
    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	fprintf(file, "%c%g", i ? ' ' : '\t', guide->box[box].sigmafwhm);
    }
    for ( i = 0; i < guide->nerrors; i++ ) {
	fprintf(file, "%c%g", i ? ' ' : '\t', errors[i]);
    }		
    for ( i = 0; i < guide->nerrors; i++ ) {
	fprintf(file, "%c%g", i ? ' ' : '\t', clipped[i]);
    }		
    fprintf(file, "\t%s\t%d\t%d\n", guide->guidefile, guide->count, guide->send);
    fflush(file);
}

void printbox(box, x, y)
	double	**box;
	int	  x, y;
{
	int	i, j;

	for ( j = 0; j < y; j++ ) {
	    for ( i = 0; i < x; i++ ) {
		fprintf(stderr, "%10.2f ", box[j][i]);
	    }
	    fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

void sighandler()
{

}

void quit()
{
	exit(1);
}

void *setdirty(s, type, name, data, buff, leng)
        MsgServer        s;
        int              type;
        char            *name;
        int             *data;
        void            *buff;
        int             *leng;
{
    if( type == MsgSetVal ) {
        *data  = 1;
    }
    return buff;
}

void expose(s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    iferr ( if ( !DetExpose(guide, argv[1], atof(argv[2]))
	) { error("%s: %s", "guide: can't expose image",
	errmsg()); } msg_ack(c, msgid, NULL); ) {
	msg_nak(c, msgid, errmsg()); } }


void Tick(s, guide)
	MsgServer	s;
	Guide	       *guide;
{
	int	i;

	double	**data2d[10];
	double	 endtime[10];
	double	 exptime[10];

	void *ptr;

	for ( i = 0; i < guide->nactiveboxes; i++ ) {
	    int box = guide->boxlist[i]-1;
	    int img = guide->box[box].image-1;
	    int cam = guide->image[img].camera-1;

	    int boxsizepix = guide->boxsize / 
	              Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
		      guide->camera[cam].binning;

	    int imagesizepix = guide->imagesize / 
	              Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
		      guide->camera[cam].binning;
	    
	    if ( boxsizepix > imagesizepix ) {
		error("guide: box too large");
	    }

            if ( guide->image[img].camera == -1 ) {
		error("guide: image %d not initialized\n", i+1);
	    }
	}

	if ( !DetExpose(guide, "box", guide->exposure) ) {
		error("%s: %s", "guide: can't expose image", errmsg());
	}

	if ( !guide->state ) return; /* We might have been stopped while exposing */

	for ( i = 0; i < guide->nactiveboxes; i++ ) {
	    int box = guide->boxlist[i]-1;
	    int img = guide->box[box].image-1;
	    int cam = guide->image[img].camera-1;

	    if ( (data2d[i] = DetGetImage(guide, img, &endtime[i], &exptime[i])) == NULL ) {
		error("guide: can't get image %d for box %d: ", img+1, box+1);
	    }
	}

	if ( i ) ComputeCorrections(s, guide, data2d, endtime, exptime, i);

	while ( i-- ) {
	    DetFreeImage(guide, 1, data2d[i]);
	}
}

void tick(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *data;
        int              argc;
        char            *argv[];
{
    iferr (
        Tick(s, data);
     ) {
	msg_nak(c, msgid, errmsg());
     } else {
	msg_ack(c, msgid, NULL);
     }
}


double Ticker(s, tid, data)
        MsgServer       s;
	int		tid;
        Guide          *data;
{
	       double this = msg_clock();
	static double last = 0.0;

    iferr ( Tick(s, data) ) {
	fprintf(stderr, "%s\n", errmsg());
	data->tid   = 0;
	data->state = 0;
	msg_postval(s, "error", errmsg());
	return 0.0;
    }


    fprintf(stderr, "Tick	%f	%f	%f\n", data->delay, msg_clock() - last, msg_clock()); 
    last = this;

    return Max(.01, data->delay - (msg_clock() - this));
}

void start(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *data;
        int              argc;
        char            *argv[];
{
	if ( data->tid != 0 ) {
	    msg_nak(c, msgid, "guide is already started");
	    return;
	}


	iferr (
/* 	    Tick(s, data); */
	    data->tid = msg_addtimer(s
			, 0.01, Ticker, data);
	    data->state++;
        ) {
		msg_nak(c, msgid, errmsg());
	} else {
		msg_ack(c, msgid, NULL);

		msg_seti(data->camerserv, "guide", 1, 1.0);
	}
}

void stop(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *data;
        int              argc;
        char            *argv[];
{
	if ( data->tid != 0 ) msg_deltimer(s, data->tid);
	msg_ack(c, msgid, NULL);

	data->tid   = 0;
	data->state = 0;

}

void tweak(s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    int	     *boxes;
    int	      nbox;
    double ***data2d;
    int n;
    int i;

    double	etime, dtime;
    
    if ( argc < 2 ) {
	msg_nak(c, msgid, "wrong number of args to tweak");
	return;
    }
    if ( argc == 2 ) {
	nbox  = guide->nactiveboxes;
	boxes = guide->boxlist;
    } else {
	nbox = argc - 2;
	Malloc(boxes, sizeof(int) * nbox);
	for ( i = 0; i < nbox; i++ ) {
	    boxes[i] = atoi(argv[i+2]);
	}
    }
    
    iferr (
	   if ( !strcmp(argv[1], "data") ) {
	       if ( !DetExpose(guide, "box", guide->exposure, &etime, &dtime) ) {
		   error("guide: can't expose image");
	       }
	   }
	   
	   Malloc(data2d, guide->nactiveboxes * sizeof(*data2d));
	   
	   for ( i = 0; i < nbox; i++ ) {
	       int box = boxes[i]-1;
	       int img = guide->box[box].image-1;
	       int cam = guide->image[img].camera-1;
	       
	       if ( (data2d[i] = DetGetImage(guide, img, &etime, &dtime)) == NULL ) {
		   error("guide: can't get image %d for box %d", img+1, box+1);
	       }
	   }
	   if ( (n=tweakboxes(s, guide, data2d)) != guide->nactiveboxes ) {
	       error("Tweaked only %d of %d requested boxes", n, nbox);
	   }
	   
	   if ( !DetExpose(guide, "box", guide->exposure) ) {
	       error("guide: can't expose image");
	   }
    ) {
	msg_nak(c, msgid, errmsg());
	guide->image[1].dirty = 1;
	return;
    }
    
    msg_ack(c, msgid, NULL);
}

void 
stats(double *data, int npix, double lowthresh, 
      double upperthresh, double *mean, double *sigma)
{
    double sum=0;
    double sum2=0;
    double count=0;

    int i;
    for (i=0; i<npix; i++) {
      if (data[i] > lowthresh  && data[i] < upperthresh){
            sum += data[i] ;
            count += 1;
      }
    } 

    if ( count == 0 ) {
	*mean = 0;
	*sigma = 0;
    }

    *mean = sum/count;


    for (i=0; i<npix; i++) {
      if (data[i] > lowthresh  && data[i] < upperthresh){
            sum2 += pow((data[i] - *mean), 2.);
      }
    }
    *sigma = sqrt(sum2/count);
}

void 
ClippedBackground(double *input, int npix, int niter,
	 double *mean, 
         double *sigma)
{
    int i;

    *mean = 0;
    *sigma  = 1e36/3;

    /* Adjust mean and sigma */
    for ( i = 0; i < niter; i++ ) {
        stats(input, npix, *mean - 2 * *sigma, *mean + 2 * *sigma,
              mean, sigma);
	if ( Abs(*sigma) < 0.01 ) break;
    }
}

double **ClipImage(box, x, y, bg, thresh) 
    double **box;
    int x;
    int y;
    double bg;
    double thresh;
{
  int i,j,ii,jj;

  double *clipbox;
  double **clipbox2d;

  Calloc(clipbox, x * y * sizeof(double));
  clipbox2d = (double **) ft_make2d((void *) clipbox, sizeof(double), 0, 0
			, x, y);

  for (i = 0; i < y; i++) {
    for (j = 0; j < x; j++) {
      int count=0;
      for (ii = -1 ; ii <= 1; ii++) {
	for (jj = -1 ; jj <= 1 ; jj++) {
	  if ((ii+i < 0 ) || (ii+i>=y) || (jj+j<0) || (jj+j>=x)) {
	    count++;
	  } else {
	    if (box[ii+i][jj+j] >= thresh) {
	      count++;
	    }
	  }
	}
      }
      if (count==9) {
	clipbox2d[i][j] = box[i][j];
      } else {
	clipbox2d[i][j] = bg;
      }
    }
  }
  return clipbox2d;
}

int tweakboxes(s, guide, data2d)
        MsgServer       s;
	Guide  *guide;
	double	***data2d;
{
	double	offx, offy;

	int	i, x, y;

	int	ret = 0;


    for ( i = 0; i < guide->nactiveboxes; i++ ) {
        int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;

	int imgsize = guide->imagesize / 
	  Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
	    guide->camera[cam].binning;

	int boxsizepix = guide->box[box].boxsizepix;

	CalcBox(guide, box, imgsize, boxsizepix, data2d[i]);

	if ( guide->box[box].sigmaxy < guide->boxsize ) {
	    guide->box[box].x += guide->box[box].cenx;
	    guide->box[box].y += guide->box[box].ceny;
	    guide->image[img].dirty = 1;
	    ret++;
	} 
    }

    return ret;
}

double BoxRatio(nbins, box)
	int	nbins;
	double	**box;
{
	int	i, j;

	double	center 	= 0.0;
	double	ring	= 0.0;

	for ( i = 0; i < nbins; i++ ) 
	    for ( j = 0; j < nbins; j++ )
		if ( i == 2 && j == 2 )
			center =  box[i][j];
		else
			ring  += box[i][j];

	return center/(ring/8);
}

void ComputeCorrections(s, guide, data2d, endtime, exptime, n)
	MsgServer	  s;
	Guide		 *guide;
	double         ***data2d;
	double	         *endtime;
	double	         *exptime;
	int		  n;
{
    double	timeout = 10.0;
    
    double	 *boxptr;
    double	**boxptr2d;
    
    double	rotator = guide->rotang;
    
    double	*roterror;
    double	*xy_error;
    double	*errors;
    double	*clipped;
    
    int	 i, j;
    
    double	background;
    
    double sumcenaz = 0;
    double sumcenel = 0;
    double snrmin = 1.0e37;
        
    int	nerrors;
    double   angle;
    double pixsize;
    
    guide->ngoodboxes = 0;

    /*  
     * For each box, compute the centroids, etc 
     */
    guide->locked = guide->send && guide->inpos && guide->gvalid && guide->wvalid && guide->nactiveboxes;
    for ( i = 0; i < n; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;
	
	
	int imagesizepix = guide->imagesize / 
	  Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
	    guide->camera[cam].binning;
	    
	int boxsizepix = guide->box[box].boxsizepix;
    
	CalcBox(guide, box, imagesizepix, boxsizepix, data2d[i]);

fprintf(stderr, "Box %d:	X: %.3f	Y: %.3f	%d	%d\n"
	, box, guide->box[box].cenx, guide->box[box].ceny
	, imagesizepix, boxsizepix);

 	guide->box[box].locked = 0;

	if (    guide->box[box].sigmaxy >= 0
             && guide->box[box].sigmaxy <  guide->box[box].sigmaxythresh 
	     && endtime[i] > 0 ) {
	    sumcenaz += guide->box[box].cenaz;
	    sumcenel += guide->box[box].cenel;
	    if ( Abs(guide->box[box].cenaz) < guide->error[AzErr].locktol
	      && Abs(guide->box[box].cenel) < guide->error[ElErr].locktol ) {
 		guide->box[box].locked = 1;
	    }

	    guide->ngoodboxes++;
	} else {
	  if (guide->box[box].lockedposcount) {
	        double az, el;
	        xytoazel(guide, box, 
			 guide->box[box].lockedpossumx / guide->box[box].lockedposcount, 
			 guide->box[box].lockedpossumy / guide->box[box].lockedposcount, 
			 &az, &el);
		sumcenaz += az;
		sumcenel += el;
	    }

	    guide->box[box].locked = 0;
	    guide->locked          = 0;
	}
    }
fprintf(stderr, "\n");

    if (   guide->ngoodboxes <= 0 
        || guide->gvalid     == 0
        || guide->wvalid     == 0
	) {
	return;
    }

    Calloc(errors,   guide->nerrors * sizeof(double));
    Calloc(clipped,  guide->nerrors * sizeof(double));

    /* 
     * AZ and EL tracking errors in arcsec 
     */
    errors[AzErr] = sumcenaz / guide->ngoodboxes;
    errors[ElErr] = sumcenel / guide->ngoodboxes;

/*
    fprintf(stderr, "Errors Az: %8.3f	El : %8.3f	Rot : %8.3f\n"
	    , errors[AzErr], errors[ElErr], errors[RotErr]);
 */

    if (guide->nerrors > RotErr) {
	if ( guide->ngoodboxes > 1) {
	   azelrot(guide, errors);
	} else {
	    errors[RotErr] = 0;
	}
    }

    for ( i = 0; i < guide->nerrors; i++ ) {
	if ( Abs(errors[i]) > guide->error[i].locktol ) {
	    guide->locked = 0;
	}
    }

    if ( guide->locked ) {
        for ( i = 0; i < n; i++ ) {
	    int box = guide->boxlist[i]-1;
	    guide->box[box].lockedposcount++;
	    guide->box[box].lockedpossumx += guide->box[box].cenx;
	    guide->box[box].lockedpossumy += guide->box[box].ceny;
	}
    }

    if ( guide->nerrors > FocErr ) {
	if ( guide->ngoodboxes > 1) {
		int	img;
		int	cam;
		double defocus1; 
		double defocus2;

		double magnification = (guide->cassfratio / guide->primaryfratio)
				     * (guide->cassfratio / guide->primaryfratio) + 1;

		img = guide->box[0].image-1;
		cam = guide->image[img].camera-1;

		defocus1   = guide->camera[cam].defocus;

		img = guide->box[1].image-1;
		cam = guide->image[img].camera-1;

		defocus2   = guide->camera[cam].defocus;

/*
fprintf(stderr, "Defocus1 = %f	Defocus2 = %f\n", defocus1, defocus2);
fprintf(stderr, "FWHM     = %f	FWHM     = %f\n", guide->box[0].fwhm, guide->box[1].fwhm);
 */


		if ( (defocus1 - defocus2) != 0 ) {
		  errors[FocErr] = (guide->box[0].fwhm*guide->box[0].fwhm
				    - guide->box[1].fwhm*guide->box[1].fwhm
				    -  defocus1*defocus1 + defocus2*defocus2)
  		                  / (2 * (defocus1 - defocus2)) * 180 / magnification * guide->cassfratio;
		} else {
		  errors[FocErr] = 0;
		}

		/* 180 u per arcsec 	*/

	} else {
	    errors[FocErr] = 0;
	}
    }

    for ( i = 0; i < guide->nerrors; i++ ) {
	    double teldelay = guide->error[i].teldelay;
	    double begtime  = *endtime - *exptime;
	    double sample;

	for ( j = 0; j < MAX_LOOKBACK; j++ ) {
	        double errsent = guide->error[i].lookback[j].time;
		double errrecv = errsent + teldelay;
		double errapplied;

	    /* How much of the error is implimented by mid exposure?
	     */
	    errapplied = guide->error[i].lookback[j].sent;


if ( i == -1 ) {
/*	fprintf(stderr, "Look back %.2f -- %.2f %.2f %.2f  --> %.2f\n", teldelay, errrecv, begtime, *endtime, errapplied);
 */
}
	    if ( *endtime < errrecv ) {
/* fprintf(stderr, "Full   error %f %f\n", errapplied); */
		errors[i] -= errapplied;
	    } else {

		if ( errrecv <= *endtime && errrecv > begtime ) {
		    double errseenscale = 1 - ((*endtime - errrecv) / *exptime);

    /* fprintf(stderr, "Scale error %f %f\n", errapplied * errseenscale, errseenscale); */

		    errors[i] -= errapplied * errseenscale;
		}
	    }
	}

	guide->error[i].integrateq += errors[i];
	guide->error[i].integrates++;
	
	if ( guide->error[i].integrates >= guide->error[i].sample ) {
	    sample = guide->error[i].integrateq / guide->error[i].integrates;
	    guide->error[i].integrateq  = 0.0;
	    guide->error[i].integrates  = 0;
	} else {
	    sample = 0.0;
	}
	sample *= guide->error[i].active;
	clipped[i] = Clip( -guide->error[i].step
			  , guide->error[i].step
			  , (sample - guide->error[i].offset) * guide->error[i].gain);
    }

    /* Cue the clipped errors into the sent array
     */
    {	double stamp = msg_clock();

	guide->looknext = ( guide->looknext + 1 ) % MAX_LOOKBACK;

	for ( i = 0; i < guide->nerrors; i++ ) {
	    guide->error[i].lookback[guide->looknext].sent = clipped[i];
	    guide->error[i].lookback[guide->looknext].time = stamp;
	}
    }

    for ( i = 0; i < guide->nerrors; i++ ) {
	msg_postmrk(s, guide->error[i].rawhdl, &errors[i]);
	msg_postmrk(s, guide->error[i].errhdl, &clipped[i]);
    }

    guidelog(guide, errors, clipped);
    
    if ( guide->inpos && guide->send && guide->nerrors > FocErr ) {
	if ( clipped[FocErr] != 0.0 && finite(clipped[FocErr]) ) {
	    if ( msg_command(guide->focusserv, MsgNulType, NULL, 0, MsgWait, timeout
			     , "focuserr %f", clipped[FocErr])
		<= 0 ) {
		error("guide: can't send focus error to focus");
	    } else {
		fprintf(stderr, "focuserr %f\n", errors[FocErr]);
	    }
	}
    }
    
    if ( guide->inpos && guide->send && guide->nerrors > RotErr ) {
	if ( guide->inpos && guide->send && ( clipped[AzErr] != 0.0 || clipped[ElErr] != 0.0 || clipped[RotErr] != 0.0 )  && finite(clipped[AzErr]) && finite(clipped[ElErr]) && finite(clipped[RotErr]) ) {
	    if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgNoAck, timeout
			     , "azelerr %f %f %f", clipped[AzErr], clipped[ElErr], clipped[RotErr] * 3600)
		<= 0 ) {
		fprintf(stderr, "guide: can't send Az/El errors to mount\n");
	    } else {
		fprintf(stderr, "azelerr %f %f %f\n", clipped[AzErr], clipped[ElErr], clipped[RotErr] * 3600);
	    }
	}
    } else {
	if ( guide->inpos && guide->send && ( clipped[AzErr] != 0.0 || clipped[ElErr] != 0.0 )  && finite(clipped[AzErr]) && finite(clipped[ElErr]) ) {
	    if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgNoAck, timeout
			     , "azelerr %f %f %f", clipped[AzErr], clipped[ElErr], 0.0)
		<= 0 ) {
		fprintf(stderr, "guide: can't send Az/El errors to mount\n");
	    } else {
		fprintf(stderr, "azelerr %f %f %f\n", clipped[AzErr], clipped[ElErr], 0.0);
	    }
	}
    }
    
    Free(errors);
    Free(clipped);
}

/* Dither the pointing in instrument coordinates
 */
void ditherradec (s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    double azamt, elamt;
    double	timeout = 10.0;
    int i;

    if ( argc != 3 ) {
	msg_nak(c, msgid, "Wrong number of args:  ditherradec raamt decamt");
	return;
    }

    azamt = atof(argv[1]);
    elamt = atof(argv[2]);

    if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgWait, timeout
			     , "radecrel %f %f", azamt, elamt )
	<= 0 ) {
      fprintf(stderr, "guide: can't send dither command to mount\n");
    }
    sleep(1);  /* Here we should ask the telescope when it is done moving */

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;
	double angle = -(guide->camera[cam].rotation - guide->posang) / DEGSPERRAD;
	
fprintf(stderr, "angle = %f	%f + %f\n", angle, guide->camera[cam].rotation, guide->posang);

/* azel	double x =  azamt * cos(angle) + elamt * sin(angle);
	double y = -azamt * sin(angle) + elamt * cos(angle);
 */

	double x = -azamt * cos(angle) + elamt * sin(angle);
	double y =  azamt * sin(angle) + elamt * cos(angle);
	
fprintf(stderr, "dx = %f	dy = %f\n", x, y);

	guide->box[box].x -= x / guide->camera[cam].xpixelsize / guide->camera[cam].parity;
	guide->box[box].y -= y / guide->camera[cam].ypixelsize;


	guide->image[img].dirty = 1;
    }

    msg_ack(c, msgid, NULL);
}

/* Dither the pointing in instrument coordinates
 */
void ditherinst (s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    double azamt, elamt;
    double	timeout = 10.0;
    int i;

    if ( argc != 3 ) {
	msg_nak(c, msgid, "Wrong number of args:  ditherinst azamt elamt");
	return;
    }

    azamt = atof(argv[1]);
    elamt = atof(argv[2]);

    if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgWait, timeout
			     , "instrel %f %f", azamt, elamt )
	<= 0 ) {
      fprintf(stderr, "guide: can't send dither command to mount\n");
    }
    sleep(1);  /* Here we should ask the telescope when it is done moving */

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;
	double angle = -(guide->camera[cam].rotation) / DEGSPERRAD;
	
	double x =  azamt * cos(angle) + elamt * sin(angle);
	double y = -azamt * sin(angle) + elamt * cos(angle);
	
	guide->box[box].x -= x / guide->camera[cam].xpixelsize / guide->camera[cam].parity;
	guide->box[box].y -= y / guide->camera[cam].ypixelsize;
	guide->image[img].dirty = 1;
    }

    msg_ack(c, msgid, NULL);
}

/* Dither the pointing in telescope coordinates
 */
void ditherazel (s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    double azamt, elamt;
    double x, y;
    double	timeout = 10.0;
    int i;

    if ( argc != 3 ) {
	msg_nak(c, msgid, "Wrong number of args:  ditherinst azamt elamt");
	return;
    }

fprintf(stderr, "AZ: %s	EL: %s\n", argv[1], argv[2]);

    azamt = atof(argv[1]);
    elamt = atof(argv[2]);

fprintf(stderr, "AZ: %f	EL: %f\n", azamt, elamt);

    if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgWait, timeout
			     , "azelrel %f %f", azamt, elamt )
	<= 0 ) {
      fprintf(stderr, "guide: can't send dither command to mount\n");
    }
    sleep(1);  /* Here we should ask the telescope when it is done moving */

    for ( i = 0; i < guide->nactiveboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;

	azeltoxy(guide, box, azamt, elamt, &x, &y);

fprintf(stderr, "Box: %d	DX: %f	DY: %f\n", box+1, x, y);

	guide->box[box].x -= x;
	guide->box[box].y -= y;
	guide->image[img].dirty = 1;
    }

    msg_ack(c, msgid, NULL);
}

/* offset the telescope so that the star at these image coordinates
 * moves into the box.
 */
void imageoffset (s, c, msgid, guide, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Guide           *guide;
        int              argc;
        char            *argv[];
{
    double azamt, elamt;
    double xamt, yamt;
    double xpos, ypos;
    double factor;
    double      timeout = 10.0;
    int i;

    if ( argc != 5 ) {
	msg_nak(c, msgid, "Wrong number of args:  imageoffset imagename xpos ypos factor");
	return;
    }

    xpos = atof(argv[2]);
    ypos = atof(argv[3]);
    factor = atof(argv[4]);

/*
fprintf(stderr, "%s %s %f %f %f\n", argv[0], argv[1], xpos, ypos, factor);
*/


    for ( i = 0; i < guide->nimages; i++ ) {
	if (strcmp (argv[1], guide->image[i].name))
	    continue;
	if (guide->image[i].fullimage) { /* this is a box */
	    xamt = xpos - guide->image[i].imagesizepix/2;
	    yamt = ypos - guide->image[i].imagesizepix/2;
	} else {			 /* this is a full image */
	    if (guide->nactiveboxes <= 0)
	        continue;
	    int box = guide->boxlist[0]-1; /* the first active box */
	    xamt = xpos - guide->box[box].x;
	    yamt = ypos - guide->box[box].y;
	}

	xyitoazel(guide, i, xamt, yamt, &azamt, &elamt);
	azamt *= factor;
	elamt *= factor;

/*
fprintf(stderr, "azelerr %f %f 0.0\n", azamt, elamt);
*/

	if ( msg_command(guide->mountserv, MsgNulType, NULL, 0, MsgWait, timeout
				 , "azelerr %f %f 0.0", azamt, elamt ) <= 0 ) {
	  fprintf(stderr, "guide: can't send azelerr command to mount\n");
	}
	/* sleep(1);  Here we should ask the telescope when it is done moving */

    }

    msg_ack(c, msgid, NULL);
}
	
exception	args = "guide args";

void argerr(level, message, name)
	int	 level;
	char	*message;
	char	*name;
{
	except(args, "%s: %s: %s", message, paramerrstr(), name);
}

int cinit(s, guide, state)
	MsgServer       s;
	Guide	       *guide;
        int             state;
{

    guide->dirty = 1;

    return 1;
}


void *wvalidate(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
     guide->wvalid = ((guide->wstagepos > guide->wstagemin) && (guide->wstagepos < guide->wstagemax ));
}
 

void *camupdate(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
     msg_postmrk(guide->guideserv, msg_nthvalue(guide->guideserv, name), buff);
}

void *PublishCamera(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
    int ncameras = *(int *)buff;
    int i;
    if (msgtype != MsgSetVal) return NULL;

    guide->camera = (Camera *) calloc (sizeof(Camera), ncameras);
    for (i=0; i < ncameras; i++) {
	msg_publish(s, "Camera%d_Name",        MsgStrType,  guide->camera[i].name,       MAXBUFLEN, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_XSize",       MsgIntType, &guide->camera[i].xsize,      0, setdirty, &guide->camera[i].dirty, NULL, i+1);
	msg_publish(s, "Camera%d_YSize",       MsgIntType, &guide->camera[i].ysize,      0, setdirty, &guide->camera[i].dirty, NULL, i+1);
	msg_publish(s, "Camera%d_XPixelSize",  MsgDblType, &guide->camera[i].xpixelsize, 0, setdirty, &guide->camera[i].dirty, NULL, i+1);
	msg_publish(s, "Camera%d_YPixelSize",  MsgDblType, &guide->camera[i].ypixelsize, 0, setdirty, &guide->camera[i].dirty, NULL, i+1);
	msg_publish(s, "Camera%d_Binning",     MsgIntType, &guide->camera[i].binning,    0, setdirty, &guide->camera[i].dirty, NULL, i+1);
	msg_publish(s, "Camera%d_Radius",      MsgDblType, &guide->camera[i].radius,     0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_Rotation",    MsgDblType, &guide->camera[i].rotation,   0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_CamAngle",    MsgDblType, &guide->camera[i].camangle,   0, NULL, NULL, NULL, i+1);

	msg_publish(s, "Camera%d_Parity",      MsgIntType, &guide->camera[i].parity,     0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_Defocus",     MsgDblType, &guide->camera[i].defocus,    0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_Split",       MsgIntType, &guide->camera[i].split,      0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Camera%d_Bitpix",      MsgIntType, &guide->camera[i].bitpix,     0, NULL, NULL, NULL, i+1);
    }

    return buff;
}

void *PublishImage(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
    int nimages = *(int *)buff;
    int i;
    if (msgtype != MsgSetVal) return NULL;

    guide->image = (Image *) calloc (sizeof(Image), nimages);

    for (i=0; i < nimages; i++) {
fprintf(stderr, "Image%d\n", i);

	msg_publish(s, "Image%d_Name",         MsgStrType,  guide->image[i].name,         MAXBUFLEN, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Camera",       MsgIntType, &guide->image[i].camera,               0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Exposure",     MsgDblType, &guide->image[i].exposure,             0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Defocus",      MsgDblType, &guide->image[i].defocus,              0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Grid",         MsgStrType,  guide->image[i].grid,         MAXBUFLEN, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_GridWidth",    MsgIntType, &guide->image[i].gridwidth,            0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_GridHeight",   MsgIntType, &guide->image[i].gridheight,           0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_GridRotate",   MsgIntType, &guide->image[i].gridrotate,           0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_GridRotation", MsgDblType, &guide->image[i].gridrotation,         0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_GridZoom",     MsgDblType, &guide->image[i].gridzoom,             0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_ImageSizePix", MsgIntType, &guide->image[i].imagesizepix,         0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_ShmKeys",      MsgStrType,  guide->image[i].shmkeys,      MAXBUFLEN, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_ShmSize",      MsgIntType, &guide->image[i].shmsize,              0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_FullImage",    MsgIntType, &guide->image[i].fullimage,            0, NULL, NULL, NULL, i+1);
	
	msg_publish(s, "Image%d_Config",       MsgIntType, NULL, 0, NULL, NULL, NULL, i+1);
        msg_subscribe(guide->camerserv, "Image%d_Config", MsgIntType, NULL, sizeof(int), 0.0, camupdate, guide, 1.0, i+1);
	msg_publish(s, "Image%d_Update",       MsgIntType, NULL, 0, NULL, NULL, NULL, i+1);
        msg_subscribe(guide->camerserv, "Image%d_Update", MsgIntType, NULL, sizeof(int), 0.0, camupdate, guide, 1.0, i+1); 
    }

    return buff;
}

void *PublishBox(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
    int nboxes = *(int *)buff;
    int i;
    if (msgtype != MsgSetVal) return NULL;

    guide->box = (Box *) calloc (sizeof(Box), nboxes);
    for (i=0; i < nboxes; i++) {
      fprintf(stderr, "Publishing box %d\n",i);
	msg_publish(s, "Box%d_Image",          MsgIntType, &guide->box[i].image,       0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_X",              MsgDblType, &guide->box[i].x,           0, setdirty, &guide->box[i].dirty, NULL, i+1);
	msg_publish(s, "Box%d_Y",              MsgDblType, &guide->box[i].y,           0, setdirty, &guide->box[i].dirty, NULL, i+1);
	msg_publish(s, "Box%d_XOff",           MsgDblType, &guide->box[i].xoff,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_YOff",           MsgDblType, &guide->box[i].yoff,        0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_FWHM",	       MsgDblType, &guide->box[i].fwhm,        0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_CenX",	       MsgDblType, &guide->box[i].cenx,        0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_CenY",	       MsgDblType, &guide->box[i].ceny,        0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_CenAz",	       MsgDblType, &guide->box[i].cenaz,       0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_CenEl",	       MsgDblType, &guide->box[i].cenel,       0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Box%d_Counts",	       MsgDblType, &guide->box[i].counts,           0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Box%d_Background",     MsgDblType, &guide->box[i].background,       0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_Noise",          MsgDblType, &guide->box[i].noise,            0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_SigmaXYThresh",  MsgDblType, &guide->box[i].sigmaxythresh,    0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_SigmaXY",        MsgDblType, &guide->box[i].sigmaxy,          0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_SigmaFWHMThresh",MsgDblType, &guide->box[i].sigmafwhmthresh,  0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_SigmaFWHM",      MsgDblType, &guide->box[i].sigmafwhm,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_BoxSizePix",     MsgIntType, &guide->box[i].boxsizepix,       0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Box%d_Locked",         MsgIntType, &guide->box[i].locked,           0, NULL, NULL, NULL, i+1);

    	guide->box[i].sigmaxythresh  = 0.1;
    	guide->box[i].sigmafwhmthresh  = 0.1;
    	guide->box[i].xoff  = 0;
    	guide->box[i].yoff  = 0;
    }

    return buff;
}


void *PublishError(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     void *buff;
     int leng;
{
    int nerrors = *(int *)buff;
    int i;
    if (msgtype != MsgSetVal) return NULL;

    guide->error = (Error *) calloc (sizeof(Error), nerrors);
    for (i=0; i < nerrors; i++) {
	msg_publish(s, "Error%d_Name",         MsgStrType,  guide->error[i].name,        MAXBUFLEN, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_Active",       MsgIntType, &guide->error[i].active,      0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_Gain",         MsgDblType, &guide->error[i].gain,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_Offset",       MsgDblType, &guide->error[i].offset,      0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_Step",         MsgDblType, &guide->error[i].step,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_Sample",       MsgIntType, &guide->error[i].sample,      0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_LookB",        MsgDblType, &guide->error[i].teldelay,    0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Error%d_LockTol",      MsgDblType, &guide->error[i].locktol,    0, NULL, NULL, NULL, i+1);

        guide->error[i].errhdl = msg_publish(s, "Error%d",      MsgDblType, NULL,      0, NULL, NULL, NULL, i+1);
        guide->error[i].rawhdl = msg_publish(s, "Error%d_Raw",  MsgDblType, NULL,      0, NULL, NULL, NULL, i+1);

    	guide->error[i].gain	= 1;
    	guide->error[i].offset	= 0;
    	guide->error[i].step	= 1;
        guide->error[i].sample  = 1;
    }

    return buff;
}

void *ParseActiveBoxList(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    char *str;
    char *copy = strdup(buff);
    guide->nactiveboxes = 0;
    str = strtok(copy, " ");
    while (str != NULL) {
        int box = guide->boxlist[guide->nactiveboxes++] = atoi(str);
        box--;
       	guide->box[box].lockedposcount = 0;
	guide->box[box].lockedpossumx  = 0;
        guide->box[box].lockedpossumy  = 0;
	str = strtok(NULL, " ");
    }

    free(copy);
    fprintf(stderr,"Done Parsing boxes.\n");
    return buff;
}

void *SetGuideFile(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    char *str;

    if ( !strcmp(buff, "guide") ) {
	guide->count = 0;
    } else {
	guide->count = 1;
    }
    return buff;
}

void *ParseFullImageList(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    char *str;
    char *copy = strdup(buff);
    guide->nfullimages = 0;
    str = strtok(copy, " ");
    while (str != NULL) {
        guide->fulllist[guide->nfullimages++] = atoi(str);
	str = strtok(NULL, " ");
    }
    free(copy);
    return buff;
}

void *setlocked(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    if( msgtype == MsgSetVal ) {
        guide->locked = 0;
    }
    return buff;
}
void *setfullbin(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    int i;
    for (i=0; i<guide->nfullimages; i++) {
	int img = guide->fulllist[i]-1;
	int cam = guide->image[img].camera-1;
	guide->camera[cam].binning = atoi(buff);
    }
    return buff;
}

void *setboxbin(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    int i;
    guide->dirty = 1;
    for ( i = 0; i < guide->nboxes; i++ ) {
	int box = guide->boxlist[i]-1;
	int img = guide->box[box].image-1;
	int cam = guide->image[img].camera-1;
	guide->camera[cam].binning = atoi(buff);
    }
    return buff;
}

void *setimagesize(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    int i;

    guide->dirty = 1;
    for ( i = 0; i < guide->nboxes; i++ ) {
	int img = guide->box[i].image-1;
	int cam = guide->image[img].camera-1;
	guide->image[img].imagesizepix = guide->imagesize / 
	              Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
		      guide->camera[cam].binning;
    }
    return buff;
}

void *setboxsize(s, msgtype, name, guide, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Guide *guide;
     char *buff;
     int leng;
{
    int i;
    guide->dirty = 1;
    for ( i = 0; i < guide->nboxes; i++ ) {
	int img = guide->box[i].image-1;
	int cam = guide->image[img].camera-1;
	guide->box[i].boxsizepix = guide->boxsize / 
	              Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
		      guide->camera[cam].binning;
    }
    return buff;
}

int main(argc, argv)
	int	argc;
	char	*argv[];
{
    sigset_t        mask;
    MsgServer       m;

    Guide     guide;
    char   errorstr[256];
    char   guigeometry[256];
    int              i, j;
    char *configfile;
    char *inst;

    guigeometry[0]='\0';

    if (argc > 1) {
	configfile = argv[1];
    } else {
	configfile = NULL;
    }

    paramerract(argerr);

    memset (&guide, 0, sizeof(guide));
    memset (errorstr, 0, sizeof(errorstr));

    guide.tid 		= 0;
    guide.exposure 	= 1;
    guide.fullbin       = 1;
    guide.boxbin        = 1;

    guide.flex		= 0;

    guide.locked	= 0;
    guide.delay		= 1;
    guide.state		= 0;
    guide.count		= 0;
    strcpy(guide.guidefile, "guide");

    guide.log		= NULL;

    sigfillset(&mask);
    /* SigAction(SIGHUP , sighandler, &mask, 0);
       SigAction(SIGINT , sighandler, &mask, 0);
       SigAction(SIGQUIT, sighandler, &mask, 0);
       SigAction(SIGABRT, sighandler, &mask, 0);
     */
    SigAction(SIGPIPE, sighandler, &mask, 0);

    if ( (m = guide.guideserv = guide.S[0] = msg_server(argv[0])) == NULL ) {
	fprintf(stderr, "Server: %s\n", argv[0]);
	perror("guidserv:  Server initialization failed.");
	exit(1);
    }

    msg_publish(m, "file",                  MsgStrType, guide.guidefile, MAXBUFLEN, SetGuideFile, &guide, NULL);

    msg_publish(m, "NumberOfCameras",       MsgIntType, &guide.ncameras, 0, PublishCamera, &guide, NULL);
    msg_publish(m, "NumberOfBoxes",         MsgIntType, &guide.nboxes,   0, PublishBox,    &guide, NULL);
    msg_publish(m, "NumberOfImages",        MsgIntType, &guide.nimages,  0, PublishImage,  &guide, NULL);
    msg_publish(m, "NumberOfErrors",        MsgIntType, &guide.nerrors,  0, PublishError,  &guide, NULL);

    msg_publish(m, "ActiveBoxList",         MsgStrType, guide.activeboxlist, MAXBUFLEN, ParseActiveBoxList, &guide, NULL);
    msg_publish(m, "TransferBoxList",       MsgStrType, guide.transferboxlist, MAXBUFLEN, NULL,             &guide, NULL);
    msg_publish(m, "FullImageList",         MsgStrType, guide.fullimagelist, MAXBUFLEN, ParseFullImageList, &guide, NULL);
    
    msg_publish(m, "FullBin",            MsgIntType, &guide.fullbin,            0, setfullbin,    &guide, NULL);
    msg_publish(m, "BoxBin",             MsgIntType, &guide.boxbin,             0, setboxbin,     &guide, NULL);
    msg_publish(m, "BoxSize",            MsgDblType, &guide.boxsize,            0, setboxsize,    &guide, NULL);
    msg_publish(m, "ImageSize",          MsgDblType, &guide.imagesize,          0, setimagesize,  &guide, NULL);

    msg_publish(m, "Exposure",           MsgDblType, &guide.exposure,           0, NULL,  NULL, NULL);
    msg_publish(m, "BackgroundWidth",    MsgIntType, &guide.backgroundwidth,    0, NULL,  NULL, NULL);


    msg_publish(m, "WFSStageMin",           MsgDblType, &guide.wstagemin,           0, NULL,  NULL, NULL);
    msg_publish(m, "WFSStageMax",           MsgDblType, &guide.wstagemax,           0, NULL,  NULL, NULL);
    msg_publish(m, "wvalid",                MsgIntType, &guide.wvalid,              0, NULL,  NULL, NULL);


    msg_publish(m, "TelescopeDiameter",  MsgDblType, &guide.telescopediameter,  0, NULL,  NULL, NULL);
    msg_publish(m, "PrimaryFRatio",      MsgDblType, &guide.primaryfratio,      0, NULL,  NULL, NULL);
    msg_publish(m, "CassFRatio",         MsgDblType, &guide.cassfratio,         0, NULL,  NULL, NULL);
    msg_publish(m, "FocusUnits",         MsgDblType, &guide.focusunits,         0, NULL,  NULL, NULL);
    msg_publish(m, "LockErr",            MsgDblType, &guide.lockerr,            0, NULL,  NULL, NULL);

    msg_register(m, "quit",  			0, quit,   	NULL, NULL);
    msg_register(m, "expose",    		0, expose,	&guide, NULL);
    msg_register(m, "tick",			0, tick,	&guide, NULL);
    msg_register(m, "guidelog",			0, initlog,     &guide, NULL);
    msg_register(m, "tweakboxes",		0, tweak,       &guide, NULL);
    msg_register(m, "transfer",			0, transfer,    &guide, NULL);
    msg_register(m, "start", 		        0, start,   	&guide, NULL);
    msg_register(m, "stop", 		        0, stop,    	&guide, NULL);
    msg_register(m, "ditherinst",               0, ditherinst,  &guide, NULL);
    msg_register(m, "ditherazel",               0, ditherazel,  &guide, NULL);
    msg_register(m, "ditherradec",              0, ditherradec, &guide, NULL);
    msg_register(m, "imageoffset",              0, imageoffset, &guide, NULL);

    msg_register(m, "el_corr",                  0, el_corr,    &guide, NULL);
    msg_register(m, "az_corr",                  0, az_corr,    &guide, NULL);

    msg_publish(m, "state",			MsgIntType, &guide.state,    	   0, setlocked, &guide, NULL); 
    msg_publish(m, "send",			MsgIntType, &guide.send,    	   0, setlocked, &guide, NULL); 
    msg_publish(m, "locked",			MsgIntType, &guide.locked,         0, NULL, NULL, NULL); 
    msg_publish(m, "ngoodboxes",		MsgIntType, &guide.ngoodboxes, 	   0, NULL, NULL, NULL); 
    
    msg_publish(m, "error",			MsgStrType, errorstr    ,    	256 , NULL, NULL, NULL); 
    msg_publish(m, "GuiGeometry",		MsgStrType, guigeometry , 256, NULL, NULL, NULL); 

    msg_publish(m, "rot",			MsgDblType, &guide.rotang, sizeof(double), NULL, NULL, NULL); 
    msg_publish(m, "delay",			MsgDblType, &guide.delay,  sizeof(double), NULL, NULL, NULL); 

    msg_postval(m, "error", "OK");

    if ( !(guide.camerserv = guide.S[1] = msg_open("GUIDECAM", cinit, &guide)) ) {
	fprintf(stderr, "GUIDECAM?\n");
    }
    guide.mountserv = guide.S[2] = msg_open("TELESCOPE", NULL, NULL);
    guide.focusserv = guide.S[3] = msg_open("FOCUS", NULL, NULL);

    inst = getenv("inst");

fprintf(stderr, "INST = %s\n", inst);

    if ( !strcmp(inst, "megacam") || !strcmp(inst, "minicam")	\
		                  || !strcmp(inst,"guider") 	\
		                  || !strcmp(inst,"megaguider") ) {
        guide.shuttserv = guide.S[4] = msg_open("TOPSERV", NULL, NULL);
	if (guide.shuttserv != NULL) {
	    msg_subscribe(guide.shuttserv, "gvalid", MsgIntType, &guide.gvalid, sizeof(int), 0.0, NULL, NULL, 1.0);
    	} else {
	    guide.gvalid = 1;
    	}
    } else {
	guide.gvalid = 1;
    }


    guide.wavefserv = guide.S[5] = msg_open("WAVESERV", NULL, NULL);
    guide.wstagepos=-9999.9;
    if (guide.wavefserv != NULL) {
        msg_subscribe(guide.wavefserv, "tA", MsgDblType, &guide.wstagepos, sizeof(double), 0.5,  wvalidate, &guide, 3.0);
    } else {
        guide.wvalid = 1;
    }
    fprintf(stderr,"wstagepos: %f",guide.wstagepos);
    if( guide.wstagepos <= -9999.0 )
		guide.wvalid=1;
    fprintf(stderr," wvalid: %d\n",guide.wvalid);

    msg_subscribe(guide.mountserv, "inpos", MsgIntType, &guide.inpos,  sizeof(int),    0.1, NULL, NULL, 1.0);
    msg_subscribe(guide.mountserv, "rot"  , MsgDblType, &guide.rotang, sizeof(double), 0.1, NULL, NULL, 1.0);
    msg_subscribe(guide.mountserv, "posang",MsgDblType, &guide.posang, sizeof(double), 0.1, NULL, NULL, 1.0);
    msg_subscribe(guide.mountserv, "el"   , MsgDblType, &guide.eleang, sizeof(double), 0.1, NULL, NULL, 1.0);

    msg_up(m, NULL);
    /* msg_waitloop(guide.S, 5, 0, 0, 1.0); */

    fprintf(stderr, "Guide exits %d\n", msg_waitloop(guide.S, 6, 0, 0, -1.0));
    perror("guide");
}

