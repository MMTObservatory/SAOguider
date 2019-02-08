/* cbinding.c
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

extern void *ParamGetX();
extern char *strncpy();

#define ParamGet(Return, Routine, Type, type, chekindef, retindef)	\
									\
Return Routine(pfile, name)						\
			ParamFile	 pfile;				\
			char		*name;				\
{									\
		void	*value;						\
									\
	if ( value = ParamGetX(pfile, name, Type, &pfile->mode) )	\
	    if ( *((type *) value) == chekindef )			\
		return retindef;					\
	    else							\
		return *((type *) value);				\
	else {								\
		c_paramerr(1, "pget", name);				\
		return 0;						\
	}								\
}


ParamGet(int,	c_pgetb, BooleanType, int, INDEFI, INDEFI)
ParamGet(short, c_pgets, IntegerType, int, INDEFI, INDEFS)
ParamGet(int,	c_pgeti, IntegerType, int, INDEFI, INDEFI)
ParamGet(float, c_pgetf, RealType,    real,INDEFD, INDEFR)
ParamGet(double,c_pgetd, RealType,    real,INDEFD, INDEFD)

char *c_pgetstr(pfile, name, str, max)
			ParamFile	pfile;
			char 	*name;
			char	*str;
			int	max;
{
		void	*value;

	if ( value = ParamGetX(pfile, name, StringType, &pfile->mode) )
		return strncpy(str, value, max);
	else
		c_paramerr(1, "pget", name);
	return NULL;
}


#define ParamPut(Take, Routine, Type, type, chekindef, putindef)	\
									\
void Routine(pfile, name, value)					\
			ParamFile	 pfile;				\
			char		*name;				\
			Take		 value;				\
{									\
		type xvalue;						\
									\
	if ( value == chekindef ) xvalue = putindef;			\
	else			  xvalue = value;			\
									\
	if ( !ParamSetX(pfile, name, Type, &xvalue) )			\
		c_paramerr(1, "pset", name);				\
}


ParamPut(int,	c_pputb, BooleanType, int, INDEFI, INDEFI)
ParamPut(short,	c_pputs, IntegerType, int, INDEFS, INDEFI)
ParamPut(int,	c_pputi, IntegerType, int, INDEFI, INDEFI)
ParamPut(float,	c_pputf, RealType,    real,INDEFR, INDEFD)
ParamPut(double,c_pputd, RealType,    real,INDEFD, INDEFD)

void c_pputstr(pfile, name, string)
			ParamFile	 pfile;
			char 		*name;
			char		*string;
{
	if ( !ParamSetX(pfile, name, StringType, string) )
		c_paramerr(1, "pset", name);
}
