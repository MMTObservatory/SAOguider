

#include <tcl.h>
#include <tk.h>
#include "../tclobj.h"

#include "ximtool.h"
#include "iistcl.h"

int  IISObjObj();
void IISObjDel();

struct _IIS iis = {
 {
	  ""
	, ""
	, "/tmp/.IMT%d"
	, 5137
 }
};


IISObjCmd(data, interp, objc, objv)
        ClientData       data;
        Tcl_Interp      *interp;
        int              objc;
        Tcl_Obj         *objv[];
{
    iis.interp = interp;

    TclCmdCmd(interp, iis, "query", "win ?root?", objc == 3 || objc == 4, objc, objv
	, 
	{
	    char	result[256];

	    int		leng;
	    int	x;
	    int y;

	    Tk_Window	tkwin = (Tk_Window) data;

	    Window win;
	    char  *name = Tcl_GetStringFromObj(objv[2], &leng);
  	    Tk_Window w = Tk_NameToWindow(interp, name, tkwin);
	    Window root = RootWindow(Tk_Display(w)
				, Tk_ScreenNumber(w));

	    if ( objc == 4 ) {
		win = root;
	    } else {
	 	if ( *name == '.' ) {
		    win = Tk_WindowId(w);
		} else {
		    win = strtol(name, NULL, 0);
		}
	    }
	    {
		int root;
		int child;
		int rx;
		int ry;
		int wx;
		int wy;
		int key;

	      XQueryPointer(Tk_Display(w), win
		, &root, &child, &rx, &ry, &wx, &wy, &key);

	      sprintf(result, "0x%0x %d %d", win, wx, wy);
	    }

	    TclObjCmdRetStr(interp, result, TCL_OK)
	}
    );
    TclCmdCmd(interp, iis, "warp", "win x y", objc == 5, objc, objv
	, 
	{
	    int		leng;
	    char	*name;
	    int	x;
	    int y;

	    Tk_Window	tkwin = (Tk_Window) data;
	    Tk_Window	w; 

	    Window	win;
	    Window	root = RootWindow(Tk_Display(tkwin)
				, Tk_ScreenNumber(tkwin));

	    name = Tcl_GetStringFromObj(objv[2], &leng);

	    Tcl_GetIntFromObj(interp, objv[3], &x);
	    Tcl_GetIntFromObj(interp, objv[4], &y);

	    if ( !strcmp(name, "root") ) {
		win = root;
	    } else {
	 	if ( *name == '.' ) {
  	            w = Tk_NameToWindow(interp, name, tkwin);
		    win = Tk_WindowId(w);
		} else {
		    win = strtol(name, NULL, 0);
		}
	    }

	    XWarpPointer(Tk_Display(tkwin), None, win
			, 0, 0, 0, 0, x, y);

	    return TCL_OK;
	}
    );
    TclCmdCmd(interp, iis, "init", "", objc == 2, objc, objv
	, 
	{
	    xim_initxim(&iis.xim);
	    xim_iisopen(&iis.xim);
	    return TCL_OK;
	}
    );
    TclCmdCmd(interp, iis, "close", "", objc == 2, objc, objv
	, 
	{
	    xim_iisclose(&iis.xim);
	    return TCL_OK;
	}
    );

    TclCmdCmd(interp, iis, "retcur", "x y key", 5 == 5, objc, objv
	, 
	{
	    double 	 x;
	    double 	 y;
	    char 	*key;
	    int		 frame;

	    if ( iis.xim.cursor_chan == NULL ) return;

	    frame = iis.xim.cursor_chan->reference_frame;

	    Tcl_GetDoubleFromObj(interp, objv[2], &x);
	    Tcl_GetDoubleFromObj(interp, objv[3], &y);
	    key = Tcl_GetStringFromObj(objv[4], NULL);

	    xim_retCursorVal(&iis, x, y, frame, 0, *key, "");
	    return TCL_OK;
	}
    );

    TclObjCmdV__(interp, (&iis), "port",   xim.port,      int,    objc, objv);
    TclObjCmdV__(interp, (&iis), "unix",   xim.unixaddr,  string, objc, objv);

    TclObjCmdV__(interp, (&iis), "open",   open,   string, objc, objv);
    TclObjCmdV__(interp, (&iis), "close",  close,  string, objc, objv);
    TclObjCmdV__(interp, (&iis), "erase",  erase,  string, objc, objv);
    TclObjCmdV__(interp, (&iis), "frame",  frame,  string, objc, objv);
    TclObjCmdV__(interp, (&iis), "title",  title,  string, objc, objv);
    TclObjCmdV__(interp, (&iis), "setcur", setcur, string, objc, objv);
    TclObjCmdV__(interp, (&iis), "getcur", getcur, string, objc, objv);
    TclObjCmdV__(interp, (&iis), "crmode", crmode, string, objc, objv);
    TclObjCmdV__(interp, (&iis), "setwcs", setwcs, string, objc, objv);
    TclObjCmdV__(interp, (&iis), "getwcs", getwcs, string, objc, objv);
    TclObjCmdV__(interp, (&iis), "read",   read,   string, objc, objv);
    TclObjCmdV__(interp, (&iis), "write",  write,  string, objc, objv);


  if ( objc != 1 ) {
     Tcl_AppendResult(interp
        , "iis: unknown command: "
        , Tcl_GetStringFromObj(objv[1], NULL), "\n", NULL); \
  }
  return TCL_ERROR;
}

