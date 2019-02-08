/* string.c
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
#include <stdio.h>

#include "pfile.h"
#include "range.h"

static void *Int2String ();
static void *Bool2String ();
static void *Real2String ();
void *Range2String();
void *StringCopy ();
int SSSize();
int SSComp();

static Converter Converters[] = {
	{ModeType,	StringType,  Mode2String},
	{IntegerType,	StringType,  Int2String},
	{RealType,	StringType,  Real2String},
	{BooleanType,	StringType,  Bool2String},
	{StringType,	StringType,  StringCopy},
	{PSetType,	StringType,  StringCopy},
	{RangeType,	StringType,  Range2String},
	{StructType,	StringType,  StringCopy},
	{FileType,	StringType,  StringCopy},
	{GType,		StringType,  StringCopy},
	{0,		0,	     NULL}
};

vtype stringRec = {
	  "s"
	, Converters

	, SSSize
	, VXAcce
	, SSComp
};

void	*Mode2String(value, copy)
				int	*value;
				char	*copy;
{
		char *m = copy;

	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	if ( *value & AMODE ) *m++ = 'a';
	if ( *value & HMODE ) *m++ = 'h';
	if ( *value & QMODE ) *m++ = 'q';
	if ( *value & LMODE ) *m++ = 'l';

	*m = '\0';

	return copy;
}


static void	*Int2String(value, copy)
				int	*value;
				char	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	if ( *value == INDEFI )
		strcpy(copy, "INDEF");
	else
		sprintf(copy, "%d", *value);

	return copy;
}



static void	*Real2String(value, copy)
				real	*value;
				char	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	if ( *value == INDEFD )
		strcpy(copy, "INDEF");
	else
		sprintf(copy, "%.16g", *value);
	
	return copy;
}



static void	*Bool2String(value, copy)
				int	*value;
				char	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

	strcpy(copy, *value ? "yes" : "no");

	return copy;
}


void *StringCopy(value, copy)
				char	*value;
				char	*copy;
{
	if ( value == NULL ) return NULL;
	if ( copy == NULL  ) return NULL;

 	strcpy(copy, value);

	return copy;
}

int SSSize(str)
			void	*str;
{
	return strlen((char *) str) + 1;
}


int SSComp(value1, value2, type)
			char	*value1;
			char	*value2;
			int	type;
{
	int	compare;

  while ( *value1 == ' ' ) value1++;
  while ( *value2 == ' ' ) value2++;

  compare = strcmp((char *) value1, (char *) value2);

	if ( value2 && ! *((char *) value2) ) return 1;

	switch ( type ) {
	 case PRange_EQ: return compare == 0;
	 case PRange_NE: return compare != 0;
	 case PRange_GT: return compare >  0;
	 case PRange_GE: return compare == 0 || compare >  0;
	 case PRange_LT: return compare <  0;
	 case PRange_LE: return compare == 0 || compare <  0;
	 default: return 0;
	}
}

