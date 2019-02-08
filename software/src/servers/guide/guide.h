#ifndef __GUIDE_H
#define __GUIDE_H

#include <math.h>
#include <xos.h>
#include <xfile.h>
#include <msg.h>

void Background();
double Snr();


#define MAX_LOOKBACK	10
#define DEGSPERRAD      (180.0 / M_PI)
#define MAXBUFLEN       80

#define AzErr    0
#define ElErr    1
#define RotErr   2
#define FocErr   3

typedef struct _Camera {
	char	name[MAXBUFLEN];
	int	xsize;
	int	ysize;
	double	xpixelsize;
	double	ypixelsize;

	double  radius;
	double	camangle;	/* Camera location in r,t on instrument	*/
	double	rotation;	/* Camera pixel grid position angle	*/
	int	parity;
	double	defocus;
	int     split;
	int	binning;
	int     bitpix;

	int     dirty;
} Camera;

typedef struct _Image {
	char	name[MAXBUFLEN];
	int	camera;
	int     fullimage;
	double	exposure;
	double	defocus;

	char	grid[MAXBUFLEN];
	int	gridwidth;
	int	gridheight;
	int	gridrotate;
	double	gridrotation;
	double  gridzoom;

	int     imagesizepix;
	char    shmkeys[MAXBUFLEN];
	int     shmsize;

	int     dirty;
} Image;

typedef struct _Box {
	int	image;
	double	x;
	double	y;
	double	xoff;
	double	yoff;
	double	fwhm;
	double	cenx;
	double	ceny;
	double  cenaz;
	double  cenel;
	double  counts;
	double  background;

	double  noise;
        double  sigmaxythresh;
        double  sigmaxy;
        double  sigmafwhmthresh;
        double  sigmafwhm;
	int     locked;

        double  lockedpossumx;
        double  lockedpossumy;
        int     lockedposcount;
  
	int     boxsizepix;

	int     dirty;
  
  } Box;

typedef struct Look {
    double	sent;
    double	time;
} Look;

typedef struct _Error {
    char        name[MAXBUFLEN];
    double	gain;
    double	offset;
    double	step;
    double	teldelay;
    double	locktol;
    int	        sample;
    int         active;

    int 	integratep;
    int 	integratet;
    int 	integrates;
    double	integrateq;
    
    int         errhdl;
    int         rawhdl;

    int         update;
    int         config;

    Look	lookback[MAX_LOOKBACK];
} Error;

typedef struct _Guide {

	int     dirty;
	int	locked;
	int	inpos;

        MsgServer       guideserv;
    	MsgServer	camerserv;
	MsgServer	mountserv;
	MsgServer	focusserv;
	MsgServer       shuttserv;
	MsgServer       wavefserv;

	MsgServer	S[6];

	int	ncameras;
	Camera	*camera;
	int	nimages;
	Image	*image;
	int	nboxes;
	Box	*box;
	int     nerrors;
	Error   *error;

	double  boxsize;
	double  imagesize;
	double	exposure;
	int	backgroundwidth;

	char    activeboxlist[MAXBUFLEN];
	char    transferboxlist[MAXBUFLEN];
	char    fullimagelist[MAXBUFLEN];

	int     nactiveboxes;
	int     nfullimages;

	int	boxlist[10];
	int	fulllist[10];

	int     boxbin;
	int     fullbin;

	double	telescopediameter;
	double	primaryfratio;
	double	cassfratio;
	double	focusunits;

	double  rotang;
	double  posang;
	double  eleang;

	double     delay;
	unsigned int     state;
        unsigned int     send;
	unsigned int     tid;

	int     gvalid;
	int     ngoodboxes;

        double  wstagepos;
        double  wstagemin;
        double  wstagemax;
        int     wvalid;

	File    log;
	int	count;
	char    guidefile[MAXBUFLEN];

        int 	looknext;
	double	lockerr;

	int	flex;

	double	x_zero[10];
	double	y_zero[10];
	
	double	elcoeff[3];
	double	azcoeff[3];
} Guide;

void CalcBox(Guide  *guide, int box, int imgsize, int boxsize, double **data2d);
void xytoazel(Guide *guide, int box, double  x, double  y, double *az, double *el);
void azeltoxy(Guide *guide, int box, double az, double el, double  *x, double  *y);
double **ClipImage( double **box, int x, int y, double bg, double thresh);
double **DetGetImage(Guide *, int, double *, double *);


#endif

