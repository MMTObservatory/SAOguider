/* f77.c
**
**
**
**/

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


#include <stdlib.h>
#include <string.h>
#define bcopy(s, d, n) memcpy( (d), (s), (n))

#define MAX_ARGC	 512
#define MAX_LINE	4096

static int      F77argc;
static char   **F77argv = 0;

#define CStringLength	1024

char            CString1[CStringLength];
char            CString2[CStringLength];

char           *strf2c(length, string, cstring)
    int             length;
    char           *string;
    char           *cstring;
{
    while (length && string[--length] == ' ');
    if (string[length] != ' ')
	length++;

    bcopy(string, cstring, length);
    cstring[length] = '\0';

    return cstring;
}

void            strc2f(cstring, fstring, length)
    char           *cstring;
    char           *fstring;
    int             length;
{
    int             i;

    /* Copy the C string, then padd with ' ' */
    for (i = 0; *cstring; i++)
	*fstring++ = *cstring++;
    for (; i < length; i++)
	*fstring++ = ' ';
}

#ifdef vms

#include jpidef.h
#include descrip.h

#include ctype.h

char          **GetF77argv(argc)
    int            *argc;
{

    int             request = JPI$_IMAGNAME;
    int             length;
    char            buffer[MAX_LINE + 1];
    int             i;

    struct dsc$descriptor_s line = {
	MAX_LINE, DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer
    };

    if (F77argv) {
	*argc = F77argc;
	return F77argv;
    }
    F77argv = (char **)malloc(sizeof(char *) * MAX_ARGC);

    /* Get Job Process Information - Image Name */
    length = MAX_LINE;
    lib$getjpi(&request, NULL, NULL, NULL, &line, &length);
    buffer[length] = '\0';

    F77argv[0] = (char *)malloc(length + 1);
    strcpy(F77argv[0], buffer);

    /* Get Foriegn command line test */
    length = MAX_LINE;
    lib$get_foreign(&line, 0, &length);
    buffer[length] = '\0';

    for (i = 0; i < length; i++)
	if (isupper(buffer[i]))
	    buffer[i] = tolower(buffer[i]);

    F77argv[1] = (char *)malloc(length + 1);
    strcpy(F77argv[1], buffer);

    F77argc = *argc
	= parsline(F77argv[1], length, &F77argv[1], MAX_ARGC)
	+ 1;
    F77argv = (char **)realloc(F77argv, sizeof(char *) * F77argc);

    return F77argv;
}

static int      parsline(buffer, nbuf, column, ncol)
    char           *buffer;
    int             nbuf;
    char          **column;
    int             ncol;
{
    int             i, j;

    if (!nbuf)
	return 0;

    for (i = 0, j = 0; i < nbuf && j <= ncol; j++) {
	column[j] = &buffer[i];

	while (buffer[i] != ' '
		&& buffer[i] != '\t'
		&& buffer[i] != '\0'
		&& i < nbuf)
	    i++;

	if (buffer[i] == '\0')
	    break;
	buffer[i++] = '\0';
    }

    buffer[i] = '\0';
    column[j + 1] = &buffer[i + 1];

    return j + 1;
}

/* Fake the normal unix command line access for any interested parties

   NAME
	getarg, iargc - get the kth command line argument
   SYNOPSIS
     subroutine getarg ( k, arg )
     character*(*) arg

     function iargc ()

   DESCRIPTION
     The statement call getarg( k  ,  arg  )  will  get  the  kth
     command-line argument and put it into arg .

     The 0th argument is the command name.

     The function iargc returns the index of  the  last  command-
     line argument, and therefore the number  of  arguments  after
     the  command name.
**/

void            getarg(k, arg)
    int            *k;
    struct dsc$descriptor_d *arg;
{
    int             argc;
    char          **argv;

    if (F77argv==NULL)
	argv = GetF77argv(&argc);

    strc2f(argv[*k], arg->dsc$a_pointer, arg->dsc$w_length);
}

int             iargc()
{
    int             argc;
    char          **argv;

    if (F77argv==NULL)
	argv = GetF77argv(&argc);

    return argc - 1;
}

#else

char          **GetF77argv(argc)
    int            *argc;
{
    int             i;

    if (F77argv) {
	*argc = F77argc;
	return F77argv;
    }
    F77argv = (char **)malloc(sizeof(char *) * MAX_ARGC);

    *argc = F77argc = iargc_() + 1;

    for (i = 0; i < F77argc; i++) {
	F77argv[i] = (char *)malloc(MAX_LINE);
	getarg_(&i, F77argv[i], MAX_LINE);
	strf2c(MAX_LINE, F77argv[i], F77argv[i]);
	F77argv[i] = (char *) realloc(F77argv[i]
					 ,strlen(F77argv[i]) + 1);
    }

    F77argv = (char **) realloc(F77argv, sizeof(char *) * i);

    return F77argv;
}

#endif
