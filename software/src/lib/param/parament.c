/* parament.c
**
** ParamEntry();
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
#include <stdlib.h>

#include "pfile.h"


int ParamAccess(pfile, name)
			ParamFile	 pfile;
			char		*name;
{
	return ParamLookup(pfile->psets, name) != NULL;
}


int ParamInfo(pfile, name, mode, type, value, min, max, prompt)
			ParamFile	pfile;
			char	*name, *mode, *type, *value, *min, *max, *prompt;
{
	    char	xname[SZ_PFLINE];

	if ( pfile == NULL ) return 0;

	PParseName(name, xname);
	/* fprintf(stderr, "%s %s\n", name, xname); */

	return ParamEntry(pfile, ParamLookup(pfile->psets, xname)
		, NULL, mode, type, value, min, max, prompt);
}

char *ParamResolve(pfile, name, str, max)
			ParamFile       pfile;
			char		*name;
			char		*str;
			int		 max;
{
		int mode=pfile->mode;
		void *value;
		if( value = ParamGetX(pfile, name, StringType, &mode) )
		  strncpy(str, value, max);
		else
		  *str = '\0';
		return str;
}	

int ParamEntry(pf, p, name, mode, type, value, min, max, prompt)
     			ParamFile	*pf;
			Parameter	*p;
			char	*name, *mode, *type, *value, *min, *max, *prompt;
{
	int got=1;

	if ( p == NULL ) return 0;

/* fprintf(stderr, "Here X 1\n"); */
	if ( name )	strcpy(name, p->pname);
	if ( mode )	Convert(&p->pmode, ModeType, mode, StringType);
	if ( type )	Type2String(p->ptype, type);
	if ( value )
/* fprintf(stderr, "Here X X\n"); */
	  	if ( p->pvalue.indir ){
			strcpy(value, p->pvalue.indir);
			got = 2;
		}	
		else{
			    char c[1024];	/* Remember that only john can write code like this */

		  	VConvert(&p->pvalue, value, StringType);
		  	if( pf != NULL ){
		  	  Value	*v;
			  v = (Value *)calloc(1, sizeof(Value));
			  VNewValue(v, p->pvalue.value, p->pvalue.type);
			  if( IndirectClassing(pf, p, v, c) ){
			    VConvert(v, value, StringType);
			    got = 3;
			  }
			  free((char *)v);
			}
		}
	if ( min )
		if ( p->pmin.indir )
			strcpy(min, p->pmin.indir);
		else
			VConvert(&p->pmin, min, StringType);
	if ( max )
		if ( p->pmax.indir )
			strcpy(max, p->pmax.indir);
		else
			VConvert(&p->pmax, max,   StringType);

	if ( prompt )	strcpy(prompt, p->pprompt ? p->pprompt : "");

	return got;
}
