/*
 *----------------------------------------------------------------------
 *
 * Tcl_FormatObjCmd --
 *
 *	This procedure is invoked to process the "format" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

#include <tcl.h>
#define UCHAR(c) ((unsigned char) (c))
double SAOstrtod();



	/* ARGSUSED */
int
Ast_FormatObjCmd(dummy, interp, objc, objv)
    ClientData dummy;    	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    register char *format;	/* Used to read characters from the format
				 * string. */
    int formatLen;              /* The length of the format string */
    char *endPtr;               /* Points to the last char in format array */
    char newFormat[40];		/* A new format specifier is generated here. */
    int width;			/* Field width from field specifier, or 0 if
				 * no width given. */
    int precision;		/* Field precision from field specifier, or 0
				 * if no precision given. */
    int size;			/* Number of bytes needed for result of
				 * conversion, based on type of conversion
				 * ("e", "s", etc.), width, and precision. */
    int intValue;		/* Used to hold value to pass to sprintf, if
				 * it's a one-word integer or char value */
    char *ptrValue = NULL;	/* Used to hold value to pass to sprintf, if
				 * it's a one-word value. */
    double doubleValue;		/* Used to hold value to pass to sprintf if
				 * it's a double value. */
    int whichValue;		/* Indicates which of intValue, ptrValue,
				 * or doubleValue has the value to pass to
				 * sprintf, according to the following
				 * definitions: */
#   define INT_VALUE 0
#   define PTR_VALUE 1
#   define DOUBLE_VALUE 2
#   define MAX_FLOAT_SIZE 320
    
    Tcl_Obj *resultPtr;  	/* Where result is stored finally. */
    char staticBuf[MAX_FLOAT_SIZE + 1];
                                /* A static buffer to copy the format results 
				 * into */
    char *dst = staticBuf;      /* The buffer that sprintf writes into each
				 * time the format processes a specifier */
    int dstSize = MAX_FLOAT_SIZE;
                                /* The size of the dst buffer */
    int noPercent;		/* Special case for speed:  indicates there's
				 * no field specifier, just a string to copy.*/
    int objIndex;		/* Index of argument to substitute next. */
    int gotXpg = 0;		/* Non-zero means that an XPG3 %n$-style
				 * specifier has been seen. */
    int gotSequential = 0;	/* Non-zero means that a regular sequential
				 * (non-XPG3) conversion specifier has been
				 * seen. */
    int useShort;		/* Value to be printed is short (half word). */
    char *end;			/* Used to locate end of numerical fields. */

    int sao  = 0;
    int	flag = 0;		/* Conversion flags for SAOconvert()	*/

    /*
     * This procedure is a bit nasty.  The goal is to use sprintf to
     * do most of the dirty work.  There are several problems:
     * 1. this procedure can't trust its arguments.
     * 2. we must be able to provide a large enough result area to hold
     *    whatever's generated.  This is hard to estimate.
     * 2. there's no way to move the arguments from objv to the call
     *    to sprintf in a reasonable way.  This is particularly nasty
     *    because some of the arguments may be two-word values (doubles).
     * So, what happens here is to scan the format string one % group
     * at a time, making many individual calls to sprintf.
     */

    if (objc < 2) {
        Tcl_WrongNumArgs(interp, 1, objv,
		"formatString ?arg arg ...?");
	return TCL_ERROR;
    }

    format = Tcl_GetStringFromObj(objv[1], &formatLen);
    endPtr = format + formatLen;
    resultPtr = Tcl_NewObj();
    objIndex = 2;

    while (format < endPtr) {
	register char *newPtr = newFormat;

	width = precision = noPercent = useShort = 0;
	whichValue = PTR_VALUE;

	/*
	 * Get rid of any characters before the next field specifier.
	 */
	if (*format != '%') {
	    ptrValue = format;
	    while ((*format != '%') && (format < endPtr)) {
		format++;
	    }
	    size = format - ptrValue;
	    noPercent = 1;
	    goto doField;
	}

	if (format[1] == '%') {
	    ptrValue = format;
	    size = 1;
	    noPercent = 1;
	    format += 2;
	    goto doField;
	}

	/*
	 * Parse off a field specifier, compute how many characters
	 * will be needed to store the result, and substitute for
	 * "*" size specifiers.
	 */
	*newPtr = '%';
	newPtr++;
	format++;
	if (isdigit(UCHAR(*format))) {
	    int tmp;

	    /*
	     * Check for an XPG3-style %n$ specification.  Note: there
	     * must not be a mixture of XPG3 specs and non-XPG3 specs
	     * in the same format string.
	     */

	    tmp = strtoul(format, &end, 10);
	    if (*end != '$') {
		goto notXpg;
	    }
	    format = end+1;
	    gotXpg = 1;
	    if (gotSequential) {
		goto mixedXPG;
	    }
	    objIndex = tmp+1;
	    if ((objIndex < 2) || (objIndex >= objc)) {
		goto badIndex;
	    }
	    goto xpgCheckDone;
	}

	notXpg:
	gotSequential = 1;
	if (gotXpg) {
	    goto mixedXPG;
	}

	xpgCheckDone:
	while ((*format == '-') || (*format == '#') || (*format == '0')
		|| (*format == ' ') || (*format == '+')) {

	    switch ( *format ) {
		case '-': flag |=  2;      break;
		case '+': flag |=  4;      break;
		case ' ': flag |=  8;      break;
		case '#': flag |= 16;      break;
		case '0': flag |= 32;      break;
	    }

	    *newPtr = *format;
	    newPtr++;
	    format++;
	}
	if (isdigit(UCHAR(*format))) {
	    width = strtoul(format, &end, 10);
	    format = end;
	} else if (*format == '*') {
	    if (objIndex >= objc) {
		goto badIndex;
	    }
	    if (Tcl_GetIntFromObj(interp, objv[objIndex], 
                    &width) != TCL_OK) {
		goto fmtError;
	    }
	    objIndex++;
	    format++;
	}
	if (width > 100000) {
	    /*
	     * Don't allow arbitrarily large widths:  could cause core
	     * dump when we try to allocate a zillion bytes of memory
	     * below.
	     */

	    width = 100000;
	} else if (width < 0) {
	    width = 0;
	}
	if (width != 0) {
	    TclFormatInt(newPtr, width);
	    while (*newPtr != 0) {
		newPtr++;
	    }
	}
	if (*format == '.') {
	    *newPtr = '.';
	    newPtr++;
	    format++;
	}
	if (isdigit(UCHAR(*format))) {
	    precision = strtoul(format, &end, 10);
	    format = end;
	} else if (*format == '*') {
	    if (objIndex >= objc) {
		goto badIndex;
	    }
	    if (Tcl_GetIntFromObj(interp, objv[objIndex], 
                    &precision) != TCL_OK) {
		goto fmtError;
	    }
	    objIndex++;
	    format++;
	}
	if (precision != 0) {
	    TclFormatInt(newPtr, precision);
	    while (*newPtr != 0) {
		newPtr++;
	    }
	}
	if (*format == 'l') {
	    format++;
	} else if (*format == 'h') {
	    useShort = 1;
	    *newPtr = 'h';
	    newPtr++;
	    format++;
	}
	*newPtr = *format;
	newPtr++;
	*newPtr = 0;
	if (objIndex >= objc) {
	    goto badIndex;
	}
	switch (*format) {
	    case 'i':
		newPtr[-1] = 'd';
	    case 'd':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
		if (Tcl_GetIntFromObj(interp, objv[objIndex], 
		        (int *) &intValue) != TCL_OK) {
		    goto fmtError;
		}
		whichValue = INT_VALUE;
		size = 40 + precision;
		break;
	    case 's':
		ptrValue = Tcl_GetStringFromObj(objv[objIndex], &size);
		break;
	    case 'c':
		if (Tcl_GetIntFromObj(interp, objv[objIndex], 
                        (int *) &intValue) != TCL_OK) {
		    goto fmtError;
		}
		whichValue = INT_VALUE;
		size = 1;
		break;
	    case 'e':
	    case 'E':
	    case 'f':
	    case 'g':
	    case 'G':
	    case '@': {
		    char	*str;

		sao = *format == '@';

		str = Tcl_GetStringFromObj(objv[objIndex], &size);
		doubleValue = SAOstrtod(str, NULL);
		
		/*
		if (Tcl_GetDoubleFromObj(interp, objv[objIndex], 
			&doubleValue) != TCL_OK) {
		    goto fmtError;
		}
		*/

		whichValue = DOUBLE_VALUE;
		size = MAX_FLOAT_SIZE;
		if (precision > 10) {
		    size += precision;
		}
		break;
	    }
	   
	    case 0:
		Tcl_SetResult(interp,
		        "format string ended in middle of field specifier",
			TCL_STATIC);
		goto fmtError;
	    default:
		{
		    char buf[40];
		    sprintf(buf, "bad field specifier \"%c\"", *format);
		    Tcl_SetResult(interp, buf, TCL_VOLATILE);
		    goto fmtError;
		}
	}
	objIndex++;
	format++;

	/*
	 * Make sure that there's enough space to hold the formatted
	 * result, then format it.
	 */

	doField:
	if (width > size) {
	    size = width;
	}
	if (noPercent) {
	    Tcl_AppendToObj(resultPtr, ptrValue, size);
	} else {
	    if (size > dstSize) {
	        if (dst != staticBuf) {
		    ckfree(dst);
		}
		dst = (char *) ckalloc((unsigned) (size + 1));
		dstSize = size;
	    }

	    if (whichValue == DOUBLE_VALUE) {
		if ( sao ) {
		    SAOconvert(dst, doubleValue, ':', width, precision, flag);
		    sao = 0;
		} else {
		    sprintf(dst, newFormat, doubleValue);
		}
	    } else if (whichValue == INT_VALUE) {
		if (useShort) {
		    sprintf(dst, newFormat, (short) intValue);
		} else {
		    sprintf(dst, newFormat, intValue);
		}
	    } else {
	        sprintf(dst, newFormat, ptrValue);
	    }
	    Tcl_AppendToObj(resultPtr, dst, -1);
	}
    }

    Tcl_SetObjResult(interp, resultPtr);
    if(dst != staticBuf) {
        ckfree(dst);
    }
    return TCL_OK;

    mixedXPG:
    Tcl_SetResult(interp, 
            "cannot mix \"%\" and \"%n$\" conversion specifiers", TCL_STATIC);
    goto fmtError;

    badIndex:
    if (gotXpg) {
        Tcl_SetResult(interp, 
                "\"%n$\" argument index out of range", TCL_STATIC);
    } else {
        Tcl_SetResult(interp, 
                "not enough arguments for all format specifiers", TCL_STATIC);
    }

    fmtError:
    if(dst != staticBuf) {
        ckfree(dst);
    }
    Tcl_DecrRefCount(resultPtr);
    return TCL_ERROR;
}
