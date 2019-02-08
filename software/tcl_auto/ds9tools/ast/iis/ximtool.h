/*
 * XIMTOOL.H -- Global definitions for XImtool.
 */

#define XtPointer void *
#define Boolean   int
#define String    char *
#define Widget	  void *
#define XtInputId int

#define MAX_COLORS              256     /* max size colormap            */
#define MAX_CLIENTS             8       /* max display server clients   */
#define MAX_FRAMES              16      /* max number of frames         */


#define SZ_CMAPNAME             32      /* colormap name buffer         */
#define SZ_FNAME                256
#define SZ_IMTITLE              128     /* image title string           */
#define SZ_LABEL                256     /* main frame label string      */
#define SZ_WCSBUF               320     /* WCS text buffer size         */
#define SZ_FIFOBUF              4000    /* transfer size for FIFO i/o   */


/* Functions.
 */
#ifndef abs
#define	abs(a)		(((a)<0)?(-(a)):(a))
#endif
#ifndef min
#define min(a,b)	((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b)	((a)<(b)?(b):(a))
#endif


/* Magic numbers. */
#define DEF_PORT		5137		/* default tcp/ip socket */
#define	I_DEVNAME		"/dev/imt1o"	/* pseudo device names */
#define	O_DEVNAME		"/dev/imt1i"	/* our IN is client's OUT */
#define	DEF_UNIXADDR		"/tmp/.IMT%d"	/* default unix socket */

/* WCS definitions. */
#define	W_UNITARY	0
#define	W_LINEAR	1
#define	W_LOG		2
#define	W_DEFFORMAT	" %7.2f %7.2f %7.1f%c"

/* Rotation matrix defining world coordinate system (WCS) of a frame.  */
typedef struct {
	int valid;			/* has WCS been set?		*/
	float a, b;			/* x, y scale factors		*/
	float c, d;			/* x, y cross factors		*/
	float tx, ty;			/* x, y translation		*/
	float z1, z2;			/* greyscale range		*/
	int zt;				/* greyscale mapping		*/
	char format[32];		/* wcs output format		*/
	char imtitle[SZ_IMTITLE+1];	/* image title from WCS	*/
} Ctran, *CtranPtr;



/* Predefined colormaps. */
typedef struct {
	int mapno;			/* widget colormap number */
	char name[SZ_CMAPNAME+1];	/* colormap name */
} ColorMap, *ColorMapPtr;

/* Predefined lookup tables. */
typedef struct {
	float red, green, blue;
} Triplet, *TripletPtr;

typedef struct {
	int	lutlen;
	Triplet hue[MAX_COLORS];
} Lut, *LutPtr;


/* The frame buffers. */
typedef struct {
	int frameno;			/* frame number			*/
	int	width;
	int	height;
	char label[SZ_LABEL+1];		/* frame label string		*/
	Ctran ctran;			/* world coordinate system	*/

	void	*wcs;
	char wcsbuf[SZ_WCSBUF];		/* wcs info string		*/

	char	 *data;
} FrameBuf, *FrameBufPtr;

/* Client I/O channel. */
typedef struct {
	struct _XimData *xim;		/* backpointer to xim descriptor */
	XtPointer id;			/* input callback id */
	int type;			/* channel type */
	int datain;			/* input channel */
	int dataout;			/* output channel */
	int keepalive;			/* used to keep input fifo ready */
	char path[SZ_FNAME+1];		/* for unix sockets */
	int reference_frame;		/* reference (cmd i/o) frame */
	FrameBufPtr rf_p;		/* reference frame descriptor */
	void	(*func)();
} IoChan, *IoChanPtr;


typedef struct _XimData {
        String input_fifo;              /* client's output, e.g. /dev/imt1o */
        String output_fifo;             /* client's input, e.g. /dev/imt1i */
        String unixaddr;                /* format for unix socket path */
        int port;                       /* port for INET socket */

        IoChan chan[MAX_CLIENTS];       /* client i/o descriptors */
        IoChanPtr cursor_chan;          /* cursor mode channel */

	int	nframes;
	FrameBufPtr	df_p;
	FrameBuf	frame[MAX_FRAMES];
} XimData, *XimDataPtr;


#define	IO_FIFO		1
#define	IO_INET		2
#define	IO_UNIX		3

int  xim_iisopen();
void xim_iisclose();
int  xim_iisio();

XtPointer xim_addInput();
void xim_removeInput();
 
void xim_clientOpen(), xim_clientClose();
int  xim_clientExecute();

