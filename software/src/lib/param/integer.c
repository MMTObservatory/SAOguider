/* integer.c
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


#define BASE 10

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "pfile.h"
#include "range.h"

static void *String2Int(), *Real2Int(), *Bool2Int();
static int Comp();

static Converter Converters[] = {
	{StringType,	IntegerType,	String2Int}, 
	{RealType,	IntegerType,	Real2Int},
	{BooleanType,	IntegerType,	Bool2Int},
	{0,		0,   	NULL}
};


vtype integerRec = {
	"i",
	Converters,

	VXSize,
	VXAcce,
	Comp
};

static void *String2Int(value, copy)
			char	*value;
			int	*copy;
{
		char	*scanto = NULL;

	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	if ( !strcmp(value, "INDEF") || !strcmp(value, "INDEFI") ) {
		*copy = INDEFI;
		return copy;
	}

	*copy = strtol(value, &scanto, BASE);

	if ( scanto == NULL
	 || ( *scanto != '\0' && *scanto != '.' ) ) {
		parerr = PARCANTCONVERT;
		return NULL;
 	}

	return copy;
}


static void *Real2Int(value, copy)
			real	*value;
			int	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	*copy = *value;
	return copy;
}


static void *Bool2Int(value, copy)
			bool	*value;
			int	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	*copy = *value == 1;
	return copy;
}


static int Comp(pvalue1, pvalue2, type)
			void	*pvalue1;
			void	*pvalue2;
			int	type;
{
  int *value1 = (int *) pvalue1;
  int *value2 = (int *) pvalue2;

	if ( *value1 == INDEFI || *value2 == INDEFI ) return 1;

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
