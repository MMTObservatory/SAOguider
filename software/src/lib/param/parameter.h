/* parameter.h
**
** API to parameter file routines
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


#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "parambind.h"


#ifndef PARAMETER_FILE_SOURCE
typedef void *paramfile;		/* Make the Param data types opaque */
typedef void *pmatchlist;
#else
typedef ParamFile paramfile;
typedef ParamMatchList *pmatchlist;
#endif

extern int tmatch();

extern paramfile PFFile;		/* Global parameter file	    */
extern paramfile PFList;		/* Global parameter file list	    */

#ifndef INDEFS
#define INDEFS          (-32767)
#endif
#ifndef INDEFL
#define INDEFL          ((unsigned long) 0x80000001)
#endif
#ifndef INDEFI
#define INDEFI          INDEFL
#endif
#ifndef INDEFR
#define INDEFR          1.6e38
#endif
#ifndef INDEFD
#define INDEFD          1.6e38
#endif
#ifndef INDEF
#define INDEF           INDEFR
#endif

/* Routines to manipulate parameter files

  paramopen	Open a parameter file and return a paramfile handle

	The argc and argv arguments are the C main's arguments.  If
	you don't want the command line arguments folded into the 
	parameter file pass them as NULL and 0.
	
  paramfind	Search for a parameter file and return its full path

	The extn and path arguments are lists of allowable extension and 
	directories to attempt to search for parameter files.  The default
	search in paramopen is:

	extn = ".par .rdb $PFEXTN"
	path = "$PDIRS $PFILES $UPARM"

	Identifiers prefixed with "$" are recursivly expanded in the run
	time environment.  For comaptiblity with IRAF $UPARM should be set 
	to a single directory name with a trailing "/".  $PFILES may be
	set to a space or "," separated list of directories.

  paramclose	Close the paramfile handle

**/	

paramfile paramopen(/* char *filename, char **argv, int argc, char *mode */);
paramfile paramstropen(/* char *str, char *strname, char *mode */);
char	 *paramfind(/* char *filename, char *mode, char *extn, char*path, NULL */);
void	  paramclose(/* paramfile pfile */);
   

/* Error handling
**/
void	  paramerr();		/* Report an error to the user		*/
char	 *paramerrstr();	/* Return the current error string	*/


/* Check if a string type value is valid for this parameter
*/
int	paramvalid(/* paramfile pfile, char *pname, char *pvalue */);
int	paccess(/*paramfile pfile, char *pname */);

/* Get  parameter values from the parameter file.
**/
int 	pgetb(/* paramfile pfile, char *pname */);
short 	pgets(/* paramfile pfile, char *pname */);
int 	pgeti(/* paramfile pfile, char *pname */);
long 	pgetl(/* paramfile pfile, char *pname */);
float	pgetf(/* paramfile pfile, char *pname */);
double 	pgetd(/* paramfile pfile, char *pname */);
char *pgetstr(/* paramfile pfile, char *pname, char *string, int length */);


/* Put parameter values into the parameter file.
**/
void pputb(/* paramfile pfile, char *pname, int     bvalue */);
void pputs(/* paramfile pfile, char *pname, short   svalue */);
void pputi(/* paramfile pfile, char *pname, int     ivalue */);
void pputl(/* paramfile pfile, char *pname, long    lvalue */);
void pputf(/* paramfile pfile, char *pname, float   fvalue */);
void pputd(/* paramfile pfile, char *pname, double  dvalue */);
void pputstr(/* paramfile pfile, char *pname, char *string */);


/* Read a list of parameter names from the parameter file
**
	pmatchopen builds a list of parameter names which match the
	supplied template.  pmatchnext returns each name in turn on
	sucessive calls.

	template meta-characters:

	?   		match any character, but there must be one
	*		match anything, or nothing
	[<c>...]	match an inclusive set

**/
pmatchlist	 pmatchopen(/* paramfile pfile, char *template */);
char		*pmatchnext(/* pmatchlist mlist */);
int	         pmatchlength(/* pmatchlist mlist */);
void	         pmatchrewind(/* pmatchlist mlist */);
void		 pmatchclose(/* pmatchlist mlist */);
char 		*pfilelast();

/* Return the string representation of the named parameter's field's
**
	any of *mode, *type, *value, *min, *max, *prompt that are
	passed as NULL are ignored.  Otherwise they should point
	to enough memory to hold the string representation of the
	parameter field.  In general SZ_PFLINE is recomended.
**/

/*
int paraminfo(paramfile pfile, char *pname
		 char *mode, *type, *value, *min, *max, *prompt);
*/


/* The default parameter file interface
**/
#define clinit(argc, argv, mode)  ( (PFFile = paramopen(NULL, argv, argc, mode)) == NULL ? paramerr(1, "clinit", pfilelast()), PFFile : PFFile)

#define clgetb(pname)			pgetb(PFFile, pname)
#define clgets(pname)			pgets(PFFile, pname)
#define clgeti(pname)			pgeti(PFFile, pname)
#define clgetf(pname)			pgetf(PFFile, pname)
#define clgetd(pname)			pgetd(PFFile, pname)
#define clgetstr(pname, string, max)    pgetstr(PFFile, pname, string, max)
#define clgstr(pname, string, max)	pgetstr(PFFile, pname, string, max)

#define clputb(pname, value)		pputb(PFFile, pname, value)
#define clputs(pname, value)		pputs(PFFile, pname, value)
#define clputi(pname, value)		pputi(PFFile, pname, value)
#define clputf(pname, value)		pputf(PFFile, pname, value)
#define clputd(pname, value)		pputd(PFFile, pname, value)
#define clputstr(pname, string)		pputstr(PFFile, pname, string)
#define clpstr(pname, string)		pputstr(PFFile, pname, string)

int   pgetenum(/* paramfile pfile, char *pname, char *dict */);
FILE *pgetfile(/* paramfile pfile, char *pname, char *mode, char *extn, char *path */);
void *pfilename(/* paramfile pfile, char *pname, char *mode, char *extn, char *path, code, data */);
 
#define clgetenum(pname, dict)                  pgetenum(PFFile, pname, dict)
#define clgetfile(pname, mode, file, root, rext, extn, path)    \
        pgetfile(PFFile, pname, mode, file, root, rext, extn, path)
#define clfilename(pname, mode, file, root, rext, extn, path, code, data)    \
        pfilename(PFFile, pname, mode, file, root, rext, extn, path, code, data)

#define clclose() paramclose(PFFile)


/* pp here is of type paramfile
 * so should be declared in the
 * calling code as..
 *
 * paramfile pp;
 */

#define clopset(pset, mode)  (paramopen(pset, "", 0, mode))
#define clcpset(pp)                     paramclose(pp)

#define clgpsetb(pp, param)             pgetb(pp, param)
#define clgpsetc(pp, param)             pgetc(pp, param)
#define clgpsets(pp, param)             pgets(pp, param)
#define clgpseti(pp, param)             pgeti(pp, param)
#define clgpsetl(pp, param)             pgetl(pp, param)
#define clgpsetr(pp, param)             pgetr(pp, param)
#define clgpsetd(pp, param)             pgetd(pp, param)
#define clgpsetx(pp, param)             pgetx(pp, param)
#define clgpset(pp, pname, string, max)     pgetstr(pp, pname, string, max)

#define clppsetb(pp, param, pval)       pputb(pp, param, pval)
#define clppsetc(pp, param, pval)       pputc(pp, param, pval)
#define clppsets(pp, param, pval)       pputs(pp, param, pval)
#define clppseti(pp, param, pval)       pputi(pp, param, pval)
#define clppsetl(pp, param, pval)       pputl(pp, param, pval)
#define clppsetr(pp, param, pval)       pputr(pp, param, pval)
#define clppsetd(pp, param, pval)       pputd(pp, param, pval)
#define clppsetx(pp, param, pval)       pputx(pp, param, pval)
#define clppset(pp, pname, string)	pputstr(pp, pname, string)


#ifndef SZ_PFLINE
#define SZ_PFLINE	160
#endif

#endif
