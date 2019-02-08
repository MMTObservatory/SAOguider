/* except.c
 *
 * A simple error/exception handler.
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */

#if 0

/* Trap any exception into a single handler 
 */
iferr(<code to trap errors within >)
{
    <error handler >
}


/* Trap some named exceptions to specific 
 * exception handlers
 */
exception       bingo_ex = "bingo exception";
exception       wacko_ex = "wacko exception";
...

/* Trap exceptions by name 
*/

ex_check(
	 <code to trap exceptions within >
)
ex_if(ex_bingo)			    /* Named exceptions		 */
	< exception handler >
ex_if(ex_wacko)
	< exception handler >
...

ex_if(ex_error)			    /* The default exception	 */
	< error handler >
ex_end


error("printf string",...);		    /* take ex_error	 */
excpt(excep, printf string,...);	    /* take exception	 */
fatal("printf string",...);		    /* always die		 */

char           *errmsg()		    /* return last message	 */
                ivector(*errvec) (ivector); /* set error vector	 */

#endif


#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "except.h"

exception       ex_error = "error";	/* Error exception		 */
exception       ex_fatal = "fatal";	/* Fatal exception		 */
exception	ex_malloc = "malloc";	/* Memory Accocation		 */

static void    	excexe();		/* Process an exception		 */
static int      excdie();		/* Default error handler	 */
static evector  exchdl = excdie;  	/* Error handler pointer	 */

excbox   *excstk = NULL;		/* The stack of errors		 */
exception excval = NULL;	    	/* The current exception	 */

static char     excstr[ERRSTRLEN];    	/* The formatted error string	 */

/* Take the Fatal exception.
 */
#ifdef __STDC__
void fatal(char *format, ...)
{
    va_list         args;
    va_start(args, format);
#else
void fatal(va_alist) va_dcl
{
    va_list         args;

    char	*format;
    va_start(args);
    format = va_arg(args, char *);
#endif

    if ( format == NULL ) excstr[0] = '\0';
    excexe(ex_fatal, format, args);
}


/* Take the default exception.
 */
#ifdef __STDC__
void error(char *format, ...)
{
    va_list         args;
    va_start(args, format);
#else
void error(va_alist) va_dcl
{
    va_list	 args;
    char	*format;

    va_start(args);
    format = va_arg(args, char *);
#endif

    if ( format == NULL ) excstr[0] = '\0';
    excexe(ex_error, format, args);
}


/* Take specific exception to the situation
 */
#ifdef __STDC__
void except(exception excpt, char *format, ...) 
{
    va_list         args;
    va_start(args, format);
#else
void except(va_alist) va_dcl
{
    va_list	 args;
    exception    excpt;
    char        *format;

    va_start(args);
    excpt  = va_arg(args, exception);
    format = va_arg(args, char *);
#endif

    if ( format == NULL ) excstr[0] = '\0';
    excexe(excpt, format, args);
}


/* Return the error message to the user
 */
char           *errmsg()
{
    return excstr;
}


/* Replace the default error handler with one of your own
 */
evector excvec(handle)
    evector handle;
{
    evector excswp;

    excswp = exchdl;
    exchdl = handle;

    return excswp;
}


/* Process an exception
 */
static void excexe(exc, format, args)
    exception	 exc;
    char	*format;
    va_list      args;
{
	char	str[ERRSTRLEN];
    	int	n = 0;

    if ( format ) {
	n += vsprintf(&str[n], format, args);
        strcpy(excstr, str);
    }

    excval = exc;
    excjmp();
}

void excjmp()
{
    /* If there is no handler set or fatal bail out to the exception handler 
     */
    if ( excstk == NULL || excval == ex_fatal)
	exit((*exchdl)(excval));
    else
	longjmp(excstk->jmp, 1);
}


/* Default error handler (printf and die).
 */
static int      excdie(exc)
	exception exc;
{
   	char *str = errmsg();
	int	n = strlen(str);

    if ( str[n-1] == '\n' ) n--;
    if ( str[n-1] != '.' ) {
	str[n++]   = '.';
    }
    str[n++] = '\0';

    fprintf(stderr, "%s: %s\n", exc, str);

    return 1;
}

#include <errno.h>
char *strerror();

char *exc_oserror()
{
	return strerror(errno);
}

#ifndef HAVE_STRERROR

extern const char *const sys_errlist[];

extern int   sys_nerr;

char *strerror(err)
	int	err;
{
	if ( err >= sys_nerr ) return NULL;
	else		       return (char *) sys_errlist[err];
}
#endif
