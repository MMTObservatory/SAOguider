/* xfile.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */

#include "xos.h"
#include "xfile.h"

extern struct _xFileClass xFilePipe;

#ifdef __STDC__
int EPrint(char *format, ...)
#else
int EPrint(va_alist) va_dcl
#endif
{
	va_list	 args;
	File	 file = Stderr;

#ifdef __STDC__
	va_start(args, format);
#else
	char	*format;
	va_start(args);
	format = va_arg(args, char *);
#endif

	return vfprintf(file, format, args);
}

static File XFileFile = NULL;

File XFile(xfile)
	File	xfile;
{
   if ( xfile ) {
	File	ofile = XFileFile;
	XFileFile = xfile;

	return  ofile;
   } else {
	return XFileFile;
   }
}

#ifdef __STDC__
int XPrint(char *format, ...)
#else
int XPrint(va_alist) va_dcl
#endif
{
	va_list	 args;
	File	 file = XFileFile;
	int	 n;

#ifdef __STDC__
	va_start(args, format);
#else
	char	*format;
	va_start(args);
	format = va_arg(args, char *);
#endif

	if ( file == NULL ) file = stderr;

	n = vfprintf(file, format, args);
	Flush(file);

	return n;
}

char *xExtn = ".gz:.Z";

static int xFileZipLevel = 1;
int xFileZip(n) {
    int x = xFileZipLevel;
    xFileZipLevel = n;
    return x;
}


File  xOpen(name, mode)
	char	*name;
	char	*mode;
{
	File f;

    if ( !strcmp(name, "-") && !strcmp(mode, "r") ) {
	xFileInit(stdin, NULL);
	return stdin;
    }
    if ( !strcmp(name, "-") && !strcmp(mode, "w") ) {
	xFileInit(stdout, NULL);
	return stdout;
    }
    if ( *name == '|'       && 
	    ( !strcmp(mode, "r") || !strcmp(mode, "w"))) {

	if ( f = popen(&name[1], mode) ) {
	    xFileInit(f, &xFilePipe);
	}

	return f;
    }

    if ( xExtn ) {
        /* Try to open a list of extnsions with gzip
        */
	char *extn = strdup(xExtn);
	char *here, *ext;

	for ( ext = strtok(extn, ":"); ext; ext = strtok(NULL, ":") ) {
	    if ( (here = strstr(name, ext)) && *(here+strlen(ext)) == '\0' ) {
		    char pipecmd[1024];
		    pipecmd[0] = '\0';

		if ( !strcmp(mode, "r") ) sprintf(pipecmd, "gzip -d  < %s", name);
		if ( !strcmp(mode, "w") ) sprintf(pipecmd, "gzip -%d > %s"
			, xFileZipLevel, name);

		if ( pipecmd[0] )
		    if ( f = popen(pipecmd, mode) ) {
	    		xFileInit(f, &xFilePipe);
			return f;
		    }
	    }
	}
	Free(extn);

	/* Always try compress, maybe gzip isn't in the path?
	 */
	if ( (extn = strstr(name, ".Z") ) && *(extn+2) == '\0' ) {
		char pipecmd[1024];
		pipecmd[0] = '\0';

	    if ( !strcmp(mode, "r") ) sprintf(pipecmd, "compress -d < %s", name);
	    if ( !strcmp(mode, "w") ) sprintf(pipecmd, "compress    > %s", name);

	    if ( pipecmd[0] )
		if ( f = popen(pipecmd, mode) ) {
		    xFileInit(f, &xFilePipe);
		    return f;
		}
	}
    }

    {
	File fp = fopen(name,mode);
	if ( fp ) { xFileInit(fp, NULL); }
        return fp;
    }
}

int FileLeft(f)
	File	f;
{
#if rtmx 
	return (f->_r > 0);
#endif

#if linux 
	return f->_IO_read_ptr < f->_IO_read_end;
#endif

#if _FILE_OFFSET_BITS == 64
	return 0;
#endif

#ifdef __CYGWIN__
	return 0;
#endif

#ifdef Darwin
	return (f->_r);
#endif

#if _FILE_OFFSET_BITS == 64
	return 0;

#else
#if SunOS
	return (f->_cnt);
#endif
#endif
}
