/* unxfbind.c
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

void c_paramlist();
extern char	**GetF77argv();
extern char	 *strf2c();
extern void	  strc2f();
extern char	  CString1[];
extern char	  CString2[];

#define F2CStr	strf2c
#define C2FStr	strc2f

#define F2CStr1(length, string)	F2CStr(length, string, CString1)
#define F2CStr2(length, string)	F2CStr(length, string, CString2)

void paramopen_(filename, mode, pf, nfilename, nmode)
			char		*filename;
			char		*mode;
			paramfile 	*pf;
			int	 	 nfilename;
			int	 	 nmode;
	{
		int	  argc;
		char	**argv;

		char	 *cmode;

	argv = GetF77argv(&argc);

	if ( !strpbrk(cmode = F2CStr1(nmode, mode), "$") )
		argc = 1;

	*pf = c_paramopen( (!filename
			 ||  filename && *filename == '$' && nfilename == 1 )
			? NULL : F2CStr2(nfilename, filename)
			, argv, argc, cmode);
}

void paramlist_(pfile)
			paramfile *pfile;
{
	paramlist(*pfile);
}


void paramclose_(pfile)
			paramfile *pfile;
{
	paramclose(*pfile);
}

paramfind_(name, path, nname, npath)
			char	*name;
			char	*path;
			int	 nname;
			int	 npath;
{
  C2FStr(paramfind(F2CStr1(nname, name), NULL, NULL, NULL), path, npath);
}

paramerr_(level, message, name, nmessage, nname)
			int	*level;
			char	*message;
			char	*name;
			int	nmessage;
			int	nname;
{
	paramerr(*level, F2CStr1(nmessage, message), F2CStr2(nname, name));
}

paramerrstr_(errstr, nerrstr)
			char	*errstr;
			int	 nerrstr;
{
	C2FStr(paramerrstr(), errstr, nerrstr);
}

#define ParamGet(Return, Routine, Type, type, chekindef, retindef)	\
									\
void Routine(pfile, name, value, status, nname)				\
			ParamFile	*pfile;				\
			char		*name;				\
			Return		*value;				\
			int		*status;			\
			int		 nname;				\
{									\
		type	*v;						\
									\
	*status = 0;							\
									\
	if ( v = ParamGetX(*pfile, F2CStr1(nname, name)			\
			 , Type, &(*pfile)->mode))			\
	    if (*v == chekindef ) {					\
		*status = 3;						\
		*value  = retindef;					\
	    } else							\
	    	*value  = *v;						\
	else								\
		*status = 1;						\
									\
}


ParamGet(int,	pgetb_, BooleanType, int, INDEFI, INDEFI)
ParamGet(short, pgets_, IntegerType, int, INDEFI, INDEFS)
ParamGet(int,	pgeti_, IntegerType, int, INDEFI, INDEFI)
ParamGet(int,	pgetl_, IntegerType, int, INDEFI, INDEFI)
ParamGet(float, pgetr_, RealType,    real,INDEFD, INDEFR)
ParamGet(float, pgetf_, RealType,    real,INDEFD, INDEFR)
ParamGet(double,pgetd_, RealType,    real,INDEFD, INDEFD)

void pgetstr_(pfile, name, string, status, nname, nstring)
			ParamFile	*pfile;
			char 		*name;
			char		*string;
			int		*status;
			int		 nname;
			int		 nstring;
{
		void	*v;

	*status = 0;

	if ( v = ParamGetX(*pfile, F2CStr1(nname, name), StringType, &(*pfile)->mode))
		C2FStr(v, string, nstring);
	else
		*status = 1;
}


#define ParamPut(Take, Routine, Type, type, chekindef, putindef)	\
									\
void Routine(pfile, name, value, status, nname)				\
			ParamFile	*pfile;				\
			char		*name;				\
			Take		*value;				\
			int		*status;			\
			int		 nname;				\
{									\
		type xvalue = *value;					\
									\
	*status = 0;							\
									\
	if ( *value == chekindef ) xvalue = putindef;			\
	else			   xvalue = xvalue;			\
									\
	if ( !ParamSetX(*pfile, F2CStr1(nname, name), Type, &xvalue) )	\
		*status = 1;						\
}


ParamPut(int,	pputb_, BooleanType, int, INDEFI, INDEFI)
ParamPut(short,	pputs_, IntegerType, int, INDEFS, INDEFI)
ParamPut(int,	pputi_, IntegerType, int, INDEFI, INDEFI)
ParamPut(int,	pputl_, IntegerType, int, INDEFI, INDEFI)
ParamPut(float,	pputr_, RealType,    real,INDEFR, INDEFD)
ParamPut(float,	pputf_, RealType,    real,INDEFR, INDEFD)
ParamPut(double,pputd_, RealType,    real,INDEFD, INDEFD)

void pputstr_(pfile, name, string, status, nname, nstring)
			ParamFile	*pfile;
			char 		*name;
			char		*string;
			int		*status;
			int		 nname;
			int		 nstring;
{
	*status = 0;

	if ( !ParamSetX(*pfile, F2CStr1(nname, name), StringType
			, F2CStr2(nstring, string)) )
		*status = 1;
}
