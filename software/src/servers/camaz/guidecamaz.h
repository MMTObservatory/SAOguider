/* mount.h
 */

#define NCameras	   3

typedef short   Camdata;

typedef struct _Handles {
        int     up;
        int     data;
        int     image;
} Handles;

typedef struct Camera {
	char		  *shid;	/* Camera image block def */
	FITSHead	   head;
	Camdata	          *data;
	Camdata          **data2d;
	char		  *base;
	int		   chipwidth, chipheight;

	float	rotation;
	float   skyx;      /* Camera position	  */
	float   skyy;
	float	xscale;
	float	yscale;
	float	focus;

	int   xparity;
	int   yparity;

	int   x, y, size, binning;

	Handles h;

} *Camera;

typedef struct GuideC {
	double	exptime;
	struct Camera		camera[NCameras];
} *GuideC;

#define CAMERA_PFILE	"r\n\
camera,i,a,1,0,9,\n\
x,r,a,0.0,,,\n\
y,r,a,0.0,,,\n\
size,i,a,5,,,\n\
binning,i,a,1,,,\n\
timeout,r,h,5.0,,,-\n\
mode,s,h,ql,,,-\n\
"

#define CAMCONFIG_PFILE	"r\n\
camera,r,a,0,,,camera\n\
shid,s,a,23001,,,Shid\n\
rotation,r,h,0,,,Camera rotation\n\
skyx,r,h,0,,,Camera position \n\
skyy,r,h,0,,,Camera position \n\
focus,r,h,0,,,Camera focus\n\
xparity,i,h,1,,,flip in X\n\
yparity,i,h,1,,,flip in Y\n\
rparity,i,h,1,,,flip in R\n\
chipwidth,i,h,256,,,X dim\n\
chipheight,i,h,256,,,Y dim\n\
scale,r,h,1.0,,,Camera angle\n\
saotng,b,h,yes,,,Init saotng\n\
timeout,r,h,5,,,-\n\
mode,s,h,ql,,,-\n\
"
