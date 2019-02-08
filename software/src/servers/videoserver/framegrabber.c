/* framegrabber.c
 */

#include <except.h>
#include <xos.h>
#include <xfile.h>
#include <msg.h>
#include <fitsy.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <pthread.h>

#include <math.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>

extern int videoserver (char *h, unsigned char *d);
static unsigned char *nextFrame(unsigned char *data) {
    int status;
    char *host = getenv("VIDEOSERVER");
    if (host == NULL)
        host = "videoserver1.mmto.arizona.edu";
    if (status = videoserver (host, data)) {
	/* fprintf (stderr, "vs = %d\n", status); */
	return (NULL);
    }
    return (data);
}

#include "iproc.h"

void *framegrabber(iproc)
	IProc	iproc;
{

	AccBox	*box;
	int	xbox, j;

	int		   frame = 0;

	double	mmxone;

	unsigned char	*video;
        Calloc(video, iproc->X * iproc->Y);

    iproc->blocking = 1;

    Calloc(box, sizeof(struct _AccBox) * iproc->naccum);
    MMX(mmxone, 1);


    /* Allow this thread to be canceled
     */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    for ( j = 0; j < iproc->naccum; j++ ) {
    	box[j] = iproc->box[j];
    }

    for ( j = 0; 1; j++ ) {
	int	ncmds;


	/* pthread_testcancel(); */

	for ( ncmds = 0; handle1command(iproc, box); ncmds++ ) ;

	if ( iproc->idle ) {
		SAOusleep(0.250);
		iproc->pixel   = iproc->fakepix;
	} else {

	    if ( iproc->blocking ) alarm(1);
	    if (!(iproc->pixel = nextFrame(video)) ) {

		/* printf("No Frame %d\n", iproc->N); */
		if ( iproc->blocking ) {
		    iproc->blocking = 0;
		    alarm(0);
		}
		iproc->pixel   = iproc->fakepix;
		SAOusleep(0.300);
	    }
	    if ( iproc->blocking ) 	alarm(0);
	    else {
		if ( iproc->pixel != iproc->fakepix ) {
		    iproc->blocking = 1;
		}
	    }

	    iproc->frameno = frame++;
	    /* iproc->time   = ?; */
	}

#define ACCOFF box[xbox].ay1*iproc->X + box[xbox].ax1

	/* Run the designated type of image processing
	 */
	for ( xbox = 0; xbox < iproc->naccum; xbox++ ) {
	    if ( !box[xbox].accum ) continue;

	    switch ( box[xbox].acctype ) {
	     case ProcXAdd:
		    processxadd(
			      &iproc->pixel[ACCOFF]
			    , &box[xbox].buffr[box[xbox].N 
					     * box[xbox].anx
					     * box[xbox].any]
			    , &iproc->box[xbox].accum[ACCOFF]
			    , box[xbox].anx
			    , box[xbox].any
			    , iproc->X);

		    if ( box[xbox].init <= box[xbox].N )    box[xbox].init++;
		    if ( ++box[xbox].N == box[xbox].frac ) box[xbox].N = 0;

		    break;

	     case ProcAdd:
		    process_add(
			      &iproc->pixel[ACCOFF]
			    , &box[xbox].buffr[box[xbox].N 
					     * box[xbox].anx
					     * box[xbox].any]
			    , &iproc->box[xbox].accum[ACCOFF]
			    , box[xbox].anx
			    , box[xbox].any
			    , iproc->X);

		    if ( box[xbox].init <= box[xbox].N )    box[xbox].init++;
		    if ( ++box[xbox].N == box[xbox].frac ) box[xbox].N = 0;

		    break;
	    }
	}


	for ( xbox = 0; xbox < iproc->naccum; xbox++ ) {
		int	done = 0;

		CenCmp  cenbox;
		int	i;

		double	dox = iproc->cen[xbox].x;
		double	doy = iproc->cen[xbox].y;

		int	cy1 = box[xbox].cy1;
		int	cx1 = box[xbox].cx1;

		int	rawsum;
		int	biassum;
		double	biasscale;

		double	Xw, Yw, XXw, YYw, FWHM;

	    cenbox.x = -1.0;
	    cenbox.y = -1.0;


#define CENOFF cy1*iproc->X + cx1

	 if ( box[xbox].iter
	   && box[xbox].cnx && box[xbox].cny ) {
			int CX, CY, Max;


		if ( box[xbox].Bkpix != 0 )
		    cenbox.bkg = (double) cenbox.bksum / box[xbox].Bkpix;

		{
		    quickcenter(&iproc->box[xbox].accum[CENOFF]
				, box[xbox].cnx, box[xbox].cny
				, iproc->X
				, &CX, &CY);

		    CX += cx1;
		    CY += cy1;

		    cx1 = Clip(box[xbox].bx1, (box[xbox].bx1 + box[xbox].bnx) - box[xbox].cnx/2
				, CX - box[xbox].cnx/2);
		    cy1 = Clip(box[xbox].by1, (box[xbox].by1 + box[xbox].bny) - box[xbox].cny/2
				, CY - box[xbox].cny/2);
		}

		/* Make two passes to compute Bksum/biasscale for this frame
		 */
		biasscale = 1.0;
		for ( i = 0; i < 2; i++ ) {
		    center__c(&iproc->box[xbox].accum[CENOFF]
			    , box[xbox].cnx
			    , box[xbox].cny
			    , iproc->X
			    , iproc->box[xbox].maskx
			    , box[xbox].Bkrad
			    , box[xbox].Bkrad - box[xbox].Bkwid
			    , 0
			    , &CX, &CY
			    , iproc->cen[xbox].bkg
			    , &cenbox.sum, &Xw, &Yw, &XXw, &YYw, &FWHM
			    , &cenbox.bksum, &cenbox.min, &cenbox.max
			    , &iproc->biasx[CENOFF]
			    , biasscale, &rawsum, &biassum);

		    
			if ( biassum != 0 ) 
			    biasscale = rawsum/ (double) biassum;
		}

		/* fprintf(stderr, "Proc%d  Box%d  %f\n", iproc->N, xbox, biasscale); */

		if ( box[xbox].Bkpix != 0 )
		    cenbox.bkg = (double) cenbox.bksum / box[xbox].Bkpix;

	  for ( i = 0; i < box[xbox].iter; i++ ) {
		center__c(&iproc->box[xbox].accum[CENOFF]
			, box[xbox].cnx
			, box[xbox].cny
			, iproc->X
			, iproc->box[xbox].maskx
			, box[xbox].Bkrad
			, box[xbox].Bkrad - box[xbox].Bkwid
			, box[xbox].radius
			, &cenbox.x, &cenbox.y
			, cenbox.bkg
			, &cenbox.sum, &Xw, &Yw, &XXw, &YYw, &FWHM
			, &cenbox.bksum, &cenbox.min, &cenbox.max
			, &iproc->biasx[CENOFF]
			, biasscale, &rawsum, &biassum);

		cenbox.x += cx1;
		cenbox.y += cy1;

		cx1 = Clip(box[xbox].bx1, box[xbox].bx1 + box[xbox].bnx - box[xbox].cnx/2
				, (int)cenbox.x - box[xbox].cnx/2);
		cy1 = Clip(box[xbox].by1, box[xbox].by1 + box[xbox].bny - box[xbox].cny/2
				, (int)cenbox.y - box[xbox].cny/2);

		if ( biassum != 0 ) 
		   biasscale = rawsum/ (double) biassum;

	      if ( box[xbox].Bkpix != 0 )
		cenbox.bkg = (double) cenbox.bksum / box[xbox].Bkpix;

	     if ( dox == cenbox.x
	       && doy == cenbox.y ) {
			/* break; */
	     } else {
		    dox = cenbox.x;
		    doy = cenbox.y;
             }
	  }
  	  iproc->box[xbox].biasscale = biasscale;
  	  box[xbox].biasscale = biasscale;

	  if ( iproc->pixel == iproc->fakepix ) {
	      cenbox.x += drand48()/1000;
	      cenbox.y += drand48()/1000;
	  }

	  cenbox.frame = iproc->frameno;
	  cenbox.time = iproc->time;
	  pthread_mutex_lock(&iproc->cenlock);

		iproc->cen[xbox] = cenbox;
	  pthread_mutex_unlock(&iproc->cenlock);
	}
      }
    }
}

