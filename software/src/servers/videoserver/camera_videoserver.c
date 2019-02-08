
void headsimple(void *mem
	, int x1
	, int y1
	, int x
	, int y
	, double xzero
	, double yzero
	, double xscale
	, double yscale
	, char	*xunits
	, char	*yunits
	, int bitpix
	, int frame
	, int frac);

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

#include "iproc.h"

extern void *framegrabber();
/* extern unsigned long long stamp(); */

#define CheckArgs(argc, argv, narg, cmd, str)			\
    if ( argc != narg+1 ) {					\
	msg_nak(c, msgid, "%s takes %d args: %s %s", cmd, narg, cmd, str);	\
	return;							\
    }

#define CheckPrc(procs, prc)					\
    if ( prc < 0 || prc > procs ) {				\
	msg_nak(c, msgid, "image processor must be 0..%d", procs);	\
	return;							\
    }

#define CheckBox(boxes, prc)					\
    if ( prc < 0 || prc > boxes ) {				\
	msg_nak(c, msgid, "image box must be 0..%d", boxes);	\
	return;							\
    }



void ACK(s, c, msgid, iproc, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        IProc            iproc;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 1, "ACK", "acknoledgement");
	FPrint(c->out, "%s\n", argv[1]);
	Flush(c->out);
}

void stop(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 1, "stop", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    SendCommand(, snappy->iproc[n], STOP);
	}

}

void start(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 1, "start", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    SendCommand(
		    , snappy->iproc[n], START);
	}
}

void iidle(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 1, "stop", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    SendCommand(
		    , snappy->iproc[n], IDLE);
	}
}

void resume(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 1, "stop", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    SendCommand(
		    , snappy->iproc[n], RESUME);
	}
}

void snapacc(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        IProc            iproc = snappy->iproc;
	
	CheckArgs(argc, argv, 2, "snapacc", "iproc box");
	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);
  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(iproc[n].nbox, b);

	    if ( iproc[n].box[b].accum == NULL ) {
		msg_nak(c, msgid, "No Accumulator %d\n", b);
	    }

	    headsimple(iproc[n].head
		, iproc[n].box[b].bx1
		, iproc[n].box[b].by1
		, iproc[n].box[b].bnx
		, iproc[n].box[b].bny
		     , iproc[n].box[b].xzero
		     , iproc[n].box[b].yzero
		     , iproc[n].box[b].xscale
		     , iproc[n].box[b].yscale
		     , iproc[n].box[b].xunits
		     , iproc[n].box[b].yunits
		, 16, iproc->frameno_h
		, iproc[n].box[b].frac);


	    if ( ft_byteswap() ) {
		bswap(iproc[n].data, iproc[n].data
		    , iproc[n].box[b].bnx
		    * iproc[n].box[b].bny
		    * 2);
	    } 
	msg_ack(c, msgid, NULL);
	}
}


void setproc(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 3, "setproc", "iproc box type");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);
	    int type;
  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    switch ( *argv[3] ) {
		case 'n' : type = ProcNone;	break;
		case 'c' : type = ProcCopy;	break;
		case 'a' : type = ProcAccum;	break;
		case 'r' : type = ProcReduc;	break;
		case 's' : type = ProcC;	break;
		case 'p' : type = ProcXAdd;	break;
		case 'P' : type = ProcAdd;	break;
		case 'x' : type = ProcXCpy;	break;
		default:
			msg_nak(c, msgid, "setproc: bad proc type must be one of \"ncarsx\"");
	    }

	    SendCommand(
		snappy->iproc[n].box[b].acctype = type;

		, snappy->iproc[n], BOX_COMMAND);
	}
	msg_ack(c, msgid, NULL);
}

void setchan(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "setchan", "iproc chan");
	{
	    int n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);
	    if ( snappy->iproc[n].dt ) {
		SetVideoChannel(snappy->iproc[n].dt, atoi(argv[2]));
		SetSyncChannel(snappy->iproc[n].dt, atoi(argv[2]));
	    }
	}
	msg_ack(c, msgid, NULL);
}

void setwhite(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "setwhite", "iproc level");
	{
	    int n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);
	    if ( snappy->iproc[n].dt ) SetWhiteLevel(snappy->iproc[n].dt, atoi(argv[2]));
	}
	msg_ack(c, msgid, NULL);
}

void setblack(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "setblack", "iproc level");
	{
	    int n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);
	    if ( snappy->iproc[n].dt) SetBlackLevel(snappy->iproc[n].dt, atoi(argv[2]));
	}
	msg_ack(c, msgid, NULL);
}

	
void backgr(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 3, "backgr", "iproc box width");
}

void getfrac(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "getfrac", "iproc b");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);
  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, "%d", snappy->iproc[n].box[b].frac);
	}
}

void getscale(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "getsample", "iproc box");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);
  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, "%f %f"
		    , snappy->iproc[n].box[b].yscale
		    , snappy->iproc[n].box[b].xscale);
	}
}
void getzero(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "getsample", "iproc box");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);
  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, "%f %f"
		    , snappy->iproc[n].box[b].yzero
		    , snappy->iproc[n].box[b].xzero);
	}
}
void getsam(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 2, "getsample", "iproc box");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, "%d %f %d"
		    , snappy->iproc[n].box[b].A
		    , snappy->iproc[n].box[b].T
		    , snappy->iproc[n].box[b].nK);
	}
}

void setsam(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        CheckArgs(argc, argv, 5, "setsample", "iproc box A tol ntol");

	{
	    int n = atoi(argv[1]);
	    int b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    snappy->iproc[n].box[b].A  = atoi(argv[3]);
	    snappy->iproc[n].box[b].T  = atof(argv[4]);
	    snappy->iproc[n].box[b].nK = atof(argv[5]);

	    msg_ack(c, msgid, NULL);
	}
}


void getcen(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
        IProc            iproc = snappy->iproc;
	CenCmp	cenbox;
	int	I, i, box;
	int	a = 0, n, N, A, nK;
 
	double  x[256];
	double	y[256];
	double  cx;
	double  cy;
	double  cxsq;
	double  cysq;
	double  cxsm;
	double  cysm;
	double 	T;


	int	reject = 0;
	int	quick  = 0;
	int	X = 0;

        CheckArgs(argc, argv, 3, "getcen", "image N A");

	I   = atoi(argv[1]) - 1;
	i   = snappy->image[I].processor;
	box = snappy->image[I].accum;

	N   = atoi(argv[2]);
	A   = atoi(argv[3]);

  	CheckPrc(snappy->niproc, i);
  	CheckBox(iproc[i].nbox, box);

	if ( N == 0 ) {
		quick = 1;
		N     = 1;
	}

	T  = iproc[i].box[box].T;
	nK = iproc[i].box[box].nK;

	if ( !quick ) {
	    msg_ack(c, msgid, NULL);
	    if ( A > 1 ) {
		fprintf(c->out, "PX\tPY\tMin\tMax\tBk\tRMSPX\tRMSPY\tRMSPR\tTol\tFrame\tTime\n");
		fprintf(c->out, "--\t--\t---\t---\t--\t-----\t-----\t-----\t---\t-----\t----\n");
	    } else {
		fprintf(c->out, "PX\tPY\tMin\tMax\tBk\tFrame\tTime\n");
		fprintf(c->out, "--\t--\t---\t---\t--\t-----\t----\n");
	    }
	}

	if ( A < 0 ) {
		reject = 1;
		A = -A;
	}

        for ( n = 4; n--; ) {
	    SendCommand(
		iproc[i].xbox = box; , iproc[i], NOP);
	    GetCenter(&cenbox, &iproc[i], box);
	}

	if ( T != 0.0 ) {
		int    K = 0;

	    GetCenter(&cenbox, &iproc[i], box);

	    do {
		cx = cenbox.x;
		cy = cenbox.y;

		SendCommand(iproc[i].xbox = box;, iproc[i], NOP);
	        GetCenter(&cenbox, &iproc[i], box);

		if ( (cenbox.x - cx) * (cenbox.x - cx)
		   + (cenbox.y - cy) * (cenbox.y - cy) < T*T )
		     K++;
		else K = 0;

		X++;
	    } while ( X < 30 * 30 && K < nK );

	    x[a] = cx;
	    y[a] = cy;

	    a    = 1;
	    cxsm = cx;
	    cysm = cy;
	    cxsq = cx * cx;
	    cysq = cy * cy;
	} else {
		int	k;

	    for ( k = iproc[i].box[box].frac; k--; ) {
	        SendCommand(iproc[i].xbox = box;, iproc[i], NOP);
	    }
	    GetCenter(&cenbox, &iproc[i], box);

	    a = 0;
	    cxsm = 0;
	    cysm = 0;
	    cxsq = 0;
	    cysq = 0;
	}

	for ( n = 0; n < N; n++ ) {
	    for ( ; a < A; a++ ) {
		x[a]  = cenbox.x;
		y[a]  = cenbox.y;

		cxsm += cenbox.x;
		cysm += cenbox.y;
		cxsq += cenbox.x * cenbox.x;
		cysq += cenbox.y * cenbox.y;

		if ( a < A-1 || n < N-1 || (A == 1 && N != 1 ) ) {
			int	k;

		    for ( k = iproc[i].box[box].frac; k--; ) {
			SendCommand(iproc[i].xbox = box;, iproc[i], NOP);
		    }
		    GetCenter(&cenbox, &iproc[i], box);
		}
	    }

	    if ( A > 1 ) {
			int j = A;
			double rmsx, rmsy, rmsr;
			double sigx, sigy;

		cx = cxsm / j;
		cy = cysm / j;

		sigx = sqrt((cxsq - cx * cxsm) / (j - 1));
		sigy = sqrt((cysq - cy * cysm) / (j - 1));

		if ( reject ) {
		    /* Pass 1 reject at 2.5 sigma
		     */
		    j = 0;
		    cxsm = 0;
		    cysm = 0;
		    cxsq = 0;
		    cysq = 0;
		    for ( a = 0; a < A; a++ ) {

			if ( Abs(cx - x[a]) < 2.5*sigx
			  && Abs(cy - y[a]) < 2.5*sigy ) {
			    j++;
			    cxsm += x[a];
			    cysm += y[a];
			    cxsq += x[a] * x[a];
			    cysq += y[a] * y[a];

			} /* else {
			    fprintf(stderr, "1 Reject %f %f	(%f) (%f)\n", Abs(cx - x[a]), Abs(cy - y[a]), sigx, sigy);
			}  */
		    }

		    cx = cxsm / j;
		    cy = cysm / j;
		    sigx = sqrt((cxsq - cx * cxsm) / (j - 1));
		    sigy = sqrt((cysq - cy * cysm) / (j - 1));

		    /* Pass 2 reject at 15 sigma
		     */
		    j = 0;
		    cxsm = 0;
		    cysm = 0;
		    cxsq = 0;
		    cysq = 0;
		    for ( a = 0; a < A; a++ ) {

			if ( Abs(cx - x[a]) < 15*sigx
			  && Abs(cy - y[a]) < 15*sigy ) {
			    j++;
			    cxsm += x[a];
			    cysm += y[a];
			    cxsq += x[a] * x[a];
			    cysq += y[a] * y[a];

			} else {
			    fprintf(stderr, "2 Reject %f %f	(%f) (%f)\n", Abs(cx - x[a]), Abs(cy - y[a]), sigx, sigy);
			} 
		    }

		    X = j;	/* Number of points accepted	*/
		}

		cx = cxsm / j;
		cy = cysm / j;

		rmsx = sqrt((cxsq - 2 * cx * cxsm + j * cx * cx) /(j-1));
		rmsy = sqrt((cysq - 2 * cy * cysm + j * cy * cy) /(j-1));
		rmsr = sqrt(rmsx * rmsx + rmsy * rmsy);


/* ADD One to the computed Framegrabber center to get to IMAGE coordinates
   All position IO is done in IMAGE coords.
 */
		if ( quick ) {
		    msg_ack(c, msgid, "%7.3f\t%7.3f\t%d\t%d\t%7.3f\t%7.3f\t%7.3f\t%7.3f\t%d\t%d\t%Lu"
			    , cx + 1, cy + 1
			    , cenbox.min, cenbox.max, cenbox.bkg
			    , rmsx, rmsy, rmsr, X
			    , cenbox.frame, cenbox.time);
		} else {
		    fprintf(c->out, "%7.5f\t%7.5f\t%d	%d\t%7.5f\t%7.5f\t%7.5f\t%7.5f\t%d\t%d\t%Lu\n"
			    , cx + 1, cy + 1
			    , cenbox.min, cenbox.max, cenbox.bkg
			    , rmsx, rmsy, rmsr, X
			    , cenbox.frame, cenbox.time);
		}
	    } else {
		if ( quick ) {
		    msg_ack(c, msgid, "%7.5f\t%7.5f\t%d\t%d\t%7.5f\t%d\t%Lu"
			    , cenbox.x + 1, cenbox.y + 1
			    , cenbox.min, cenbox.max, cenbox.bkg
			    , cenbox.frame, cenbox.time);
		} else {
		    fprintf(c->out, "%7.5f\t%7.5f\t%d\t%d\t%7.5f\t%d\t%Lu\n"
			    , cenbox.x + 1, cenbox.y + 1
			    , cenbox.min, cenbox.max, cenbox.bkg
			    , cenbox.frame, cenbox.time);
		}
	    }

	    a    = 0;
	    cxsm = 0;
	    cysm = 0;
	    cxsq = 0;
	    cysq = 0;
	}

	if ( !quick ) fprintf(c->out, "\f\n");
	fflush(c->out);
}

dosetacc(iproc, n, b, x, y, bw, bh)
	IProc	iproc;
	int	n;
	int	b;
	int	x;
	int	y;
	int	bw;
	int	bh;
{
	    int aw, ah;

	    x -= 1;
	    y -= 1;

	    aw = ((bw / 8 ) + 1 ) * 8;
	    ah = bh;

	    SendCommand(

		iproc[n].xbox = b;

		iproc[n].box[b].anx = Clip(0, 640, aw);
		iproc[n].box[b].any = Clip(0, 480, ah);
		iproc[n].box[b].ax1 = Clip(0, iproc[n].X - aw, x - (aw/2 - 1));
		iproc[n].box[b].ay1 = Clip(0, iproc[n].Y - ah, y - (ah/2 - 1));

	if ( iproc[n].box[b].ax1 < 0 ) iproc[n].box[b].ax1 = 0;
	if ( iproc[n].box[b].ay1 < 0 ) iproc[n].box[b].ay1 = 0;

		iproc[n].box[b].bnx = Clip(0, iproc[n].box[b].anx, bw);
		iproc[n].box[b].bny = Clip(0, iproc[n].box[b].any, bh);
		iproc[n].box[b].bx1 = Clip(0, iproc[n].X - iproc[n].box[b].bnx
					, x - (iproc[n].box[b].bnx/2 -1));
		iproc[n].box[b].by1 = Clip(0, iproc[n].Y - iproc[n].box[b].bny
					, y - (iproc[n].box[b].bny/2 -1));

	if ( iproc[n].box[b].bx1 < 0 ) iproc[n].box[b].bx1 = 0;
	if ( iproc[n].box[b].by1 < 0 ) iproc[n].box[b].by1 = 0;

		, iproc[n], BOX_COMMAND);

}

dosetbox(iproc, n, b, x, y, iter, rad, bkrad, bkwidth, bscalefrom)
	IProc	iproc;
	int	n;
	int	b;
	double	x;
	double	y;
	int	iter;
	int	rad;
	int	bkrad;
	int	bkwidth;
	int	bscalefrom;
{
	    x -= 1;
	    y -= 1;

/* There are an even number of pixels on the sub image used to compute 
 * the centroid.  The center of the sub image is on the boundary between
 * four pixels.  For a 200x200 sub image the center point is at 100.5, 100.5.
 * To place this sub image center on the image framebuffer within a half
 * pixel of the desired centroid starting coordinate, compute cx1 and cy1:
 *
 * x is the desired x centroid center coordinate on the full framebuffer
 * y is the desired y centroid center coordinate on the full framebuffer
 *
 * cnx, cny are the the centroid sub image height and width.
 *
 * cx1 and cy1 are the centroid sub image starting pixel in full framebuffer image 
 * coords.  This is used by the framegrabber code to select the centroid input box
 * in framegrabber.c
 *
 *   cx1 = (int) (x - (cnx/2 - 1))
 *   cy1 = (int) (y - (cny/2 - 1))
 */
	    SendCommand( iproc[n].xbox = b;

		iproc[n].box[b].cnx = Clip(0, iproc[n].box[b].anx, Max(rad * 2, bkrad * 2));
		iproc[n].box[b].cny = Clip(0, iproc[n].box[b].any, Max(rad * 2, bkrad * 2));
		iproc[n].box[b].cx1 = Clip(iproc[n].box[b].bx1
					 , (iproc[n].box[b].bx1 + iproc[n].box[b].bnx) - iproc[n].box[b].cnx
					 , x - (iproc[n].box[b].cnx/2 - 1));
		iproc[n].box[b].cy1 = Clip(iproc[n].box[b].by1
					 , (iproc[n].box[b].by1 + iproc[n].box[b].bny) - iproc[n].box[b].cny
					 , y - (iproc[n].box[b].cny/2) - 1);

		iproc[n].box[b].radius  = rad;
		iproc[n].box[b].Bkrad   = bkrad;
		iproc[n].box[b].Bkwid   = bkwidth;
		iproc[n].box[b].iter    = iter;

		iproc[n].box[b].bscalefrom  = bscalefrom;

		, iproc[n], BOX_COMMAND);

	iproc[n].box[b].cx = x;
	iproc[n].box[b].cy = y;
}

void setacc(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    if ( argc != 5 && argc != 7 ) {
	CheckArgs(argc, argv, 10, "setacc", "iproc box x y [w h]");
    }

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

	    int x  	  = atoi(argv[3]);
	    int y  	  = atoi(argv[4]);

	    int bw	  = snappy->iproc[n].box[b].bnx;
	    int bh	  = snappy->iproc[n].box[b].bny;

	  if ( argc == 7 ) {
	    bw  	  = atoi(argv[5]);
	    bh  	  = atoi(argv[6]);
	  } 
	    

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    dosetacc(snappy->iproc, n, b, x, y, bw, bh);
	}
	msg_ack(c, msgid, NULL);
}

void setbox(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    if ( argc != 5 && argc != 9 ) {
	CheckArgs(argc, argv, 9, "setbox", "iproc box x y [iter radius bkrad bkwidth]");
    }

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

	    double x  	  = atof(argv[3]);
	    double y  	  = atof(argv[4]);

	    int	iter	  = snappy->iproc[n].box[b].iter;
	    int rad	  = snappy->iproc[n].box[b].radius;
	    int bkrad	  = snappy->iproc[n].box[b].Bkrad;
	    int bkwid	  = snappy->iproc[n].box[b].Bkwid;
	    int bsfrom	  = snappy->iproc[n].box[b].bscalefrom;

	  if ( argc == 10 ) {
	    iter      = atoi(argv[5]);
	    rad       = atoi(argv[6]);
	    bkrad     = atoi(argv[7]);
	    bkwid     = atoi(argv[8]);
	  } 
	    

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    dosetbox(snappy->iproc, n, b, x, y, iter, rad, bkrad, bkwid, bsfrom);
	}
	msg_ack(c, msgid, NULL);
}


void setfrac(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    	IProc	iproc = snappy->iproc;

	CheckArgs(argc, argv, 3, "setfrac", "iproc box n");

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);
	    int f = atoi(argv[3]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(iproc[n].nbox, b);

	    if ( f < 1   ) f = 1;
	    if ( f > 255 ) f = 255;


	    SendCommand(
		iproc[n].xbox = b;
	        iproc[n].box[b].frac = f;
		MMX(iproc[n].box[b].fracx, FRAC((f-1.0)/f));

		, iproc[n], BOX_COMMAND);
	}
	msg_ack(c, msgid, NULL);
}

void setzero(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 4, "setzero", "iproc box xzero yzero");

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

		snappy->iproc[n].box[b].xzero = atof(argv[4]);
		snappy->iproc[n].box[b].yzero = atof(argv[3]);
	}
	msg_ack(c, msgid, NULL);
}

void getunits(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 2, "getunits", "iproc box");

	{
	    char	reply[128];

	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

		sprintf(reply, "%s	%s"
			, snappy->iproc[n].box[b].yunits 
			, snappy->iproc[n].box[b].xunits);
		msg_ack(c, msgid, reply);
	}
}
void setunits(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 4, "setunits", "iproc acc xunits	yunits");

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

		snappy->iproc[n].box[b].xunits = strdup(argv[4]);
		snappy->iproc[n].box[b].yunits = strdup(argv[3]);
	}
	msg_ack(c, msgid, NULL);
}

void setscale(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 4, "setscale", "iproc box xscale yscale");

	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

		snappy->iproc[n].box[b].xscale = atof(argv[4]);
		snappy->iproc[n].box[b].yscale = atof(argv[3]);
	}
	msg_ack(c, msgid, NULL);
}

void clrpix(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

	CheckArgs(argc, argv, 1, "clrpix", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    memset(iproc[n].fakepix, 0, 640 * 480);
	}

}

void setpix(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

	CheckArgs(argc, argv, 1, "setpix", "iproc");
	{
	    int	n = atoi(argv[1]);
  	    CheckPrc(snappy->niproc, n);

	    SendCommand(
		    , iproc[n], STOP);

	    fits = ft_headread(c->inp);
	           ft_dataread(c->inp, fits, iproc[n].fakepix, 8);

	    SendCommand( , iproc[n], START);
	}

}

void setmask(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
	CheckArgs(argc, argv, 2, "setmsk", "iproc box");
	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(iproc[n].nbox, b);

	    {
	    int cx1 = iproc[n].box[b].cx1;
	    int cy1 = iproc[n].box[b].cy1;

#define CENOFF cy1*iproc[n].X + cx1

		    centermsk(&iproc[n].fakepix[CENOFF]
			    , iproc[n].box[b].cnx
			    , iproc[n].box[b].cny
			    , iproc[n].X
			    , iproc[n].box[b].maskx
			    , iproc[n].box[b].Bkrad
			    , iproc[n].box[b].Bkrad - iproc[n].box[b].Bkwid
			    , iproc[n].box[b].radius);

	    SendCommand( , iproc[n], IDLE);
	    SendCommand( , iproc[n], START);
	    }
	}
	msg_ack(c, msgid, NULL);
}

void setvalue(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
	CheckArgs(argc, argv, 4, "setpix", "iproc x y v");
	{
	    int	n = atoi(argv[1]);
	    int	x = atoi(argv[2]) - 1;
	    int	y = atoi(argv[3]) - 1;
	    int	v = atoi(argv[4]);
  	    CheckPrc(snappy->niproc, n);

	    iproc[n].fakepix[y*iproc->X + x] = v;
	}
	msg_ack(c, msgid, NULL);
}

getdata(iproc, c, n , b, command, size)
    	IProc	iproc;
	Client	c;
	int	n;
	int	b;
	int	command;
	int	size;
{

    	int i;

	SendCommand(
	    iproc[n].xbox = b;
	    , iproc[n], command);

	headsimple(iproc[n].head
		 , iproc[n].box[b].bx1
		 , iproc[n].box[b].by1
		 , iproc[n].box[b].bnx
		 , iproc[n].box[b].bny
		 , iproc[n].box[b].xzero
		 , iproc[n].box[b].yzero
		 , iproc[n].box[b].xscale
		 , iproc[n].box[b].yscale
		 , iproc[n].box[b].xunits
		 , iproc[n].box[b].yunits
		 , size * 8, iproc->frameno_h
		, iproc[n].box[b].frac);

	Write(c->out
		, iproc[n].head
		, FT_BLOCK
		  + ((iproc[n].box[b].bnx 
		  *   iproc[n].box[b].bny
		  * size
		    + FT_BLOCK) / FT_BLOCK) * FT_BLOCK, 1);

	Flush(c->out);
}

void getpixel(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
	CheckArgs(argc, argv, 2, "getpixel", "iproc box");
	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, NULL);
	    getdata(snappy->iproc, c, n, b, GETPIXEL, 2);
	}
}

void clrbias(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

    if ( argc != 2 ) {
	msg_nak(c, msgid, "%s takes %d args: %s %s", "clrbias", 1, "clrbias", "image");
	return;
    }
	{
	    int i = atoi(argv[1]) - 1;
	    int k;

  	    CheckPrc(snappy->niproc, i);

	    for ( k = 0; k < 640 * 480; k++ ) {
		iproc[i].data[k] = 0;
	    }

	    SendCommand(, iproc[i], SETBIAS);
	}

	msg_ack(c, msgid, NULL);
}

void lodbias(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

    if ( argc != 2 ) {
	msg_nak(c, msgid, "%s takes %d args: %s %s", "lodbias", 1, "lodbias", "image");
	return;

    }
	{
	    int i = atoi(argv[1]) - 1;
	    short *bias;

  	    CheckPrc(snappy->niproc, i);
	    Calloc(bias, 640 * 480 * sizeof(short));

	    fits = ft_headread(c->inp);
	           ft_dataread(c->inp, fits, iproc[i].data, 16);

	    SendCommand(, iproc[i], SETBIAS);
	    Free(bias);
	}

	msg_ack(c, msgid, NULL);
}

void setbias(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

    if ( argc != 3 ) {
	msg_nak(c, msgid, "%s takes %d args: %s %s", "setbias", 2, "setbias", "accum nframes");
	return;

    }
	{
	    int i = atoi(argv[1]) - 1;
	    int	a = atoi(argv[2]);

	    int j, k;
	    unsigned int *bias;

	CheckPrc(snappy->niproc, i);

	/*
	if ( !iproc[i].blocking ) {
	    msg_nak(c, msgid, "framegrabber is offline");
	    return;
	}
	*/

	Calloc(bias, 640 * 480 * sizeof(unsigned int));

	if ( a ) {
	    SendCommand(iproc[i].xbox = -1; , iproc[i], GETPIXEL);
	    for ( k = 0; k < 640 * 480; k++ ) {
		bias[k]  = ((unsigned char *)(iproc[i].data))[k];
	    }

	    for ( j = 1; j < a-1; j++ ) {
	    	SendCommand(iproc[i].xbox = -1;, iproc[i], GETPIXEL);

		for ( k = 0; k < 640 * 480; k++ ) {
		    bias[k] += ((unsigned char *)(iproc[i].data))[k];
		}
	    }
	}
	for ( k = 0; k < 640 * 480; k++ ) {
	    iproc[i].data[k] = bias[k];
	}

	SendCommand(, iproc[i], SETBIAS);
	Free(bias);
    }

    msg_ack(c, msgid, NULL);
}

void getbias(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    if ( argc != 2 ) {
	msg_nak(c, msgid, "%s takes %d args: %s %s", "getbias", 1, "getbias", "image");
	return;

    }
	{
	    int i = atoi(argv[1]) - 1;
	    int n = snappy->image[i].processor;
	    int b = snappy->image[i].accum;

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, NULL);
	    getdata(snappy->iproc, c, n, b, GETBIAS, 2);
	}
}

void xsmooth(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
    		IProc		iproc = snappy->iproc;
		FITSHead	fits;

    if ( argc != 3 ) {
	msg_nak(c, msgid, "%s takes %d args: %s %s", "smooth", 3, "smooth", "image onoff");
	return;

    }
	{
	    int I = atoi(argv[1]) - 1;
	    int n = snappy->image[I].processor;
	    int b = snappy->image[I].accum;

	    unsigned int *bias;

	    int	s = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(iproc[n].nbox, b);

	    iproc[n].box[b].smooth = s;
	    SendCommand(iproc[n].xbox = b; , iproc[n], SMOOTH);
	}

	msg_ack(c, msgid, NULL);
}

void getflat(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        char            *argv[];
{
	CheckArgs(argc, argv, 2, "getflat", "iproc box");
	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, NULL);
	    getdata(snappy->iproc, c, n, b, GETFLAT, 2);
	}
}

void getaccum(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
		int	i;

	CheckArgs(argc, argv, 2, "getacc", "iproc box");
	{
	    int	n = atoi(argv[1]);
	    int	b = atoi(argv[2]);

  	    CheckPrc(snappy->niproc, n);
  	    CheckBox(snappy->iproc[n].nbox, b);

	    msg_ack(c, msgid, NULL);
	    getdata(snappy->iproc, c, n, b, GETBOXAC, 2);
	}
}


void doexit(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy           snappy;
        int              argc;
        char            *argv[];
{
		int	i;

	for ( i = NIProc; i--; ) {
	    pthread_cancel(snappy->iproc[i].thread);
	    pthread_join(snappy->iproc[i].thread, NULL);
	}
	msg_ack(c, msgid, NULL);
	fprintf(stderr, "Exiting\n");
	exit(1);
}


int Signal = 0;

void *PublishAccum(s, msgtype, name, iproc, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     IProc iproc;
     void *buff;
     int leng;
{
    int	 	n = *(int *)buff;
    int 	i;

    pthread_attr_t	attr;
    struct sched_param param;

    iproc->nbox = n;

    Calloc (iproc->box, sizeof(struct _AccBox) * n);
    Calloc (iproc->cen, sizeof(struct _CenCmp) * n);

    for ( i = 0; i < n; i++ ) {
	iproc->box[i].accum = NULL;

	iproc->box[i].acctype = ProcNone;

	MMX(iproc->box[i].fracx, FRAC(0.5));
	iproc->box[i].frac = 1;
	iproc->box[i].N = 0;
	iproc->box[i].init = 1;

	iproc->box[i].buffr = NULL;

	iproc->box[i].ax1   = 0;
	iproc->box[i].ay1   = 0;
	iproc->box[i].anx   = 640;
	iproc->box[i].any   = 480;

	iproc->box[i].cx1   = 0;
	iproc->box[i].cy1   = 0;
	iproc->box[i].cnx   = 0;
	iproc->box[i].cny   = 0;

	iproc->box[i].bx1   = 0;
	iproc->box[i].by1   = 0;
	iproc->box[i].bnx   = 640;
	iproc->box[i].bny   = 480;


	iproc->box[i].iter  = 1;
	iproc->box[i].Bkwid = 0;

	iproc->box[i].smooth = 0;

	iproc->cen[i].x = -1;
	iproc->cen[i].y = -1;
    }

    /* Set the thread
     */
    param.sched_priority = 20;

    pthread_attr_init(&attr);
    /*
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);
     */

    pthread_create(&iproc->thread, &attr, framegrabber, iproc);

    return buff;
}

void *PublishFrame(s, msgtype, name, snappy, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Snappy snappy;
     void *buff;
     int leng;
{
    int	 	n = *(int *)buff;
    int 	i;
    IProc	iproc;

    if (msgtype != MsgSetVal) return;

    Calloc (snappy->iproc, sizeof(struct _IProc) * n);
    iproc = snappy->iproc;


    /* Start a thread for each frame grabber
     */
    for ( i = 0; i < snappy->niproc; i++ ) {
	int	j;
	int 	b;

	for ( j = 0; j < 640*480; j++ ) {
	    iproc[i].biasx[j] = 0;
	    iproc[i].biass[j] = 0;
	}
	msg_publish(s, "Frame%d_NAccum", MsgIntType, &snappy->iproc[i].naccum,  0, PublishAccum,  &snappy->iproc[i], NULL
			, i+1);

	/* Init the IProc struct
	 */
	iproc[i].N	     = i;

	iproc[i].dt      = NULL;
	iproc[i].command = 0;
	iproc[i].xbox    = 0;
	iproc[i].idle    = 1;
	iproc[i].stopped = 1;

	iproc[i].X       = XSIZE;
	iproc[i].Y       = YSIZE;
	
	pthread_mutex_init(&iproc[i].cmdlock, NULL);
	pthread_mutex_init(&iproc[i].cenlock, NULL);
	pthread_mutex_init(&iproc[i].radlock, NULL);
	pthread_cond_init(&iproc[i].cmddone, NULL);
	
	Malloc(iproc[i].head, XSIZE * YSIZE * 2 + FT_BLOCK * 2);
        iproc[i].data   = (void *) (iproc[i].head + FT_BLOCK);
    }

    return buff;
}

void sighandler(sig)
        int     sig;
{
        Signal = sig;
}

void *PublishImage();
void imageconfig();
void expose();
double postcenter();

void *imageio(snappy) 
	Snappy 	snappy;
{
	msg_up(snappy->imageio, NULL);
	msg_loop(snappy->imageio);

	return NULL;
}

void sethost(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        Snappy            data;
        int              argc;
        char            *argv[];
{
	static char	envstring[128];

	if ( argc != 2 ) {
	    msg_nak(c, msgid, "usage: sethost hostname");
	    return;
	}


	strcpy (envstring, "VIDEOSERVER=");
	strncat (envstring, argv[1], sizeof(envstring));
	putenv (envstring);

	msg_ack(c, msgid, NULL);
}

main(argc, argv)
	int	 argc;
	char	*argv[];
{
		int	pid;
		int	i;

    	pthread_attr_t	attr;

	MsgServer	m = NULL;
	
	struct Snappy 	snappy;
	
        sigset_t        mask;

        sigfillset(&mask);
        SigAction(SIGPIPE , sighandler, &mask, 0);
        sigfillset(&mask);
        SigAction(SIGALRM , sighandler, &mask, 0);
	
	snappy.niproc = 0;
	snappy.nimage = 0;

	pthread_t	imageio_thread;


	if ( !(m = msg_server(argv[0])) ) {
		perror("msg_server:");
		fprintf(stderr, "Exiting\n");
		exit(1);
	}
	if ( !(snappy.imageio = msg_server("VIDEOSERVER_IMAGEIO")) ) {
		perror("msg_server:");
		fprintf(stderr, "Exiting\n");
		exit(1);
	}

	msg_publish(m, "NumberOfFrames", MsgIntType, &snappy.niproc,  0, PublishFrame,  &snappy, NULL);

	msg_register(m, "ACK",		0, ACK,      NULL	, NULL);

	msg_register(m, "start",	0, start,    &snappy	, NULL);
	msg_register(m, "stop",		0, stop,     &snappy	, NULL);
	msg_register(m, "idle",		0, iidle,    &snappy	, NULL);
	msg_register(m, "resume",	0, resume,   &snappy	, NULL);

	msg_register(m, "setproc",	0, setproc,  &snappy	, NULL);
	msg_register(m, "setfrac",	0, setfrac,  &snappy	, NULL);
	msg_register(m, "setscale",	0, setscale, &snappy	, NULL);
	msg_register(m, "setzero",	0, setzero,  &snappy	, NULL);
	msg_register(m, "setsample",	0, setsam,   &snappy	, NULL);
	msg_register(m, "setblack",     0, setblack, &snappy      , NULL);
	msg_register(m, "setwhite",     0, setwhite, &snappy      , NULL);
	msg_register(m, "setchan",      0, setchan,  &snappy      , NULL);


	msg_register(m, "setacc",	0, setacc,   &snappy	, NULL);
	msg_register(m, "setbox",	0, setbox,   &snappy	, NULL);

	msg_register(m, "getcen",	0, getcen,   &snappy	, NULL);
	msg_register(m, "getfrac",	0, getfrac,  &snappy	, NULL);
	msg_register(m, "getscale",	0, getscale, &snappy	, NULL);
	msg_register(m, "setunits",	0, setunits, &snappy	, NULL);
	msg_register(m, "getunits",	0, getunits, &snappy	, NULL);
	msg_register(m, "getzero",	0, getzero,  &snappy	, NULL);

	msg_register(m, "getsample",	0, getsam,   &snappy	, NULL);


	msg_register(m, "backgr",	0, backgr,   &snappy	, NULL);

	msg_register(m, "setpix",	0, setpix, &snappy	, NULL);
	msg_register(m, "clrpix",	0, clrpix, &snappy	, NULL);

	msg_register(m, "setval",	0, setvalue, &snappy	, NULL);
	msg_register(m, "setmsk",	0, setmask,  &snappy	, NULL);
	msg_register(m, "getpix",	0, getpixel, &snappy	, NULL);
	msg_register(m, "getacc",	0, getaccum, &snappy	, NULL);
	msg_register(m, "clrbias",	0, clrbias , &snappy	, NULL);
	msg_register(m, "setbias",	0, setbias , &snappy	, NULL);
	msg_register(m, "getbias",	0, getbias , &snappy	, NULL);
	msg_register(m, "lodbias",	0, lodbias , &snappy	, NULL);
	msg_register(m, "getflat",	0, getflat , &snappy	, NULL);
	msg_register(m, "snap",		0, snapacc,  &snappy	, NULL);
	msg_register(m, "smooth",	0, xsmooth,   &snappy	, NULL);
	msg_register(m, "exit",		0, doexit  , &snappy	, NULL);
/* --- */

	msg_publish(m, "NumberOfImages", MsgIntType, &snappy.nimage,  0, PublishImage,  &snappy, NULL);
        msg_publish(m, "guide",  MsgIntType, NULL, 0, NULL, NULL, "set guiding state");
        msg_register(m, "expose", MsgCmdType, expose, &snappy, "Acquire guide data");
        msg_register(m, "imageconfig", MsgCmdType, imageconfig, &snappy, "Guide data parameters");

	msg_register(m, "sethost",      0, sethost,    &snappy, NULL);

    	pthread_attr_init(&attr);
        pthread_create(&imageio_thread, &attr, imageio, &snappy);

	msg_addtimer(m, 0.1, postcenter, &snappy); 
	

	msg_up(m, NULL);
	while ( 1 ) { msg_loop(m); }
	fprintf(stderr, "Exiting Msg loop returns!!\n");
}

