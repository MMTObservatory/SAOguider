/* clfg.c

 Get parameters from a formatted parameter name

 */
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>

#undef PARAMETER_FILE_SOURCE
#include "parameter.h"


#ifdef __STDC__
int clfgetb(char *format, ...)
#else
int clfgetb(va_alist)
	va_dcl
#endif
{
        va_list args;
	char	param[SZ_PFLINE];

#ifdef __STDC__
        va_start(args, format);
#else
	char	*format;

	va_start(args);

	format = va_arg(args, char *);
#endif

    vsprintf(param, format, args);
    return clgetb(param);
}

#ifdef __STDC__
int clfgets(char *format, ...)
#else
int clfgets(va_alist)
	va_dcl
#endif
{
        va_list args;
	char	param[SZ_PFLINE];

#ifdef __STDC__
        va_start(args, format);
#else
	char	*format;

	va_start(args);

	format = va_arg(args, char *);
#endif

    vsprintf(param, format, args);
    return clgets(param);
}


#ifdef __STDC__
int clfgeti(char *format, ...)
#else
int clfgeti(va_alist)
	va_dcl
#endif
{
        va_list args;
	char	param[SZ_PFLINE];

#ifdef __STDC__
        va_start(args, format);
#else
	char	*format;

	va_start(args);

	format = va_arg(args, char *);
#endif

    vsprintf(param, format, args);
    return clgeti(param);
}



#ifdef __STDC__
double clfgetd(char *format, ...)
#else
double clfgetd(va_alist)
	va_dcl
#endif
{
        va_list args;
	char	param[SZ_PFLINE];

#ifdef __STDC__
        va_start(args, format);
#else
	char	*format;

	va_start(args);

	format = va_arg(args, char *);
#endif

    vsprintf(param, format, args);
    return clgetd(param);
}

#ifdef __STDC__
char *clfgstr(char *format, char *value, int length, ...)
#else
char *clfgstr(va_alist)
	va_dcl
#endif
{
        va_list args;
	char	param[SZ_PFLINE];

#ifdef __STDC__
        va_start(args, length);
#else
	char	*format;
	char	*value;
	int	 length;

	va_start(args);

	format = va_arg(args, char *);
	value  = va_arg(args, char *);
	length = va_arg(args, int);
#endif

    vsprintf(param, format, args);
    return clgstr(param, value, length);
}
