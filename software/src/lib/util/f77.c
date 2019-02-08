/* f77.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */

#define MAX_ARGC	 512
#define MAX_LINE	4096

static int      SAOF77argc;
static char   **SAOF77argv = 0;

#include "xos.h"

#define SAOCStringLength	1024

char            SAOCString1[SAOCStringLength];
char            SAOCString2[SAOCStringLength];
char            SAOCString3[SAOCStringLength];

char           *SAOstrf2c(length, string, cstring)
    int             length;
    char           *string;
    char           *cstring;
{
    while (length && string[--length] == ' ');
    if (string[length] != ' ')
	length++;

    memcpy(cstring, string, length);
    cstring[length] = '\0';

    return cstring;
}

void            SAOstrc2f(cstring, fstring, length)
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

char          **SAOGetF77argv(argc)
    int            *argc;
{

    int             request = JPI$_IMAGNAME;
    int             length;
    char            buffer[MAX_LINE + 1];
    int             i;

    struct dsc$descriptor_s line = {
	MAX_LINE, DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer
    };

    if (SAOF77argv) {
	*argc = SAOF77argc;
	return SAOF77argv;
    }
    Malloc(SAOF77argv, sizeof(char *) * MAX_ARGC);

    /* Get Job Process Information - Image Name */
    length = MAX_LINE;
    lib$getjpi(&request, NULL, NULL, NULL, &line, &length);
    buffer[length] = '\0';

    Malloc(SAOF77argv[0], length + 1);
    strcpy(SAOF77argv[0], buffer);

    /* Get Foriegn command line test */
    length = MAX_LINE;
    lib$get_foreign(&line, 0, &length);
    buffer[length] = '\0';

    for (i = 0; i < length; i++)
	if (isupper(buffer[i]))
	    buffer[i] = tolower(buffer[i]);

    Malloc(SAOF77argv[1], length + 1);
    strcpy(SAOF77argv[1], buffer);

    SAOF77argc = *argc
	= parsline(SAOF77argv[1], length, &SAOF77argv[1], MAX_ARGC)
	+ 1;
    ReAlloc(SAOF77argv, sizeof(char *) * SAOF77argc);

    return SAOF77argv;
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

/* Fake the normal unix command line acces for any interested parties

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

    if (!SAOF77argv)
	argv = SAOGetF77argv(&argc);

    SAOstrc2f(argv[*k], arg->dsc$a_pointer, arg->dsc$w_length);
}

int             iargc()
{
    int             argc;
    char          **argv;

    if (!SAOF77argv)
	argv = SAOGetF77argv(&argc);

    return argc - 1;
}

#else

int  iargc_();
void getarg_();

char          **SAOGetF77argv(argc)
    int            *argc;
{
    int             i;

    if (SAOF77argv) {
	*argc = SAOF77argc;
	return SAOF77argv;
    }
    Malloc(SAOF77argv, sizeof(char *) * MAX_ARGC);

    *argc = SAOF77argc = iargc_() + 1;

    for (i = 0; i < SAOF77argc; i++) {
	Malloc(SAOF77argv[i], MAX_LINE);
	getarg_(&i, SAOF77argv[i], MAX_LINE);
	SAOstrf2c(MAX_LINE, SAOF77argv[i], SAOF77argv[i]);
	SAOF77argv[i] = (char *) ReAlloc(SAOF77argv[i]
	,strlen(SAOF77argv[i]) + 1); }

    SAOF77argv = (char **) ReAlloc(SAOF77argv, sizeof(char *) * i);

    return SAOF77argv;
}

#endif
