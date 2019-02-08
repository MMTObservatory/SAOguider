/* guide.c
 */

#include <xos.h>
#include <xfile.h>
#include <except.h>
#include <msg.h>
#include <fitsy.h>

#include <pthread.h>

#include "iproc.h"

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void *image(s, op, name, snappy, data, leng)
        MsgServer        s;
	int		 op;
	char		*name;
	Snappy		 snappy;
	void		*data;
	int		*leng;
{
    int n = atoi(&name[5]) -1;

  switch ( op ) {
   case MsgGetVal: {
    int prc = snappy->image[n].processor;
    int box = snappy->image[n].accum;

    if ( snappy->image[n].head == NULL ) {
	Malloc(snappy->image[n].head, FT_BLOCK + ((XSIZE * YSIZE * 2 + FT_BLOCK) / FT_BLOCK) * FT_BLOCK);
	snappy->image[n].data   = (short *) (snappy->image[n].head + FT_BLOCK);
    }


    *leng = FT_BLOCK + ((
		  snappy->iproc[prc].box[box].bnx
		* snappy->iproc[prc].box[box].bny
		* 2
		+ FT_BLOCK) / FT_BLOCK) * FT_BLOCK;

    if ( snappy->imageio == s ) {
printf("Lock\n");
	pthread_mutex_lock(&snappy->image[n].lock);
    	return snappy->image[n].buff;
    } else {
    	return snappy->image[n].head;
    }
    break;
   }

   case MsgLokVal:
    if ( snappy->imageio == s ) {
printf("UnLock\n");
        pthread_mutex_unlock(&snappy->image[n].lock);
    }
    break;
    return NULL;
  }
}


void imageconfig(s, c, msgid, snappy, argc, argv)
    MsgServer        s;
    Client           c;
    int              msgid;
    Snappy            snappy;
    int              argc;
    char            *argv[];
{
    int	one = 1;
    int n;
    double x, y;
    int    w, bin;
    int dtest;
    int colmax,rowmax;

    int prc;
    int box;

    iferr (
	    if ( argc != 6 ) {
	    	printf("argc: %d\n",argc);
	    	error("usage: camera <no> <x> <y> <size> <bin>");
	    }
	    n=atoi(argv[1]) - 1;
	    x=atof(argv[2]);
	    y=atof(argv[3]);
	    w=atoi(argv[4]);
	    bin=atoi(argv[5]);

	    prc = snappy->image[n].processor;
	    box = snappy->image[n].accum;

	    snappy->image[n].x = x + snappy->image[n].x0;
	    snappy->image[n].y = y + snappy->image[n].y0;
	    snappy->image[n].w = w;
	    snappy->image[n].h = w;

	    snappy->image[n].dirty = 1;

	    dosetacc(snappy->iproc, prc, box
		, (int) snappy->image[n].x
		, (int) snappy->image[n].y
		, snappy->image[n].w
		, snappy->image[n].h);
    	
	    dosetbox(snappy->iproc, prc, box, x, y
		, 9
		, (int) (x/2-snappy->iproc[prc].box[box].Bkwid)
		, (int) (x/2)
		, snappy->iproc[prc].box[box].Bkwid
		, snappy->iproc[prc].box[box].bscalefrom);
	  ) {
	msg_nak(c, msgid, errmsg());
    }

    msg_ack(c, msgid, NULL);
}

check_image(snappy, i)
        Snappy          snappy;
	int		i;
{
    int prc;
    int box;

    double f = snappy->image[i].expose;

    IProc iproc = snappy->iproc;

    prc = snappy->image[i].processor;
    box = snappy->image[i].accum;

    iproc[prc].box[box].data = snappy->image[i].data;

    if ( snappy->image[i].head == NULL ) {
	Malloc(snappy->image[i].head, FT_BLOCK + ((XSIZE * YSIZE * 2 + FT_BLOCK) / FT_BLOCK) * FT_BLOCK);
	snappy->image[i].data   = (short *) (snappy->image[i].head + FT_BLOCK);

        Malloc(snappy->image[i].buff, FT_BLOCK + ((XSIZE * YSIZE * 2 + FT_BLOCK) / FT_BLOCK) * FT_BLOCK);
    }
    if ( snappy->image[i].dirty ) {

	    snappy->iproc[prc].xbox = box;
	    snappy->iproc[prc].box[box].frac = Max(1, f*30.0);
	    MMX(snappy->iproc[prc].box[box].fracx, FRAC((f*30-1.0)/(int)(f*30)));

	dosetacc(snappy->iproc
		, prc, box
		, (int) snappy->image[i].x
		, (int) snappy->image[i].y
		, snappy->image[i].w
		, snappy->image[i].h);
    	
	dosetbox(snappy->iproc, prc, box
		, snappy->image[i].x
		, snappy->image[i].y
		, snappy->image[i].type == 0 ? 9 : 0
		, snappy->image[i].rad
		, snappy->image[i].bkrad
		, snappy->image[i].bkwid
		, snappy->image[i].bscalefrom);
    }
}

finishimage(s, snappy, i)
        MsgServer       s;
        Snappy          snappy;
	int		i;
{
    int	one = 1;

    int prc;
    int box;

    double f = snappy->image[i].expose;
    IProc iproc = snappy->iproc;

    prc = snappy->image[i].processor;
    box = snappy->image[i].accum;
    int j;


    if ( iproc[prc].box[box].init ) {
	for ( 	j = 0;
		j < iproc[prc].box[box].bnx * iproc[prc].box[box].bny;
		j++ ) {
	    snappy->image[i].data[j] /= iproc[prc].box[box].init;
	}
    }

    if ( ft_byteswap() ) {
	bswap(snappy->image[i].data
	    , snappy->image[i].data
	    , iproc[prc].box[box].bnx * iproc[prc].box[box].bny * 2);
    }

    headsimple(snappy->image[i].head
	    , (int) (iproc[prc].box[box].bx1 - snappy->image[i].x0)
	    , (int) (iproc[prc].box[box].by1 - snappy->image[i].y0)
	    , iproc[prc].box[box].bnx
	    , iproc[prc].box[box].bny
	    , snappy->image[i].x0
	    , snappy->image[i].y0
	    , iproc[prc].box[box].xscale
	    , iproc[prc].box[box].yscale
	    , iproc[prc].box[box].xunits
	    , iproc[prc].box[box].yunits
	    , 16, iproc[prc].frameno_h
	    , iproc[prc].box[box].frac);

    if ( !pthread_mutex_trylock(&snappy->image[i].lock) ) {
	memcpy(snappy->image[i].buff, snappy->image[i].head
		, iproc[prc].box[box].bnx * iproc[prc].box[box].bny * 2 + FT_BLOCK
	);
	pthread_mutex_unlock(&snappy->image[i].lock);
    }
    

    if ( snappy->image[i].dirty ) {
	msg_postmrk(s, snappy->image[i].config, &one);
	snappy->image[i].dirty = 0;
    } else {
        msg_postmrk(s, snappy->image[i].update, &one);
    }
}

exposeimage(s, snappy, i)
        MsgServer       s;
        Snappy          snappy;
	int		i;
{
    int prc;
    int box;

    IProc iproc = snappy->iproc;

    prc = snappy->image[i].processor;
    box = snappy->image[i].accum;

    check_image(snappy, i);

    SendCommand(
	    iproc[prc].xbox = box;
	    , iproc[prc], GETPROCED);


    finishimage(s, snappy, i);
}

void expose(s, c, msgid, snappy, argc, argv)
        MsgServer        s;
        Client           c;
        int              msgid;
        Snappy          snappy;
        int              argc;
        char            *argv[];
{
    int	type;
    int	i;

    iferr (
        if ( argc < 2 || argc > 3 ) {
            printf("argc: %d\n",argc);
            error("usage: expose <type> <time>");
        }
	
if ( 0 ) {
	for ( i = 0; i < snappy->nimage; i++ ) {
	    exposeimage(s, snappy, i);
	}
}

	for ( i = 0; i < snappy->nimage; i++ ) {
	    check_image(snappy, i);
	}

	for ( i = 0; i < snappy->niproc; i++ ) {
    	    SendCommand(
		snappy->iproc[i].xbox = -1;
		, snappy->iproc[i], GETPROCED);
	}

	for ( i = 0; i < snappy->nimage; i++ ) {
	    finishimage(s, snappy, i);
	}
    ) {
	msg_nak(c, msgid, errmsg());
    } else {
        msg_ack(c, msgid, NULL);
    }
}

void *imalloc(s, type, name, snappy, buff, leng)
        MsgServer        s;
        int              type;
        char            *name;
        Snappy           snappy;
        int             *buff;
        int             *leng;
{
    	int id;
    	int n = atoi(&name[5])-1;

    if ( snappy->image[n].data ) return;


    if ( snappy->image[n].shmkey ) {
	id = shmget(snappy->image[n].shmkey
		, FT_BLOCK + ((XSIZE * YSIZE * 2 + FT_BLOCK) / FT_BLOCK) * FT_BLOCK
		, IPC_CREAT|0666);

	if ( (snappy->image[n].head = shmat(id, 0, 0)) == (char *) -1 ) {
	    fprintf(stderr, "cannot map image %d %d\n", n+1, id); 
	}
    }

    Malloc(snappy->image[n].buff, FT_BLOCK + ((XSIZE * YSIZE * 2 + FT_BLOCK) / FT_BLOCK) * FT_BLOCK);
    snappy->image[n].data   = (short *) (snappy->image[n].head + FT_BLOCK);

    return buff;
}

void *setdirty(s, type, name, data, buff, leng)
        MsgServer        s;
        int              type;
        char            *name;
        int             *data;
        void            *buff;
        int             *leng;
{
    *data  = 1;
    return buff;
}


void *PublishImage(s, msgtype, name, snappy, buff, leng)
     MsgServer s;
     int msgtype;
     char *name;
     Snappy snappy;
     void *buff;
     int leng;
{
    int n = *(int *)buff;
    int i;
    if (msgtype != MsgSetVal) return buff;

    snappy->image = (Image) calloc (sizeof(struct Image), n);


    for (i=0; i < n; i++) {
	snappy->image[i].bkrad = 20;
	snappy->image[i].bkwid =  5;

	pthread_mutex_init(&snappy->image[i].lock, NULL);

	msg_publish(s, "Image%d_Name",         MsgStrType,  snappy->image[i].name,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Type",         MsgIntType, &snappy->image[i].type,        0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Frame",        MsgIntType, &snappy->image[i].processor,   0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Accum",        MsgIntType, &snappy->image[i].accum,       0, NULL, NULL, NULL, i+1);
	msg_publish(s, "Image%d_Exposure",     MsgDblType, &snappy->image[i].exposure,    0, NULL, NULL, NULL, i+1);

	msg_publish(s, "Image%d_X0",           MsgDblType, &snappy->image[i].x0,          0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Y0",           MsgDblType, &snappy->image[i].y0,          0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_X",            MsgDblType, &snappy->image[i].x,           0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Y",            MsgDblType, &snappy->image[i].y,           0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Width",        MsgIntType, &snappy->image[i].w,           0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Height",       MsgIntType, &snappy->image[i].h,           0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Expose",       MsgDblType, &snappy->image[i].expose,      0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_Rad",          MsgIntType, &snappy->image[i].rad,         0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_BkRad",        MsgIntType, &snappy->image[i].bkrad,       0, setdirty, &snappy->image[i].dirty, NULL, i+1);
	msg_publish(s, "Image%d_BkWid",        MsgIntType, &snappy->image[i].bkwid,       0, setdirty, &snappy->image[i].dirty, NULL, i+1);

	msg_publish(s, "Image%d_BScaleFrom",   MsgIntType, &snappy->image[i].bscalefrom,  0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Image%d_CX",	       MsgDblType, &snappy->image[i].cenx,        0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_CY",	       MsgDblType, &snappy->image[i].ceny,        0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Image%d_Counts",       MsgDblType, &snappy->image[i].counts,      0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_Min",	       MsgDblType, &snappy->image[i].min,         0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_Max",	       MsgDblType, &snappy->image[i].max,         0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_Bkgnd",        MsgDblType, &snappy->image[i].bkg,         0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Image%d_RawCounts",    MsgDblType, &snappy->image[i].rawcounts,   0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_RawMin",       MsgDblType, &snappy->image[i].rawmin,      0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_RawMax",       MsgDblType, &snappy->image[i].rawmax,      0, NULL, NULL, NULL, i+1);
        msg_publish(s, "Image%d_RawBkgnd",     MsgDblType, &snappy->image[i].rawbkg,      0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Image%d_ShmKey",       MsgIntType, &snappy->image[i].shmkey,      0, imalloc, snappy, NULL, i+1);
        msg_publish(s, "Image%d_ShmSize",      MsgIntType, &snappy->image[i].shmsize,     0, NULL, NULL, NULL, i+1);
	snappy->image[i].shmsize = XSIZE * YSIZE * 2 + FT_BLOCK;

	snappy->image[i].config = msg_publish(s, "Image%d_Config",       MsgIntType, NULL, 0, NULL, NULL, NULL, i+1);
	snappy->image[i].update = msg_publish(s, "Image%d_Update",       MsgIntType, NULL, 0, NULL, NULL, NULL, i+1);

        msg_publish(s, "Image%d",              MsgBlkType, NULL, 0, image, snappy, "Xfer guide image data", i+1);
        msg_publish(snappy->imageio, "Image%d",MsgBlkType, NULL, 0, image, snappy, "Xfer guide image data", i+1);
    }

    return buff;
}


double postcenter(m, tid, snappy) 
	MsgServer	 m;
	int		 tid;
        Snappy           snappy;
{
	int	i;
	int	b;

	for ( i = 0; i < snappy->nimage; i++ ) {
	    CenCmp cenbox;

	    int prc = snappy->image[i].processor;
	    int box = snappy->image[i].accum;

	    GetCenter(&cenbox, &snappy->iproc[prc], box);

	    snappy->image[i].cenx = cenbox.x + 1;
	    snappy->image[i].ceny = cenbox.y + 1;

	    if ( snappy->iproc[prc].box[box].init ) {
		snappy->image[i].bkg = cenbox.bkg / snappy->iproc[prc].box[box].init;
		snappy->image[i].min = cenbox.min / snappy->iproc[prc].box[box].init;
		snappy->image[i].max = cenbox.max / snappy->iproc[prc].box[box].init;

		snappy->image[i].rawbkg = cenbox.bkg;
		snappy->image[i].rawmin = cenbox.min;
		snappy->image[i].rawmax = cenbox.max;
	    }
	}

	return 0.25;
}
