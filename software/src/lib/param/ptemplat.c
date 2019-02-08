/* ptemplat.c
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

#include "list.h"
#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"


pmatchlist pmatchopen(pfile, template)
			ParamFile	 pfile;
			char		*template;
{
		ParamMatchList	*mlist;
		ParamList	*plist;
		void		*list = NULL;
		int		leng = 0;
		int		i;

	if ( pfile == NULL ) return NULL;
	if ( pfile->psets == NULL ) return NULL;

	if ( template && !strcmp(template, "*") ) template = NULL;
	plist = pfile->psets;

	for ( i = plist->nparam; i--; )
		if ( plist->param[i]->ptype != CommentType
		    && (  (template == NULL)
			|| tmatch(plist->param[i]->pname, template) ) ) {
				ParamMatchEntry *e;

			e = (ParamMatchEntry *)malloc(sizeof(ParamMatchEntry));
			e->next = NULL;
			e->name = NewString(plist->param[i]->pname);
			list = (ParamMatchEntry *) ListPush(list, e);
			leng++;
		}

	mlist = (ParamMatchList *)malloc(sizeof(ParamMatchList));

	mlist->list = list;
	mlist->next = list;
	mlist->leng = leng;

	return mlist;
}
	

char *pmatchnext(mlist)
			ParamMatchList	*mlist;
{
		char *name;

	if ( mlist == NULL ) return NULL;

	name 		= mlist->next ? mlist->next->name : NULL;
	mlist->next	= mlist->next ? mlist->next->next : NULL;

	return name;
}


void pmatchrewind(mlist)
			ParamMatchList	*mlist;
{
	mlist->next = mlist->list;
}

 
int pmatchlength(mlist)
			ParamMatchList *mlist;
{
	return mlist->leng;
}


void pmatchclose(mlist)
			ParamMatchList *mlist;
{
		ParamMatchEntry	*e;
		ParamMatchEntry	*next;

	if ( mlist == NULL ) return;

	if ( mlist->list == NULL ) {
		free(mlist);
		mlist = NULL;
		return;
	}
	next = mlist->list;
	free(mlist);
        mlist = NULL;
		
	while ( e = next ) {
		next = (ParamMatchEntry *) ListPop(e);
		free(e->name);
		free(e);
		e = NULL;
	};
}
