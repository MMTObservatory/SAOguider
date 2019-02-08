/* type.c
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

vtype	voidRec = {
	  "void"
	, NULL

	, NULL
	, NULL
	, NULL
};
	
vtype	*VTypeList[MaxTypes+3] = {
	  &voidRec
	, &booleanRec
	, &integerRec
	, &realRec
	, &stringRec
	, &fileRec
	, &gRec
	, &structRec
	, &gcurRec
	, &imcurRec
	, &modeRec
	, &psetRec	

	, &rangeRec
	, &commentRec
	, NULL
};


void *Type2String(type, string)
			VType	type;
			char	*string;
{
		int	i;

	if ( type == 0 ) return NULL;
	if ( string == NULL ) return NULL;

	if ( type & ListType ) {
		*string = '*'; 
		string++;
	}

	strcpy(string, VTypeList[TypeIndex(type)]->name);
	return string;
}


VType String2Type(string)
			char	*string;
{
		int	i;
                int     list = 0;

	if ( *string == '*' ) {
		list = 	ListType;
		string++;
	}

	if ( *string == 'f' )
		i = FileType;
	else
		for ( i = 0; VTypeList[i]; i++ ) 
			if ( !strcmp(string, VTypeList[i]->name) ) break;

	if ( VTypeList[i] == NULL ) {
		parerr = PARBADTYPE;
		return 0;
	}

	return i | list;
}
