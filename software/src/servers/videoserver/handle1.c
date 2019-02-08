/* handle1.c
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

#include "iproc.h"

#define BOXOFF box[xbox].by1*iproc->X + box[xbox].bx1

allocbox(iproc, box)
	IProc	iproc;
    	AccBox	*box;
{
    Calloc(box->accum, iproc->X * iproc->Y * sizeof(short));
}


int handle1command(iproc, box)
	IProc	 iproc;
	AccBox	*box;
	
{
    int handled = 0;

    do {
	if ( !pthread_mutex_trylock(&iproc->cmdlock) ) {
	    int	xbox = iproc->xbox;

	    handled = 1;

	    iproc->time_h   = iproc->time;
	    iproc->frameno_h = iproc->frameno;

	    switch ( iproc->command ) {

	     case FLUSH:
/*
		if ( iproc->dt ) 
		    dt3155_flush(iproc->dt);
 */
	        handled = 0;
		break;

	     case START:	iproc->stopped = 0; handled = 0; break;
	     case STOP:		iproc->stopped = 1; handled = 0; break;
	     case RESUME:	iproc->idle    = 0; handled = 0; break;
	     case IDLE:		iproc->idle    = 1; handled = 0; break;
	     case SMOOTH:
	        box[xbox].smooth = iproc->box[xbox].smooth;
		break;
	     case GETPIXEL:
		if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
	        if ( xbox == -1 ) {
		    blitpix(iproc->data, iproc->pixel
			    , iproc->X
			    , iproc->Y
			    , iproc->X
			    , iproc->X);
		} else {
		    blitpix(iproc->data, &iproc->pixel[BOXOFF]
			    , box[xbox].bnx
			    , box[xbox].bny
			    , iproc->X
			    , box[xbox].bnx);
		}
		handled = 0;
		break;
	     case GETBOXAC:
		if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
		blitwrd(iproc->data
			, &iproc->box[xbox].accum[BOXOFF]
			, box[xbox].bnx
			, box[xbox].bny
			, iproc->X
			, box[xbox].bnx);

		 iproc->box[xbox].init = box[xbox].init;
		break;

	     case GETPROCED: {
			unsigned short *biasp;
			double		biasscale = 0.0;
			int	box0;
			int	boxN;

		if ( xbox == -1 ) {
			box0 = 0;
			boxN = iproc->nbox;
		} else {
			box0 = xbox;
			boxN = xbox + 1;
		}

		for ( xbox = box0; xbox < boxN; xbox++ ) {
		    if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
		    if ( iproc->box[xbox].smooth ) {
			    biasp = iproc->biass;
		    } else {
			    biasp = iproc->biasx;
		    }
			
		    if ( box[xbox].iter
		      && box[xbox].cnx && box[xbox].cny ) {
			biasscale = box[xbox].biasscale;
		    } else {
			if ( box[xbox].bscalefrom ) {
			    biasscale = box[box[xbox].bscalefrom].biasscale;
			}
		    }

		    procwrd(iproc->box[xbox].data
			    , &iproc->box[xbox].accum[BOXOFF]
			    , &biasp[BOXOFF]
			    ,  biasscale
			    ,  box[xbox].smooth
			    , box[xbox].bnx
			    , box[xbox].bny
			    , iproc->X
			    , box[xbox].bnx);

		     iproc->box[xbox].init = box[xbox].init;
		}
		break;
	     }

	     case GETBIAS:
		blitwrd(iproc->data, iproc->biasx
			, iproc->X, iproc->Y, iproc->X, iproc->X);
		break;
	     case SETBIAS:
		blitwrd(iproc->biasx, iproc->data
			, iproc->X, iproc->Y, iproc->X, iproc->X);
		smooth(iproc->biasx, iproc->biass, iproc->X, iproc->Y
						 , iproc->X, iproc->X);

		break;
	     case GETFLAT:
		if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
		blitwrd(iproc->data, iproc->flatx
			, box[xbox].bnx
			, box[xbox].bny
			, iproc->X
			, box[xbox].bnx);
		break;
	     case SETFLAT:
		if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
		blitwrd(iproc->flatx, iproc->data
			, box[xbox].bnx
			, box[xbox].bny
			, iproc->X
			, box[xbox].bnx);
		break;

	     case BOX_COMMAND:
		{
			int changed = 0;

		if ( iproc->box[xbox].buffr == NULL
		  || box[xbox].ax1     != iproc->box[xbox].ax1
		  || box[xbox].ay1     != iproc->box[xbox].ay1
		  || box[xbox].anx     != iproc->box[xbox].anx 
		  || box[xbox].any     != iproc->box[xbox].any 
		  || box[xbox].frac    != iproc->box[xbox].frac ) {
			changed = 1;
		} 
		if ( changed
		  || box[xbox].acctype != iproc->box[xbox].acctype ) {
		    iproc->box[xbox].N    = 0;
		    iproc->box[xbox].init = 0;
		} else {
		    iproc->box[xbox].N    = box[xbox].N;
		    iproc->box[xbox].init = box[xbox].init;
		}

		if ( changed ) {

		    if ( !iproc->box[xbox].accum) allocbox(iproc, &iproc->box[xbox]);
		    if (  iproc->box[xbox].buffr ) Free(iproc->box[xbox].buffr);

		    memset(iproc->box[xbox].accum, 0, iproc->X * iproc->Y * sizeof(short));

		    if ( iproc->box[xbox].anx 
		       * iproc->box[xbox].any
	 	       * iproc->box[xbox].frac != 0 ) {
		      Calloc(iproc->box[xbox].buffr
			, iproc->box[xbox].anx
			* iproc->box[xbox].any
			* iproc->box[xbox].frac);
		    }
		}

		if ( box[xbox].Bkrad != iproc->box[xbox].Bkrad
		  || box[xbox].Bkwid != iproc->box[xbox].Bkwid ) {
		    /* Compute new mask
		     */
			int bkrad = iproc->box[xbox].Bkrad;
			int bkwid = iproc->box[xbox].Bkwid;

			unsigned char *mask;
			int ix;
			int iy;
			
			int cw = bkrad * 2;
			int ch = bkrad * 2;

			int rSq =  sqrt(bkrad        *  bkrad);
			int rBk = sqrt((bkrad-bkwid) * (bkrad-bkwid));

			int bkpix = 0;


		    if ( iproc->box[xbox].maskx ) Free(iproc->box[xbox].maskx);

		    Calloc(iproc->box[xbox].maskx, cw * ch);
 		    mask = iproc->box[xbox].maskx;

		    for ( iy = 0; iy < ch; iy++ ) {
			for ( ix = 0; ix < cw; ix++ ) {
				int r = (iy-ch/2)*(iy-ch/2) + (ix-cw/2)*(ix-cw/2);

			    if ( r > 255*255 ) *mask = 255;
			    else {	  
				*mask = (int) sqrt((double)r + 0.5);
				if ( *mask <= rSq && *mask >= rBk ) bkpix++;
			    }

			    mask++;
			}
		    }

		    iproc->box[xbox].Bkpix = bkpix;
		} else {
		    iproc->box[xbox].Bkpix = box[xbox].Bkpix;
		}

	        box[xbox] = iproc->box[xbox];

		handled = 0; 
		break;
	     }


	     default:
		handled = 0;
		break;
	    }

	    if ( iproc->command ) {
		iproc->command = 0;

		pthread_cond_signal(&iproc->cmddone);
	    }
	    pthread_mutex_unlock(&iproc->cmdlock);
	}
	if ( iproc->stopped ) {
		usleep(30);
	}
    } while ( iproc->stopped );

    return handled;
}
