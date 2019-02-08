/* range.c
*/

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
#include <stdlib.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "range.h"

vtype	rangeRec = {
	  "range"
	, NULL

	, NULL
	, NULL
	, NULL
};

int VIsNotRange(value)
			Value	*value;
{
	return  !( value 
		&& value->value 
		&& TypeIndex(value->type) == StringType
		&& strpbrk(value->value, "|:!"));
}


Value *VInRange(range, value)
			Range	*range;
			Value	*value;
{
		int 	 i;
		char	*a = NULL;

	if ( !VTypeList[TypeIndex(range->type)]->compar ) {
		parerr = PARCANTRANGE;
		return NULL;
	}

	if ( range->type != value->type ) {
		fprintf (stderr, "INTERNAL TYPE MISMATCH range != value\n");
		exit(2);
	}

	/* Check is the value is an alowable sub-string of
	** an enumeration
	**/
	if ( TypeIndex(range->type) == StringType ) {

	    for ( i = 0; i < range->nval; i++ ) {
		if ( !strcmp(range->check[i].v.valu, value->value) )
			return value;

		if ( strstr(range->check[i].v.valu, value->value)
			== range->check[i].v.valu )
		    if ( a )  {
			parerr = PARENUMUNIQ;
			return NULL;
		    } else	a = range->check[i].v.valu;
	    }
	    if( a )
	      return VNewValue(value, a, StringType);
	    else{
	      parerr = PARBADENUM;
	      return NULL;
	    }
	}

		
	for ( i = 0; i < range->nval; i++ )
	    if ( VCompar(range->type, value->value, range->check[i].v.valu
					   , range->check[i].oper) )
			 return value;

	parerr = PARLIMITRANGE;
	return NULL;
}


Range *RangeNew(type)
			VType	type;
{
		Range	*evalue;

	evalue = (Range *)malloc(sizeof(Range));

	evalue->type = type;
	evalue->nval = 0;
	evalue->nalo = 0;

	return evalue;
}


Range *RangeAdd(range, valu, oper)
			Range	*range;
			void	*valu;
			int	 oper;
{
	if ( range->nval >= range->nalo ) {
	        if( range != NULL )
		  range = (Range *)realloc(range,
			  sizeof(Range)+(range->nalo+20)*sizeof(Check));
		else
		  range = (Range *)malloc(sizeof(Range)+(range->nalo+20)*sizeof(Check));

		range->nalo += 20;
	}

	bcopy(valu
		, range->check[range->nval].v.valu
		, VSize(range->type, valu));
	range->check[range->nval++].oper = oper;

	return range;
}


Value *VRange(pfile, param, value)
			ParamFile	 pfile;
     			Parameter	 *param;
			Value		 *value;
{
		char	 string[SZ_PFLINE];
		char	*tok;
		void	*xvalue;
		VType	 type;
		Range	*range = NULL;
		

	type = param->ptype;

	if ( value->type == RangeType ) return value;

	VDirect(pfile, param, value);

	if ( VIsNotRange(value) )
		return NULL;

	strcpy(string, value->value);

	range = RangeNew(type);

	for ( tok = strtok(string, "|"); tok; tok = strtok(NULL, "|") ) {
		xvalue = Convert(tok,  StringType
			       , NULL, type);

		if ( xvalue )
			range = RangeAdd(range, xvalue, PRange_EQ);
		else {
			parerr = PARBADRANGE;
			return NULL;
		}
	}

	range->str   = value->value;
	value->type  = RangeType;
	value->value = range;

	return value;
}



void *Range2String(range, string)
			Range	*range;
			char	*string;
{
		char	*str;
		int	 i;

	if ( range == NULL ) return NULL;
	if ( string == NULL ) return NULL;

	*string = '\0';
	for ( i = 0; i < range->nval; i++ ) {
		if ( str = Convert(range->check[i].v.valu, range->type
				 , NULL, StringType) ) {

			if ( *string == '\0' )
				strcat(string, str);
			else {
				strcat(string, "|");
				strcat(string, str);
			}				
		} else	return NULL;
	}

	return string;
}



Value *PRangeMinMax(value, min, max)
			Value *value;
			Value *min;
			Value *max;
{
	if ( value == NULL ) return NULL;

	if ( ( min && value->type != min->type )
	 ||  ( max && value->type != max->type ) ) {
		fprintf(stderr, "INTERNAL TYPE MISMATCH range != value\n");
		exit(2);
	}

	if ( min && !VCompar(value->type, value->value, min->value, PRange_GE) ) {
		parerr = PARLIMITMIN;
		return NULL;
	}

	if ( max && !VCompar(value->type, value->value, max->value, PRange_LE) ) {
		parerr = PARLIMITMAX;
		return NULL;
	}

	return value;
}
