/* AstMath.c

 This is the AstMath to TCL binding.
 */

#include <tcl.h>
/* #include <tk.h> */

extern int Ast_FormatObjCmd();
extern int IISObjCmd();

#include "tclobj.h"

#include "Ast.h"
#include "ast.h"

/* func(data, interp, args, result) */

int tcl_min(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = Min(args[0].doubleValue, args[1].doubleValue);
	return TCL_OK;
}
int tcl_max(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = Max(args[0].doubleValue, args[1].doubleValue);
	return TCL_OK;
}
int tcl_r2d(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = r2d(args[0].doubleValue);
	return TCL_OK;
}
int tcl_r2h(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = r2h(args[0].doubleValue);
	return TCL_OK;
}
int tcl_d2h(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = d2h(args[0].doubleValue);
	return TCL_OK;
}
int tcl_d2r(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = d2r(args[0].doubleValue);
	return TCL_OK;
}
int tcl_h2d(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = h2d(args[0].doubleValue);

	return TCL_OK;
}

int tcl_h2r(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = h2r(args[0].doubleValue);
	return TCL_OK;
}

int tcl_lst(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = h2r(args[0].doubleValue);
	return TCL_OK;
}

int tcl_pa(data, interp, args, result)
	void		*data;
	Tcl_Interp	*interp;
	Tcl_Value	 args[MAX_MATH_ARGS];
	Tcl_Value	*result;
{
	result->type        = TCL_DOUBLE;
	result->doubleValue = h2r(args[0].doubleValue);
	return TCL_OK;
}

/*
 * The data structure below defines a math function (e.g. sin or hypot)
 * for use in Tcl expressions.
 */
 
typedef struct AstFunc {
    char *name;			/* The Function Name			*/
    int numArgs;                /* Number of arguments for function. */
    Tcl_ValueType argTypes[MAX_MATH_ARGS];
                                /* Acceptable types for each argument. */
    int (*proc)();         	/* Procedure that implements this function.
                                 * NULL if isBuiltinFunc is 1. */
    ClientData clientData;      /* Additional argument to pass to the
                                 * function when invoking it. NULL if
                                 * isBuiltinFunc is 1. */
} AstFunc;

AstFunc astmathtable[] = {
   { 	  "min", 2, { TCL_DOUBLE, TCL_DOUBLE }, tcl_min, NULL  }
 , { 	  "max", 2, { TCL_DOUBLE, TCL_DOUBLE }, tcl_max, NULL  }
 , { 	  "r2d", 1, { TCL_DOUBLE }, tcl_r2d, NULL  }
 , { 	  "r2h", 1, { TCL_DOUBLE }, tcl_r2h, NULL  }
 , { 	  "d2r", 1, { TCL_DOUBLE }, tcl_d2r, NULL  }
 , { 	  "d2h", 1, { TCL_DOUBLE }, tcl_d2h, NULL  }
 , { 	  "h2d", 1, { TCL_DOUBLE }, tcl_h2d, NULL  }
 , { 	  "h2r", 1, { TCL_DOUBLE }, tcl_h2r, NULL  }
 , {       NULL }
};

Ast_ObjStrtod(dummy, interp, objc, objv)
    ClientData dummy;    	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
	if ( objc != 2 ) {
	    TclObjCmdRetStr(interp, "strtod takes one arg", TCL_ERROR);
	}

	TclObjCmdRetDbl(interp
		, SAOstrtod(Tcl_GetStringFromObj(objv[1], NULL), NULL)
		, TCL_OK);
}

  extern int Ast_FormatObjCmd();


EXPORT(int,Ast_Init)(interp)
	Tcl_Interp *interp;
{
	int	i;
        extern int Ast_ObjSla();

  static char init_script[] =
    "if [catch {source [lindex $tcl_pkgPath 0]/Asttcl.tcl}] {\n"
#include "Ast.tcl"
    "}\n";

  if (Tcl_PkgProvide (interp, "Ast", AST_VERSION) != TCL_OK) {
    return TCL_ERROR;
  }

  for ( i = 0; astmathtable[i].name != NULL; i++ ) {
      Tcl_CreateMathFunc(interp
		, astmathtable[i].name
		, astmathtable[i].numArgs
		, astmathtable[i].argTypes
		, astmathtable[i].proc
		, astmathtable[i].clientData);
  }


  Tcl_CreateObjCommand (  interp
			, "strtod"
                        , Ast_ObjStrtod
                        , (ClientData) NULL                            
                        , (Tcl_CmdDeleteProc *) NULL);              



  Tcl_CreateObjCommand (  interp
			, "sla"
                        , Ast_ObjSla
                        , (ClientData) NULL                            
                        , (Tcl_CmdDeleteProc *) NULL);              

  Tcl_CreateObjCommand (  interp
			, "format"
                        , Ast_FormatObjCmd                                    
                        , (ClientData) NULL                            
                        , (Tcl_CmdDeleteProc *) NULL);              

/*
  Tcl_CreateObjCommand (interp, "iis", IISObjCmd,
                     (ClientData) Tk_MainWindow(interp),
                     (Tcl_CmdDeleteProc *) NULL);
 */


#ifdef WIN32
  tkNormalUid   = Tk_GetUid("normal");
  tkDisabledUid = Tk_GetUid("disabled");
#endif
  /* return Tcl_Eval(interp, init_script); */

 return TCL_OK;
}

#ifdef WIN32
/*
 *----------------------------------------------------------------------
 *
 * DllEntryPoint --
 *
 *      This wrapper function is used by Windows to invoke the
 *      initialization code for the DLL.  If we are compiling
 *      with Visual C++, this routine will be renamed to DllMain.
 *      routine.
 *
 * Results:
 *      Returns TRUE;
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */
 
BOOL APIENTRY
DllEntryPoint(hInst, reason, reserved)
    HINSTANCE hInst;            /* Library instance handle. */
    DWORD reason;               /* Reason this function is being called. */
    LPVOID reserved;            /* Not used. */
{
  return TRUE;
}
#endif /* WIN32 */

