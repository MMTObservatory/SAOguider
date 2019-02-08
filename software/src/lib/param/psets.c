/* psets.c
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

#include "pfile.h"

int   SSSize(), SSComp();
void *StringCopy();
static Value *VAcce();

static Converter Converters[] = {
	{StringType,	PSetType,  StringCopy},
	{0,		0,	   	NULL}
};


vtype	psetRec = {
	  "pset"
	, Converters

	, SSSize
	, VAcce
	, SSComp
};


static Value *VAcce(pfile, p, type, value)
			ParamFile	 pfile;
			Parameter	*p;
			VType	 	 type;
			Value		*value;
{
		void	*converted;

	if ( type == 0  )				return NULL;
	if ( value == NULL )				return NULL;

	VDirect(pfile, p, value);

	if ( value->value == NULL || *((char *) value->value) == '\0' ) {
		if (  value == &p->pmin || value == &p->pmax ) return NULL;
		else
			converted = p->pname;
	} else {
		if ( value->type == type )
			return value;
		else
		       if ( (converted = VConvert(value, NULL, type)) == NULL )
				return NULL;
	}

	return VNewValue(value, converted, type);
}

