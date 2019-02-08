/* real.c
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

#include "pfile.h"
#include "range.h"

static void *Int2Real(), *String2Real(), *Bool2Real();

static int   Size(), Comp();

static Converter Converters[] = {
	{IntegerType,	RealType,  Int2Real},
	{StringType,	RealType,  String2Real},
	{BooleanType,	RealType,  Bool2Real},
	{0,		0,	   NULL}
};


vtype realRec = {
	"r",
	Converters,

	Size,
	VXAcce,
	Comp
};


static void *String2Real(value, copy)
			char	*value;
			real	*copy;
{
		char	*scanto;
		double strtod();
		

	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	if ( !strcmp(value, "INDEF") || !strcmp(value, "INDEFD") ) {
		*copy = INDEFD;
		return copy;
	}

	*copy = strtod(value, &scanto);

	if ( *scanto ) {
		parerr = PARCANTCONVERT;
		return NULL;
 	}

	return copy;
}

static void *Int2Real(value, copy)
			int	*value;
			real	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	*copy = *value;
	return copy;
}


static void *Bool2Real(value, copy)
			bool	*value;
			real	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	*copy = *value == 1;
	return copy;
}


static int Size(value)
			void *value;
{
	return sizeof(real);
}

static int Comp(pvalue1, pvalue2, type)
			void	*pvalue1;
			void	*pvalue2;
			int	type;
{
  real *value1 = (real *) pvalue1;
  real *value2 = (real *) pvalue2;

	if ( *value1 == INDEFD || *value2 == INDEFD ) return 1;

	switch ( type ) {
	 case PRange_EQ: return *value1 == *value2;
	 case PRange_NE: return *value1 != *value2;
	 case PRange_GT: return *value1 >  *value2;
	 case PRange_GE: return *value1 >= *value2;
	 case PRange_LT: return *value1 <  *value2;
	 case PRange_LE: return *value1 <= *value2;
	 default: return 0;
	}
}
