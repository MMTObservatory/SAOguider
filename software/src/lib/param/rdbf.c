/* rdb.c
**
	Routines to manipulate rdb parameter files.
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

#include <string.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "putil.h"

static ParamList	*RDBParRead();
       void		 RDBParWrit();
static void		 RDBParW();

extern ParamList	*RDBParFile();
extern ParamList	*IRAFParArgs();

/*
	The mode argument:

		"l"	lock file for exclusive access.
*/

ParamList *RDBParOpen(psetname, filename, mode)
     char	*psetname;
     char	*filename;
     char	*mode;
{
  FILE		*f;
  ParamList	*plist;
  char		*fmode = strpbrk(mode, "w" ) == NULL ? 
    "r" : "r+";

  if ( filename == NULL ) return NULL;

  if ( (f = fopen(Find(filename, fmode, ".rdb", PARAMPATH), fmode)) == NULL )
    return NULL;

	if ( strpbrk(mode, "l" ) ) Lock(f);

	if ( (plist = RDBParFile(f)) == NULL )
		return (ParamList *) ParamFileCant;

	plist->file = f;
  	plist->filename = NewString(filename);
  	plist->psetname = NewString(psetname);
  	plist->mode = NewString(mode);
	plist->read = RDBParRead;
	plist->writ = RDBParWrit;
	plist->clos = NULL;

	return plist;
}


static ParamList *RDBParRead(file, mode)
			FILE	*file;
			char	*mode;
{
	if ( strpbrk(mode, "l") ) Lock(file);
	fseek(file, 0, 0);


	return RDBParFile(file);
}


void RDBParWrit(file, plist, mode)
			FILE		*file;
			ParamList	*plist;
			char		*mode;
{
		int	i;

	fseek(file, 0, 0);

	fprintf(file, "name\ttype\tmode\tvalue\tmin\tmax\tprompt\n");
	fprintf(file, "----\t----\t----\t-----\t---\t---\t------\n");

	for ( i = 0; i < plist->nparam; i++ )
		RDBParW(plist->param[i], file);

	fflush(file);
	ftruncate(fileno(file), ftell(file));
}

static void RDBParW(p, f)
			Parameter	*p;
			FILE		*f;
{
		char	namestr[SZ_PFLINE];
		char	typestr[SZ_PFLINE];
		char	modestr[SZ_PFLINE];
		char	valustr[SZ_PFLINE];
		char	minnstr[SZ_PFLINE];
		char	maxxstr[SZ_PFLINE];
		char	promstr[SZ_PFLINE];

	ParamEntry(NULL, p, namestr, modestr, typestr
			, valustr,minnstr, maxxstr, promstr);

        outputstr(f, namestr);
	putc('\t', f);
        outputstr(f, typestr);
	putc('\t', f);
        outputstr(f, modestr);
	putc('\t', f);
        outputstr(f, valustr);
	putc('\t', f);
        outputstr(f, minnstr);
	putc('\t', f);
        outputstr(f, maxxstr);
	putc('\t', f);
        outputstr(f, promstr);
	putc('\n', f);
}

outputstr(f, str)
	FILE	*f;
	char	*str;
{
	while ( *str ) {
	    switch ( *str ) {
	     case '\t': putc('\\', f);
			putc('t', f);
			break;
	     case '\n': putc('\\', f);
			putc('n', f);
			break;
	     default:
	    		putc(*str, f);
			break;

	    }
	    str++;
	}
}

