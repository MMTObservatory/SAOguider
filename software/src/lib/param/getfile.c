/* getfile.c
*/

/*
 *      Copyrights:
 *
 *    	      Copyright (c) 1996 Smithsonian Astrophysical Observatory
 *
 *        Permission to use, copy, modify, distribute, and  sell  this
 *        software  and  its  documentation  for any purpose is hereby
 *        granted without  fee,  provided  that  the  above  copyright
 *        notice  appear  in  all  copies and that both that copyright
 *        notice and this permission notice appear in supporting docu-
 *        mentation,  and  that  the  name  of the  Smithsonian Astro-
 *	  physical Observatory not be used in advertising or publicity
 *  	  pertaining to distribution of the software without specific,
 *	  written  prior  permission.   The Smithsonian  Astrophysical 
 *        Observatory makes no representations about  the  suitability
 *	  of  this  software for any purpose.  It is provided  "as is"
 *	  without express or implied warranty.
 *        THE  SMITHSONIAN  ASTROPHYSICAL  OBSERVATORY  DISCLAIMS  ALL
 *	  WARRANTIES  WITH  REGARD  TO  THIS  SOFTWARE,  INCLUDING ALL
 *	  IMPLIED  WARRANTIES  OF  MERCHANTABILITY AND FITNESS, IN  NO
 *	  EVENT SHALL THE  SMITHSONIAN  ASTROPHYSICAL  OBSERVATORY  BE 
 *	  LIABLE FOR  ANY SPECIAL, INDIRECT  OR  CONSEQUENTIAL DAMAGES
 *	  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS  OF USE,  DATA
 *	  OR  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 *	  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 *	  THE  USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <string.h>
#include <stdlib.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "putil.h"

char	NameBuffer[1024];

char *fileextn(name, elst)
	char	*name;
	char	*elst;
{
	    char	*extn;
	    char	*ext1;
	    char	*here;

	    char	Extn[1024];

	strcpy(NameBuffer, name);
	name = NameBuffer;

	strcpy(Extn, elst);
	elst = Extn;

	for ( ext1 = extn = strtok(elst, " :;");
	      extn; 
	      extn = strtok(NULL, " :;") ) {
	    if ( (here = strstr(name, extn)) && !strcmp(extn, here) ) {
		 return name;
	    }
	}

	return strcat(name, ext1);
}

char *fileroot(root, rext)
	char	*root;
	char	*rext;
{
	char	*here;
	char	*extn;

	char	Extn[1024];

    if ( root == NULL ) return NULL;

    strcpy(NameBuffer, root);
    root = NameBuffer;

    if ( rext ) {
	strcpy(Extn, rext);
	rext = Extn;
    }

    if ( here = strrchr(root, '/') )
	    root = here +1;

    if ( rext )
	for ( extn = strtok(rext, " :;"); extn; extn = strtok(NULL, " :;") ) {
	    if ( (here = strstr(root, extn)) && !strcmp(extn, here) ) {
		 *here = '\0';
		 return root;
	    }
	}

    return root;
}

void *pfilename(p, name, mode, file, root, rext, extn, path, code, data)
	ParamFile	 p;
	char		*name;
	char		*mode;
	char		*file;
	char		*root;
	char		*rext;
	char		*extn;
	char		*path;
	void	      *(*code)();
	void		*data;
{
	char	 value[FIELD_SZ];
	char	*filename;
	char	 rootbuffer[1024];


	value[0] = '\0';

	if ( root ) {
	    strcpy(rootbuffer, root);
	    root = rootbuffer;
	}

	if ( file ) *file = '\0';

	while ( *value == '\0' )
	    pgetstr(p, name, value, FIELD_SZ);

	if ( !strcmp(value, "NONE" ) ) 		   return NULL;
	if ( !strcmp(value, "none" ) ) 		   return NULL;


	if ( !strcmp(value, "STDIN") ) 		   return stdin;
	if ( !strcmp(value, "stdin") ) 		   return stdin;
	if ( !strcmp(value, "STDOUT") ) 	   return stdout;
	if ( !strcmp(value, "stdout") ) 	   return stdout;
	if ( !strcmp(value, "stderr") ) 	   return stderr;
	if ( !strcmp(value, "STDERR") ) 	   return stderr;

	if ( !strcmp(value, "-") && *mode == 'r' ) return stdin;
	if ( !strcmp(value, "-") && *mode == 'w' ) return stdout;

	root = fileroot(root, rext);

	if ( !strcmp(value, "INDEF" ) )
	    if ( root == NULL ) return NULL;
	    else
		strcpy(value, root);
	
	if ( (filename = Find(value, mode, extn, path)) == NULL ) {
	    if ( *mode == 'w' )
		filename = fileextn(value, extn);
	    else {
		return NULL;
	    }
	}

	if ( file ) 
	    strcpy(file, filename);
		
	return (*code)(filename, data);
}

FILE *openfile(filename, mode, parameter)
	char	*filename;
	char	*mode;
	char	*parameter;
{
	FILE	*fp;

	if ( (fp = fopen(filename, mode)) == NULL ) {
		fprintf(stderr, "error opening file: %s, the value of parameter: %s\n"
			, filename, parameter);
                perror("pgetfile");
                exit(1);
        }
	return fp;
}

FILE *pgetfile(p, name, mode, file, root, rext, extn, path)
	ParamFile	 p;
	char		*name;
	char		*mode;
	char		*file;
	char		*root;
	char		*rext;
	char		*extn;
	char		*path;
{
	return pfilename(p, name, mode, file, root, rext, extn, path, openfile, mode);
}

