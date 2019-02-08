/* iraf.c
**
	Routines to manipulate iraf parameter files.
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
#include <sys/types.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "pargs.h"

static ParamList	*IRAFParRead();
       void		 IRAFParWrit();
static void		 IRAFParW();

extern ParamList	*IRAFParFile();
extern ParamList	*IRAFParseStr();
int ParamFree();
int ParamEntry();
void ESCQuote();
char * ESCUnQuote();
char *locateptemplate();
void mkpfilename();
static void mapname ();

extern ParamList *ParamFileCant;

/*
	The mode argument:

		"l"	lock file for exclusive access.
*/

ParamList *IRAFParOpen(psetname, filename, mode)
			char	*psetname;
			char	*filename;
			char	*mode;
{
  FILE		*f;
  ParamList	*plist;
  char		*fmode = strpbrk(mode, "w" ) == NULL ? 
					"r" : "r+";

  if ( filename == NULL ) return NULL;

  if( (f = fopen(filename, fmode)) == NULL ){
    fprintf(stderr, "Open error on param file: %s\n", filename);
    return NULL;
  }

  if ( strpbrk(mode, "l" ) ) Lock(f);

  if ( (plist = IRAFParFile(f)) == NULL ) {
    return (ParamList *) ParamFileCant;
  }

  plist->file = f;
  plist->filename = NewString(filename);
  plist->psetname = NewString(psetname);
  plist->mode = NewString(mode);
  plist->read = IRAFParRead;
  plist->writ = IRAFParWrit;
  plist->clos = NULL;

  return plist;
}


ParamList *IRAFStrOpen(psetname, str)
			char	*psetname;
			char	*str;
{
  ParamList	*plist;

  if ( (plist = IRAFParseStr(str)) == NULL ) {
    return (ParamList *) ParamFileCant;
  }



  plist->file = NULL;

  if ( psetname ) {
		char	*here;

	if ( (here = strstr(psetname, ".par")) && *here == '\0' ) {
		plist->filename = NewString(psetname);
	} else {
		plist->filename = malloc(strlen(psetname)+5);
		strcpy(plist->filename, psetname);
		strcat(plist->filename, ".par");
	}
	plist->psetname = NewString(psetname);
  } else {
	plist->psetname = NULL;
  	plist->filename = NULL;
  }

  plist->read = IRAFParRead;
  plist->writ = IRAFParWrit;
  plist->clos = NULL;

  return plist;
}


static ParamList *IRAFParRead(file, mode)
			FILE	*file;
			char	*mode;
{
	if ( strpbrk(mode, "l") ) Lock(file);
	fseek(file, 0, 0);

	return IRAFParFile(file);
}


void IRAFParWrit(file, plist, mode)
			FILE	 	*file;
			ParamList	*plist;
			char		*mode;
{
		int	i;

	if( fseek(file, 0, 0) )
	  perror("fseek");
	for ( i = 0; i < plist->nparam; i++ )
		IRAFParW(plist->param[i], file);

	fflush(file);
	ftruncate(fileno(file), ftell(file));
}


static void IRAFParW(p, f)
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
  char	strustr[SZ_PFLINE];
  int sent;
  char tbuf[128];

  ParamEntry(NULL, p, namestr, modestr, typestr
	      , valustr,minnstr, maxxstr, promstr);


  if ( p->ptype == CommentType ) {
    fprintf(f, "%s\n", valustr);
    return;
  }

  if ( p->ptype == StructType ) {
    strcpy(strustr, valustr);
    strcpy(valustr, "160");
  }

  ESCQuote(valustr);
  ESCQuote(minnstr);
  ESCQuote(maxxstr);
  ESCQuote(promstr);

  sent = fprintf(f, "%s,%s,%s,%s%s%s,%s,%s,%s%s%s\n"
	 , namestr
	 , typestr
	 , modestr
	 , p->ptype == StringType || p->ptype == FileType ? "\"" : ""
	 , valustr
	 , p->ptype == StringType || p->ptype == FileType ? "\"" : ""
	 , minnstr
	 , maxxstr
	 , promstr[0] ? "\"" : ""
	 , promstr
	 , promstr[0] ? "\"" : "");

  if( sent == -1 ){
    sprintf(tbuf, "could not write param '%s'", namestr);
    perror(tbuf);
  }

  if ( p->ptype == StructType ) {
    ESCQuote(strustr);
    fprintf(f, "%s\n", strustr);
  }
}


Parameter *ParamPositional(plist, position, value)
			ParamList	*plist;
			int		 position;
			char		*value;
{
		Parameter	**p = plist->param;
		Parameter	 *param;
		int i, j;
		char		 *name;

	p = plist->param;

	for ( j = plist->nparam, i = position; j; j--, p++ ) {
		if (  (*p)->ptype == CommentType ) continue;
		if ( ((*p)->pmode & HMODE) == 0 ) i--;
		if ( !i ) break;
	}

	if ( i && !j ) {
		parerr = PARTOOMANYPOS;
		return NULL;
	}

	name = NewString((*p)->pname);

	param = ParamName(NULL,  name);
	param = ParamType(param, (*p)->ptype);
	param = ParamValue(param, StringType, value);

	return param;
}

	
Parameter *ParamCreate(name, type, mode, value, mmptp)
			char		*name;
			VType	 	 type;
			int	 	*mode;
			char		*value;
			Parameter	*mmptp;
{
		Parameter	*param;

        if ( name == NULL ) {
		if ( value ) free(value);
		if ( mmptp ) ParamFree(mmptp);
	  
		return NULL;
	}

	if ( type == 0 ) {
		paramerr(0, "iraf parser", name);

		if ( name  ) free(name);
		if ( value ) free(value);
		if ( mmptp ) ParamFree(mmptp);

		return NULL;
	}

	if ( mode == NULL ) {
		paramerr(0, "iraf parser", name);

		if ( name  ) free(name);
		if ( value ) free(value);
		if ( mmptp ) ParamFree(mmptp);

		return NULL;
	}

	param = ParamName(mmptp, name);
	param = ParamType(param, type);
	param = ParamMode(param, mode);
	param = ParamValue(param, StringType, ESCUnQuote(value));

	return param;
}


VType PLookupType(reference, name)
		ParamList	*reference;
		char		*name;
{	
		Parameter *p;
	
	if ( reference ) {
	    p = ParamLookup(reference, name);

	    if ( p == NULL ) {
		    fprintf(stderr, "parameter not found : %s \n", name);
		    /* return (VType)NULL; */
		    return 0;
	    }

	    return p->ptype;
	} else
	    return StringType;
}


void ESCQuote(str)
     char	*str;
{
  char *t, *k, *s = str;
  int	i;
  int	len = strlen(str)+1;

  if ((str==NULL) || (*str=='\0')) return;

  k = t = (char *)malloc(len + 100);
  *t = '\0';

  while ( *str ) {
    if ( *str == '"' ) {
      *t++ = '\\';
    }
    *t++ = *str++;
  }
  *t = '\0';

  strcpy(s, k);
  free(k);

  return;
}

char *ESCUnQuote(str)
     char *str;
{
  char *t, *k, *s = str;
  int	i;
  int	len;
  
  if ((str==NULL) || (*str=='\0')) return str;
  
  len = strlen(str)+1;

  k = t = (char *)malloc(len + 100);
  *t = '\0';

  while ( *str ) {
    if ( *str == '\\' && *(str+1) == '"') {
      *str++;
    }
    *t++ = *str++;
  }
  *t = '\0';

  strcpy(s, k);
  free(k);

  return s;
}

/* Rip this stuff from /proj/asc/xiraf/irafhack.c
*/

#ifndef vax

#if NeXT
#include <sys/types.h>
typedef void *DIR;
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif

#define SZ_LINE 	SZ_PFLINE
#define SZ_FNAME 	SZ_PFLINE
#define PARAMFILE_EXT ".par"
#define     LEN_PFILENAME   6
#define     LEN_PKPREFIX    3

static char templatebuf[SZ_LINE];

/*
 * locatepfile - look in likely directories for a param file
 *
 * We look in the following directories:
 *
 *	the current directory
 *	the directory pointed to by UPARM environment variable
 *
*/

char *locatepfile(fname)
			char *fname;
{
		char *locateptemplate(), *getenv();
		char *e;
		char	namebuff[SZ_PFLINE];

	strcpy(namebuff, fname);
	e = strstr(namebuff, ".par");

	/* If the loser provides a .par extension we knock it off */
	if ( e && (e[strlen(".par")] == '\0') )
		*e = '\0';

	return locateptemplate(namebuff, getenv("UPARM"));
}
	

/*
 *  locateptemplate
 *   -- look at templates of IRAF task param files in a directory
 */
char *locateptemplate(fname, dname)
     char *fname;
     char *dname;
{
  DIR *dirp;
  struct dirent *direntp;
  char template[132];
  char *dot;
  int got=0;

	if ( dname == NULL ) return NULL;

	/* open the directory */
	if( (dirp = opendir(dname)) == NULL ) return(NULL);

        /* look for the case where package.task was specified as a param */
        if(  (dot=strchr(fname, '.')) != NULL ){
	  *dot++ = '\0';
	  mkpfilename(template, "", fname, dot, ".par");
	}
	/* the template uses matches against any package (the "???" part) */
        else
	  mkpfilename(template, "", "???", fname, ".par");

	/* look for files that match the template */
	while( (direntp = readdir(dirp)) != NULL ){
		if( tmatch(direntp->d_name, template) ){
			got++;
			if( got == 1 )
				/* return the first one */
				sprintf(templatebuf, "%s%s", dname, direntp->d_name);
			else{
				if( got == 2 ){
				  fprintf(stderr, "warning: multiple matches of %s in dir %s\n",
					  fname, dname);
				  fprintf(stderr, "using file %s\n", templatebuf);
				}
				  fprintf(stderr, "ignoring file %s\n", direntp->d_name);
			}
		}
	}

	/* close the directory */
	closedir(dirp);

	/* didn't find parameter file */
	if( got )
		return(templatebuf);
	else
		return(NULL);
}


/* MKPFILENAME -- Generate a parameter file name, given a directory prefix
 * the names of the package and ltask, and the filename extension.  The form
 * of the filename depends upon whether the pfile is to be stored in UPARM.
 * UPARM pfile names have the form "uparm$ // pakltask.par", where `pak' is
 * the package prefix, consisting of the first LEN_PKPREFIX-1 characters of
 * the package name plus the final character, and `ltask' is the ltask name
 * squeezed to LEN_PFILENAME characters.  If not writing to UPARM, we just
 * use the full filename.
 */
void mkpfilename (buf, dir, pkname, ltname, extn)
char	*buf;			/* receives output filename	*/
char	*dir;			/* dir name or prefix		*/
char	*pkname;		/* package name			*/
char	*ltname;		/* ltask name			*/
char	*extn;			/* filename extension		*/
{
	char	temp[SZ_FNAME+1];
	int     len;

	strcpy (buf, dir);		/* start with directory name	*/

	/* add final "/", if necessary */
	len = strlen(buf);
	if( len ){
	  if( buf[len-1] != '/' )
	    strcat (buf, "/");
	}
	mapname (pkname, temp, LEN_PKPREFIX);
	strcat (buf, temp);
	mapname (ltname, temp, LEN_PFILENAME);
	strcat (buf, temp);
	strcat (buf, extn);		/* add extension for pfile	*/
}

/* MAPNAME -- Apply the N+1 mapping convention (first N-1 plus last chars)
 * to generate a name no longer than N characters.  Returns the number of
 * characters generated.
 */
static void mapname (in, out, maxlen)
char	*in;
char	*out;
int	maxlen;
{
	register int ip, op;

	ip = 0;
	op = 0;
	while (op < maxlen-1 && (out[op++] = in[ip++]) != '\0')
	    ;
	if (out[op-1] != '\0') {	/* append last char	*/
	    if (in[ip] != '\0') {
		while (in[ip] != '\0')
		    ip++;
		out[op++] = in[ip-1];
	    }
	    out[op++] = '\0';
	}
}

#endif /* vax */
