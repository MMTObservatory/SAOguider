
#include <xos.h>
#include <xfile.h>
#include <msg.h>


#include <except.h>

#include "guide.h"


void transfer(s, c, msgid, guide, argc, argv)
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

	double	daz, del;
	double	dx,  dy;
    
    if ( argc < 2 || (argc-1) % 2 ) {
	msg_nak(c, msgid, "wrong number of args to transfer");
	return;
    }
    nbox = argc - 1;
    Malloc(boxes, sizeof(int) * nbox);
    for ( i = 0; i < nbox; i++ ) {
	    boxes[i] = atoi(argv[i+1]);
    }
    
    iferr ( 
	   if ( !DetExpose(guide, "box", guide->exposure) ) {
	  	error("guide: can't expose image");
	   }
	   
	   Malloc(data2d, nbox * sizeof(*data2d));
	   
	   for ( i = 0; i < nbox; i++ ) {
		double expend;
		double explen;

	        int box = boxes[i]-1;
	        int img = guide->box[box].image-1;
	        int cam = guide->image[img].camera-1;

        	int imgsizepix = guide->imagesize /
          		Min(guide->camera[cam].xpixelsize,guide->camera[cam].ypixelsize) /
            		guide->camera[cam].binning;
		int boxsizepix = guide->box[box].boxsizepix;
		
	       
	       if ( (data2d[i] = DetGetImage(guide, img, &expend, &explen)) == NULL ) {
		   error("guide: can't get image %d for box %d", img+1, box+1);
	       }

        	CalcBox(guide, box, imgsizepix, boxsizepix, data2d[i]);
		if (    guide->box[box].sigmaxy < 0
		     || guide->box[box].sigmaxy > guide->box[box].sigmaxythresh ) {
		    error("Transfer failed.  No star in Box %d", box+1);
		}
	   }


	for ( i = 0; i < nbox; i+=2 ) {
	       	int box = boxes[i]-1;
	       	int img = guide->box[box].image-1;
	       	int cam = guide->image[img].camera-1;
	
		int box2 = boxes[i+1]-1;
		int img2 = guide->box[box2].image-1;

		daz = guide->box[box2].cenaz - guide->box[box].cenaz;
		del = guide->box[box2].cenel - guide->box[box].cenel;

		azeltoxy(guide, box2, daz, del, &dx, &dy);

		guide->box[box2].x += dx;
		guide->box[box2].y += dy;
		guide->image[img2].dirty = 1;
	
printf("Box %d -> %d : az: %4.3f el: %4.3f  dx: %4.3f dy: %4.3f\n"
		, box, box2, daz, del, dx, dy);

	}	
	   if ( !DetExpose(guide, "box", guide->exposure) ) {
	       error("guide: can't expose image");
	   }
    ) {
	msg_nak(c, msgid, errmsg());
	return;
    }
    
    msg_ack(c, msgid, NULL);
}
