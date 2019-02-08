/* tclobj.c
 */

#include <tcl.h>
#include "tclobj.h"

Set_double(interp, str, d)
        Tcl_Interp      *interp;
        char            *str;
        double          *d;
{             return Tcl_GetDouble(interp, str, d); }

Get_double(interp, d)
        Tcl_Interp      *interp;
        double           d;
{
	char	buffer[32];

	sprintf(buffer, "%f", d);
	TclCmdRetStr(interp, buffer, TCL_VOLATILE, TCL_OK);
}

Set_float(interp, str, f)   
	Tcl_Interp	*interp;
	char		*str;
	float		*f ;
{
	double	d;

    if ( Tcl_GetDouble(interp, str, &d) == TCL_OK ) {
	*f = d;
	return TCL_OK;
    } else 
	return TCL_ERROR;
}

Get_float(interp, f)
	Tcl_Interp	*interp;
	double		 f;
{
	char	buffer[32];

	sprintf(buffer, "%f", f);
	TclCmdRetStr(interp, buffer, TCL_VOLATILE, TCL_OK);
}

Get_int(interp, obj, i)
        Tcl_Interp      *interp;
	char		*obj;
        int             *i;
{
	return Tcl_GetInt(interp, obj, i);
}

SetObj_int(interp, obj, i)
        Tcl_Interp      *interp;
	Tcl_Obj		*obj;
        int             *i;
{
	return Tcl_GetIntFromObj(interp, obj, i);
}

SetObj_string(interp, obj, s)
        Tcl_Interp      *interp;
	Tcl_Obj		*obj;
        char            **s;
{
	char	*v;
	char	*strdup();

    if ( (v = Tcl_GetStringFromObj(obj, NULL)) != NULL ) {
	*s = strdup(v);
	return TCL_OK;
    }

    return TCL_ERROR;
}

SetObj_double(interp, obj, d)
        Tcl_Interp      *interp;
	Tcl_Obj		*obj;
        double          *d;
{
	return Tcl_GetDoubleFromObj(interp, obj, d);
}

SetObj_float(interp, obj, f)   
	Tcl_Interp	*interp;
	Tcl_Obj		*obj;
	float		*f ;
{
	double	d;

    if ( Tcl_GetDoubleFromObj(interp, obj, &d) == TCL_OK ) {
	*f = d;
	return TCL_OK;
    } else 
	return TCL_ERROR;
}

GetObj_double(interp, d)
        Tcl_Interp      *interp;
        double           d;
{
	TclObjCmdRetDbl(interp, d, TCL_OK);
}

GetObj_float(interp, d)
        Tcl_Interp      *interp;
        double           d;
{
	TclObjCmdRetDbl(interp, d, TCL_OK);
}

GetObj_int(interp, i)
        Tcl_Interp      *interp;
        int             i;
{
	TclObjCmdRetInt(interp, i, TCL_OK);
}

GetObj_string(interp, s)
        Tcl_Interp      *interp;
        char            *s;
{
	TclObjCmdRetStr(interp, s, TCL_OK);
}
