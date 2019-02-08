/* guidecamaz.c
 */

#include <except.h>
 
#include <xos.h>
#include <xfile.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <math.h>

#include <parameter.h>
#include <fitsy.h>
/*
#include <table.h>
 */
 
#include <sys/signal.h>
 
#include <msg.h>

#include "guidecamaz.h"
 
double atof();
 
int             needup[4];
MsgServer	m = NULL;

void sighandler()
{
	/*
	msg_shutdown(m);
	exit(1); 
	 */
}

exception       args = "guidecamaz args";
 
void argerr(level, message, name)
{
        except(args, "%s: %s: %s", message, paramerrstr(), name);
}



void camconfig(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        GuideC            data;
        int              argc;
        char            *argv[];
{
    iferr (
        paramfile       cl = clinit(argc, argv, CAMCONFIG_PFILE);
	int		camera 	= clgeti("camera")-1;
	char		idbuf[256];
	char	       *shid    = clgetstr("shid", idbuf, 256);
	
	float		rotation = clgetd("rotation");
	float		skyx        = clgetd("skyx");
	float		skyy        = clgetd("skyy");

	float		focus 	= clgetd("focus");
	int		xparity	= clgeti("xparity");
	int		yparity	= clgeti("yparity");

	float		chipwidth= clgetd("chipwidth");
	float		chipheight = clgetd("chipheight");
	float		scale      = clgetd("scale");

	clclose();

	data->camera[camera].shid       = strdup(shid);
	data->camera[camera].skyx       = skyx;
	data->camera[camera].skyy       = skyy;
	data->camera[camera].rotation   = rotation;
	data->camera[camera].xscale     = scale;
	data->camera[camera].yscale     = scale;
	data->camera[camera].focus      = focus;
	data->camera[camera].chipwidth  = chipwidth;
	data->camera[camera].chipheight = chipheight;
	data->camera[camera].xparity    = xparity;
	data->camera[camera].yparity    = yparity;

	data->camera[camera].size	= chipwidth;
	data->camera[camera].x		= chipwidth/2;
	data->camera[camera].y		= chipheight/2;
	data->camera[camera].binning	= 1;

	FixCamera(data, camera);

	msg_ack(c, msgid, NULL);

    ) {
	msg_nak(c, msgid, errmsg());
	return;
    }
    mkimage(s,data);
}


void imageconfig(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        GuideC            data;
        int              argc;
        char            *argv[];
{
    iferr (
        paramfile       cl = clinit(argc, argv, CAMERA_PFILE);
        int             camera  = clgeti("camera")-1;
        int          	x       = clgeti("x");
        int          	y       = clgeti("y");
        int          	size    = clgeti("size");
        int             binning = clgeti("binning");

	clclose();

	data->camera[camera].x		= x;
	data->camera[camera].y		= y;
	data->camera[camera].size	= size / binning;
	data->camera[camera].binning	= binning;
	FixCamera(data, camera);

	msg_ack(c, msgid, NULL);

    ) {
	msg_nak(c, msgid, errmsg());
	return;
    }

    mkimage(s,data);
}

void sethost(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        GuideC            data;
        int              argc;
        char            *argv[];
{
	static char	envstring[128];
	extern int camaz_initialized;

	if ( argc != 2 ) {
	    msg_nak(c, msgid, "usage: sethost hostname");
	    return;
	}

	camaz_initialized = 0;

	strcpy (envstring, "AZCAMHOST=");
	strncat (envstring, argv[1], sizeof(envstring));
	putenv (envstring);

	msg_ack(c, msgid, NULL);
}

void setport(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        GuideC            data;
        int              argc;
        char            *argv[];
{
	static char	envstring[128];

	if ( argc != 2 ) {
	    msg_nak(c, msgid, "usage: setport portnumber");
	    return;
	}

	strcpy (envstring, "AZCAMHOSTPORT=");
	strncat (envstring, argv[1], sizeof(envstring));
	putenv (envstring);

	msg_ack(c, msgid, NULL);
}

void expose(s, c, msgid, data, argc, argv)
        MsgServer       s;
        Client          c;
        int             msgid;
        GuideC            data;
        int              argc;
        char            *argv[];
{
		int	exptype;

	if ( argc != 3 ) {
	    msg_nak(c, msgid, "no exposure time");
	    return;
	}

	exptype = !strcmp(argv[1], "full");
	data->exptime = atof(argv[2]);

	if ( data->camera[0].base == NULL ) {
	    msg_nak(c, msgid, "expose frame - no shared memory buffer");
	    return;
	}

	if (frame_next (
	  data->camera[0].x,
	  data->camera[0].y,
	  data->camera[0].size,
	  data->camera[0].size,
	  data->camera[0].binning,
	  data->camera[0].binning,
	  data->exptime,
	  data->camera[0].data) == 0) {
	    msg_nak(c, msgid, "expose frame failure");
	    return;
	}

	mkimage(s,data, exptype);
	msg_ack(c, msgid, NULL);
}


mkimage(s, guidec, type)
        MsgServer s;
	GuideC	  guidec;
	int	  type;
{
	int	camera, x, y;
	int     one = 1;

   for ( camera = 0; camera < NCameras; camera++ ) {

	if ( guidec->camera[camera].base   == NULL ) {
	    continue;
	}

	if (camera != 0) {
	    int fy = guidec->camera[camera].y - guidec->camera[camera].size/2;
	    for ( y = 0; y < guidec->camera[camera].size; y++, fy++ )  {
		if ((0 <= fy) && (fy < guidec->camera[0].size)) {
		    int fx = guidec->camera[camera].x - guidec->camera[camera].size/2;
		    for ( x = 0; x < guidec->camera[camera].size; x++, fx++ ) {
			if ((0 <= fx) && (fx < guidec->camera[0].size)) {
			    guidec->camera[camera].data2d[y][x] = guidec->camera[0].data2d[fy][fx];
			} else {
			    guidec->camera[camera].data2d[y][x] = 0;
			}
		    }
		} else {
		    for ( x = 0; x < guidec->camera[camera].size; x++ )
			guidec->camera[camera].data2d[y][x] = 0;
		}
	    }
	}
   }

   for ( camera = 0; camera < NCameras; camera++ ) {

	if ( guidec->camera[camera].base   == NULL ) {
	    continue;
	}

	ft_dataswap(guidec->camera[camera].data2d[0],
		    guidec->camera[camera].data2d[0],
		    guidec->camera[camera].size * guidec->camera[camera].size
		      * sizeof(Camdata), 
		    sizeof(Camdata)*8);
        if ( needup[camera] ) {
                msg_postmrk(s, guidec->camera[camera].h.up, &one);
                needup[camera] = 0;
        }
        msg_postmrk(s, guidec->camera[camera].h.data,  &one);
	msg_postmrk(s, guidec->camera[camera].h.image, &one); 

    }
}


void *image(s, type, name, guidec, buff, leng)
	MsgServer	 s;
	int		 type;
	char		*name;
	GuideC		 guidec;
	void 		*buff;
	int		*leng;
{
        int b = atoi(&name[5]) - 1;

        *leng = FT_BLOCK + ((
 			    (guidec->camera[b].size)
			  * (guidec->camera[b].size)
                          *  sizeof(Camdata)
                         + FT_BLOCK) / FT_BLOCK) * FT_BLOCK;

        return guidec->camera[b].base;
    
}

main(argc, argv)
        int     argc;
        char    *argv[];
{
        sigset_t        mask;

	struct GuideC	guidec;

	int		guidebin;
	char		objectname[128];
	char		filename[128];

	int		i;
	int             guide;

    paramerract(argerr);

    guidec.exptime = 1.0;

    for ( i = 0; i < NCameras; i++ ) {
	guidec.camera[i].base  = NULL;
    }

    paramerract(argerr);
	
    sigfillset(&mask);
    SigAction(SIGHUP , sighandler, &mask, 0);
    SigAction(SIGINT , sighandler, &mask, 0);
    SigAction(SIGQUIT, sighandler, &mask, 0);
    SigAction(SIGABRT, sighandler, &mask, 0);
    SigAction(SIGPIPE, sighandler, &mask, 0);
 
    if ( (m = msg_server(argv[0])) == NULL ) {
        perror("guidecamaz");
        exit(1);
    }
 
    /* Detector Server routines
     */
    msg_register(m, "imageconfig", 0, imageconfig, &guidec, NULL);
    msg_register(m, "expose",      0, expose,      &guidec, NULL);

    msg_register(m, "sethost",      0, sethost,    &guidec, NULL);
    msg_register(m, "setport",      0, setport,    &guidec, NULL);


    guidec.camera[0].h.data  = msg_publish (m, "Image1_Update", MsgIntType, NULL, 0, NULL , NULL, NULL);
    guidec.camera[1].h.data  = msg_publish (m, "Image2_Update", MsgIntType, NULL, 0, NULL , NULL, NULL);
    guidec.camera[2].h.data  = msg_publish (m, "Image3_Update", MsgIntType, NULL, 0, NULL , NULL, NULL);
    guidec.camera[0].h.up    = msg_publish (m, "Image1_Config", MsgIntType, NULL, 0, NULL , NULL, NULL);
    guidec.camera[1].h.up    = msg_publish (m, "Image2_Config", MsgIntType, NULL, 0, NULL , NULL, NULL);
    guidec.camera[2].h.up    = msg_publish (m, "Image3_Config", MsgIntType, NULL, 0, NULL , NULL, NULL);

    guidec.camera[0].h.image = msg_publish (m, "Image1",	     MsgBlkType, NULL, 0, image, &guidec, NULL);
    guidec.camera[1].h.image = msg_publish (m, "Image2",	     MsgBlkType, NULL, 0, image, &guidec, NULL);
    guidec.camera[2].h.image = msg_publish (m, "Image3",	     MsgBlkType, NULL, 0, image, &guidec, NULL);

    msg_publish (m, "guidebin",    MsgIntType, &guidebin, 0, NULL, NULL, "current guide binning");
    msg_publish (m, "guide"   ,    MsgIntType, &guide   , 0, NULL, NULL, "set guideing state");
    msg_publish (m, "object",      MsgStrType, objectname, 128, NULL, NULL, "current object name");


    msg_publish (m, "FILENAME",    MsgStrType, filename, 128, NULL, NULL, NULL);
    msg_publish (m, "SPLITMODE",   MsgIntType, NULL, 0,   NULL, NULL, NULL);
    msg_publish (m, "ROWBIN",      MsgIntType, NULL, 0,   NULL, NULL, NULL);
    msg_publish (m, "COLBIN",      MsgIntType, NULL, 0,   NULL, NULL, NULL);
    msg_publish (m, "TIMEOUTS",    MsgIntType, NULL, 0,   NULL, NULL, NULL);
    msg_publish (m, "LOSTBUF",     MsgIntType, NULL, 0,   NULL, NULL, NULL);

    msg_register(m, "camconfig", 0, camconfig,&guidec, NULL);


    msg_up(m,NULL);
    msg_loop(m);
}

FixCamera(data, camera)
	GuideC	data;
	int	camera;
{

    if ( data->camera[camera].base != NULL ) {
	    MemRelease(data->camera[camera].base);
	    Free(data->camera[camera].data2d);
	    ft_headfree(data->camera[camera].head, 0);
	    data->camera[camera].base = NULL;
    }

    data->camera[camera].base   = MemSegment(data->camera[camera].shid
		, ((data->camera[camera].size
		    * data->camera[camera].size
		    * sizeof(Camdata)
		 + FT_BLOCK-1) / FT_BLOCK)*FT_BLOCK + FT_BLOCK
		, 0666 | IPC_CREAT
		, NULL, NULL);

    if ( data->camera[camera].base == NULL ) {
	    return 0;
    }

    ft_cardclr((FITSCard) data->camera[camera].base, 36);
		
    data->camera[camera].head = ft_headinit((FITSCard) data->camera[camera].base
			, 1 * FT_BLOCK);
    data->camera[camera].data = (Camdata *) ((char *)data->camera[camera].base + FT_BLOCK);
    data->camera[camera].data2d = (Camdata **) ft_make2d((void *) data->camera[camera].data
				, sizeof(Camdata), 0, 0
				, data->camera[camera].size
				, data->camera[camera].size);

    ft_headappl(data->camera[camera].head, "SIMPLE", 0,   1, NULL);
    ft_headappi(data->camera[camera].head, "BITPIX", 0,  16, NULL);
    ft_headappi(data->camera[camera].head, "NAXIS" , 0,   2, NULL);
    ft_headappi(data->camera[camera].head, "NAXIS" , 1
			    , data->camera[camera].size, NULL);
    ft_headappi(data->camera[camera].head, "NAXIS" , 2
			    , data->camera[camera].size, NULL);
    ft_headappr(data->camera[camera].head, "BSCALE" , 0, 1.0, 1, NULL);
    ft_headappi(data->camera[camera].head, "BZERO"  , 0, 32768, NULL);


    needup[camera] = 1;
}
