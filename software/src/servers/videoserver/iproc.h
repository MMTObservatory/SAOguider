/* iproc.h
 */

#define NIProc	3

#define	XSIZE	640
#define YSIZE	480

#define BOX_COMMAND 	 1
#define NOP		 2

#define GETPIXEL	 3
#define GETFLAT		 5
#define SETFLAT		 6
#define GETBIAS		 7
#define SETBIAS		 8
#define CENTER		10
#define IDLE		11
#define RESUME		12
#define STOP		13
#define START		14
#define FLUSH		15
#define SETBLACK	16
#define SETWHITE	17
#define GETBOXAC	19
#define GETPROCED	20

#define ProcNone	20
#define ProcCopy	21
#define ProcAccum	22
#define ProcReduc	23
#define ProcC		25
#define ProcXCpy	26
#define ProcAdd		27
#define ProcXAdd	28

#define CenterNone	30
#define CenterC		31
#define CenterASM	32
#define CenterMMX	33

#define SMOOTH		40

typedef struct _CenCmp	{
	double		 x;		/* Center x			*/
	double		 y;		/* Center y			*/
	double		 bkg;		/* Background value		*/
	double		 sum;
	int		 r;		/* Center r			*/
	int		 bksum;		/* Sum of box pixels		*/
	int		 min;
	int		 max;
	int		 frame;
	struct timeval   time;
} CenCmp;

typedef struct _AccBox   {
	int		acctype;	/* Accum type			*/
	int		ax1;		/* Box x1			*/
	int		ay1;		/* Box y1			*/
	int		anx;		/* Box width			*/
	int		any;		/* Box height			*/

	double		cx;		/* Box center x			*/
	double		cy;		/* Box center y			*/

	int		bx1;		/* Box x1			*/
	int		by1;		/* Box y1			*/
	int		bnx;		/* Box width			*/
	int		bny;		/* Box height			*/

	int		cx1;		/* Centroid x1			*/
	int		cy1;		/* Centroid y1			*/
	int		cnx;		/* Centroid width		*/
	int		cny;		/* Centroid height		*/

	unsigned short 	*accum;
	char		*maskx;

	double		biasscale;

	int		N;
	int		init;
	int		frac;
	double		fracx;
	char		*buffr;

	double		xzero;
	double		yzero;
	double		xscale;
	double		yscale;
	char		*xunits;
	char		*yunits;

	int		 iter;		/* Max iterations to settle	*/
	int		 radius;
	int		 Bkpix;		/* Number of bk pixels in mask	*/
	int		 Bkrad;		/* Radius of bk pixels in mask	*/
	int		 Bkwid;		/* Width of bk pixels in mask	*/
	double		 thres;

	int		bscalefrom;

	int		 A;
	double		 T;
	int		 nK;
	int		 smooth;

	unsigned short *data;
} AccBox;
	

typedef struct _IProc {
	int		N;		/* Frame grabber #	*/

	int		stopped;
	int		idle;
	int		blocking;

	pthread_t	thread;

	pthread_mutex_t	cmdlock;
	pthread_cond_t	cmddone;

	pthread_mutex_t	cenlock;
	pthread_mutex_t	radlock;

	int		command;
	int		xbox;

	int		naccum;
	int		nbox;
	AccBox		*box;		/* params from server	*/
	CenCmp		*cen;		/* Computed values	*/

	int		black;
	int		white;

	void	       *dt;		/* Framegrabber		*/
	int 		X;		/* Image size		*/
	int 		Y;

	unsigned char  *pixel;

	unsigned char 	fakepix[XSIZE*YSIZE + FT_BLOCK];
	unsigned short 	biasx[XSIZE*YSIZE];
	unsigned short 	biass[XSIZE*YSIZE];
	unsigned short 	flatx[XSIZE*YSIZE];

	unsigned char  *head;
	unsigned short *data;

/*	unsigned long	frameno;	*/
/*	unsigned long	frameno_h;	*/
	unsigned long	frameno;
	unsigned long	frameno_h;

	struct timeval  time_h;
	struct timeval  time;

	int	timeout;
} *IProc;

typedef struct Image {
	char	name[132];
	pthread_mutex_t	lock;
	int	type;
	int	processor;
	int	accum;
	int	shmkey;
	int	shmsize;
	double	exposure;
	double	x0;
	double	y0;
	double	x;
	double	y;
	int	w;
	int	h;
	double	expose;
	int	rad;
	int	bkrad;
	int	bkwid;
	double	cenx;
	double	ceny;
	double	counts;
	double	min;
	double	max;
	double	bkg;

	int	bscalefrom;
	

	double  rawcounts;
	double	rawmin;
	double	rawmax;
	double	rawbkg;

	char	*buff;
	char	*head;
	short	*data;

	int	dirty;
	int	config;
	int	update;
} *Image;

typedef struct Snappy {
	MsgServer imageio;
	int	nimage;
	Image	image;
	int	niproc;
	IProc	iproc;
} *Snappy;
	

#define MMX(d, value)   ((short *) &d)[0] =  (short) (value);	\
			((short *) &d)[1] =  (short) (value);	\
			((short *) &d)[2] =  (short) (value);	\
			((short *) &d)[3] =  (short) (value);
#define FRAC(value)	((unsigned short) ((value) * (unsigned) 0x7FFF))

#define GetCenter(cb, iproc, b) {				\
		double t;					\
								\
	    pthread_mutex_lock(&((iproc)->cenlock));		\
		(*(cb)) = (iproc)->cen[b];		\
	    pthread_mutex_unlock(&((iproc)->cenlock));		\
	    /*
	    (cb)->x = ((cb)->x-((iproc)->box[b].xzero+1))	\
				* (iproc)->box[b].xscale;	\
	    (cb)->y = ((cb)->y-((iproc)->box[b].yzero+1))	\
				* (iproc)->box[b].yscale; */	\
}

#define SendCommand(code, iproc, cmd)				\
	pthread_mutex_lock(&iproc.cmdlock);			\
	code							\
	iproc.command = cmd;					\
	pthread_cond_wait(&iproc.cmddone, &iproc.cmdlock);	\
	pthread_mutex_unlock(&iproc.cmdlock);

