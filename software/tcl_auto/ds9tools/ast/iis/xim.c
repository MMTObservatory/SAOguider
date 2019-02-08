
#include <tcl.h>

#include "ximtool.h"
#include "iis.h"
#include "iistcl.h"

typedef int     (*ivector) ();
typedef void    (*xvector) ();

xim_initxim(xim)
	XimDataPtr      xim;
{
	int	i;

    for (i=0;  i < MAX_CLIENTS;  i++)
	xim->chan[i].type = 0;
}

tcleval(interp, cmd)
	Tcl_Interp *interp;
	char	*cmd;
{
    if ( Tcl_Eval(interp, cmd) == TCL_ERROR ) {
	fprintf(stderr, "%s\n", interp->result);
    }
}
   
xim_open(chan)
	IoChan	*chan;
{
	IIS xim = (IIS) chan->xim;

	tcleval(xim->interp, xim->open);
}

xim_close(chan)
	IoChan	*chan;
{
	IIS xim = (IIS) chan->xim;

	tcleval(xim->interp, xim->close);
}

xim_WritePixels(xim, frame, pixels, nbits, x, y, nx, ny)
	IIS	 xim;
	int	 frame;
	void	*pixels;
	int	 nbits;
	int	 x;
	int	 y;
	int	 nx;
	int	 ny;
{
	char	cmd[1280];

    if ( !xim->write ) return;

    sprintf(cmd, "%s %d 0x%X %d %d %d", xim->read, frame, pixels, x, y, nx, ny);
    tcleval(xim->interp, cmd); 
}

xim_ReadPixels(xim, frame, pixels, nbits, x, y, nx, ny)
	IIS	 xim;
	int	 frame;
	void	*pixels;
	int	 nbits;
	int	 x;
	int	 y;
	int	 nx;
	int	 ny;
{
	char	cmd[1280];
    if ( !xim->read ) return;

    sprintf(cmd, "%s %d 0x%X %d %d %d", xim->read, frame, pixels, x, y, nx, ny);
    tcleval(xim->interp, cmd); 
}

xim_eraseFrame(xim, frame)
	IIS	xim;
	int		frame;
{
	char	cmd[1280];

    if ( !xim->erase ) return;

    sprintf(cmd, "%s %d", xim->erase, frame);
    tcleval(xim->interp, cmd); 
}

xim_cursorMode(xim, frame, onoff)
	IIS	xim;
	int		frame;
	int		onoff;
{
	char	cmd[1280];

    if ( !xim->crmode ) return;

    sprintf(cmd, "%s %d %d", xim->crmode, frame, onoff);
    tcleval(xim->interp, cmd); 
}

xim_setFrameTitle(xim, frame, title)
	IIS	xim;
	int		frame;
	char		*title;
{
	char	cmd[1280];

    if ( !xim->title ) return;

    sprintf(cmd, "%s %d {%s}", xim->title, frame, title);
    tcleval(xim->interp, cmd); 
}

xim_setCursorPos(xim, frame, x, y)
	IIS	xim;
	int		frame;
	int		x, y;
{
	char	cmd[1280];
    if ( !xim->setcur ) return;

    sprintf(cmd, "%s %d %d %d", xim->setcur, frame, x, y);
    tcleval(xim->interp, cmd); 
}

xim_getCursorPos(xim, x, y, frame)
	IIS	xim;
	int		frame;
	int		*x, *y;
{
	char	cmd[1280];

    if ( !xim->getcur ) return;
    *x = 1;
    *y = 1;

    sprintf(cmd, "%s %d %d %d", xim->getcur, frame, *x, *y);
    tcleval(xim->interp, cmd); 
}

xim_setDisplayFrame(xim, frame)
	IIS	xim;
	int		frame;
{
	char	cmd[1280];

    if ( !xim->frame ) return;

    sprintf(cmd, "%s %d", xim->frame, frame);
    tcleval(xim->interp, cmd); 
}

xim_QueryColormap() {
}

xim_setReferenceFrame(chan, frame)
	IoChanPtr chan;
	int	  frame;
{
    chan->reference_frame = frame;
    chan->rf_p = &chan->xim->frame[frame-1];
    chan->rf_p->frameno = frame;
    chan->rf_p->height = 4096;
    chan->rf_p->height = 4096;
}

xim_setWCS(chan, frame)
	IoChan	*chan;
	int	 frame;
{
	char	cmd[1280];
	IIS xim = (IIS) chan->xim;

    if ( !xim->setwcs ) return;

    sprintf(cmd, "%s %d {%s}", xim->setwcs, frame, xim->xim.frame[frame-1].wcsbuf);
    tcleval(xim->interp, cmd); 
}

xim_getWCS(chan, frame)
	IoChan	*chan;
	int	 frame;
{
	char	cmd[1280];
	IIS xim = (IIS) chan->xim;

    if ( !xim->getwcs ) return;

    sprintf(cmd, "%s %d", xim->getwcs, frame);
    tcleval(xim->interp, cmd); 

    strcpy(xim->xim.frame[frame-1].wcsbuf, xim->interp->result);
}

void iisobjio(chan, mask)
	IoChan	*chan;
	int	 mask;
{
	int	fd = chan->datain;

    (*chan->func)(&fd, chan);
}

XtPointer xim_addInput (xim, fd, func, chan)
	XimDataPtr	 xim;
	int		 fd;
	void	       (*func)();
	IoChan		*chan;
{
    chan->func = func;
    Tcl_CreateFileHandler(fd , TCL_READABLE , (xvector) iisobjio , chan);
    return (void *) fd;
}

void xim_removeInput (xim, fd)
	IIS xim;
	int	   fd;
{
    Tcl_DeleteFileHandler(fd);
}

