/* paramset.c
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


#ifdef vax
#define HISTORY 0
#endif

#ifdef NeXT
#define HISTORY 0
#endif

#ifndef HISTORY
#define HISTORY 1
#endif

#include <stdlib.h>
#include <string.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "range.h"
#include "history.h"

char *QuoteValue();
static char *UnquoteValue();
static void  PQueryUser();

int ParamSetX(pfile, name, type, value)
			ParamFile 	 pfile;
			char		*name;
			VType		 type;
			void		*value;
{
		Parameter	*p;
		int		 field;
		char		 xname[SZ_PFLINE];
		int		 mode=pfile->mode;
		char		*freeval = NULL;
		int		 ok = 0;

	if ( value && (TypeIndex(type) == StringType) ) {
	  value = QuoteValue(value, NULL);
	  freeval = value;
	}

	field = PParseName(name, xname);

	if ( p = ParamLookup(pfile->psets, xname) )
	    switch ( field ) {
	     case PMODE:
		if ( (value = Convert(value, type, NULL, ModeType)) ) {
			p->pmode = *(int *) value;
			ok = 1;
		}
		break;
	     case PMIN: {
			Value  pvalue;
			Value *result;

		pvalue.value = NULL;
		pvalue.indir = NULL;
		VNewValue(&pvalue, value, type);

		while ( ((result = VIsIndirect(&pvalue)) == NULL)
		     && ((result = VRange (pfile, p, &pvalue))==NULL)
		     && ((result = VAccess(pfile, p, p->ptype, &pvalue))==NULL)
		     && !(mode&HHMODE) )
			PQueryUser(pfile, p, &pvalue);

		VNewValue(&p->pmin, pvalue.value, pvalue.type);

		if ( pvalue.value ) free(pvalue.value);
		if ( result ) ok = 1;
		break;
	     }
	     case PMAX: {
			Value  pvalue;
			Value *result;

		pvalue.value = NULL;
		pvalue.indir = NULL;
		VNewValue(&pvalue, value, type);

		while ( ((result = VIsIndirect(&pvalue))==NULL)
		     && ((result = VAccess(pfile, p, p->ptype, &pvalue))==NULL)
       		     && !(mode&HHMODE) )
			PQueryUser(pfile, p, &pvalue);

		VNewValue(&p->pmax, pvalue.value, pvalue.type);

		if ( pvalue.value ) free(pvalue.value);
		if ( result ) ok = 1;
		break;
	     }
	     case PVALUE:
	     case PWORTH: {
			Value  pvalue;
			Value *result;

		pvalue.value = NULL;
		pvalue.indir = NULL;
		VNewValue(&pvalue, value, type);

		if ( p->phook != NULL ) InitList(p);

		while (	((result = VIsIndirect(&pvalue))==NULL)
		     && ((result  = Validate(pfile, p, &pvalue))==NULL)
       		     && !(mode&HHMODE) )
			PQueryUser(pfile, p, &pvalue);

		VNewValue(&p->pvalue, pvalue.value, pvalue.type);

		if ( pvalue.value ) free(pvalue.value);
		if ( result ) ok = 1;
		break;
	     }
	     case PPROMPT:
		if ( (value = Convert(value, type, NULL, StringType)) ) {
				int length = strlen(value);

			if ( p->pprompt ) free(p->pprompt);
			p->pprompt = (char *)malloc(length+1);
			bcopy(value, p->pprompt, length);
			p->pprompt[length] = '\0';
			ok = 1;
		}
		break;
	     default:
		break;
	    }
	else {
	    parerr = PARNOTFOUND;
	    value = NULL;
	}


	if ( freeval != NULL ) free(freeval);
	return ok;
}


void *ParamGetX(pfile, name, type, xmode)
			ParamFile 	 pfile;
			char	*name;
			VType	 type;
			int	*xmode;
{
		Parameter	*p;
		Parameter	*a;
		int		 field;
		void		*value = NULL;
		char		xname[SZ_PFLINE];

		int		zero = 0;

    if ( *name == '$' ) {
	if ( !strcmp(name, "$nargs") ) {
	    if ( pfile->alist != NULL ) {
	    	value = Convert(&pfile->alist->nparam, IntegerType, NULL, type);
	    } else {
	    	value = Convert(&zero, IntegerType, NULL, type);
	    }
	} else {
	    return NULL;
	}
    } else {

	field = PParseName(name, xname);

	if ( p = ParamLookup(pfile->psets, xname) ) {
			int mode;

	    if ( p->pmode & AMODE ) {
			mode = *xmode;
			if ( mode & HMODE ) mode |= LMODE;
	    } else {
			mode = p->pmode;
			if ( mode & HMODE ) mode &= ~LMODE;
	    }

	    if (   *xmode & (HMODE|HHMODE) || mode & (HMODE|HHMODE) )
	      mode &= ~QMODE;
	    if ( *xmode & HHMODE )
	      mode |= HHMODE;
	
	    a =  ParamLookup(pfile->alist, xname);  parerr = 0;

	    switch ( field ) {
/*
	         case PTYPE:
	                value = (char *)malloc(SZ_PFLINE);
	                Type2String(p->ptype, value);
	                break;
*/
		 case PMODE:
			value = Convert(&p->pmode, ModeType, NULL, type);
			break;

		 case PMIN: {
				Value *result;

			while ( ((result = VRange(pfile, p, &p->pmin)) ==NULL)
			   &&   ((result = VAccess(pfile, p, p->ptype, &p->pmin))==NULL)
             		   &&   !(mode&HHMODE) )
				PQueryUser(pfile, p, &p->pmin);

			value = VConvert(&p->pmin, NULL, type);
			break;
		 }
		 case PMAX: {
				Value *result;

			while ( ((result = VAccess(pfile, p, p->ptype, &p->pmax))==NULL)
       		                && !(mode&HHMODE) )
				PQueryUser(pfile, p, &p->pmax);

			value = VConvert(&p->pmax, NULL, type);
			break;
		 }
		 case PWORTH: {
				Value *fld = a ? &a->pvalue : &p->pvalue;
				Value *result;

			if ( IsInList(p) ) {
				result = NextListValue(pfile, p);
			} else {
			    if ( (a==NULL) && mode & QMODE )
				PQueryUser(pfile, p, fld);
			
			    while ( ((result = Validate(pfile, p, fld))==NULL)
				 && !(mode&HHMODE) )
				PQueryUser(pfile, p, fld);

			    if ( a && mode & LMODE ) {
			        VNewValue(&p->pvalue, result->value, result->type);
			    	p->pvalue.indir = result->indir;
			    }

			    if ( IsInList(p) ) result = NextListValue(pfile, p);
			}

			value = VConvert(result, NULL, type);
			break;
		 }
		 case PVALUE:
			value = VConvert(&p->pvalue, NULL, type);
			break;
		 case PPROMPT: {
			value = Convert(p->pprompt, StringType, NULL, type);
			break;
		 }
		 default:	value = NULL;
		}
	} else {
	    parerr = PARNOTFOUND;
	    value = NULL;
	}
    }

    if ( value && TypeIndex(type) == StringType ) UnquoteValue(value, value);
    return value;
}


static void PQueryUser(pfile, p, value)
			ParamFile	 pfile;
			Parameter	*p;
			Value		*value;
{
		Value	 pvalue;
		char	 dv[SZ_PFLINE], dn[SZ_PFLINE], dx[SZ_PFLINE];
		char	 *dvp, *dnp, *dxp;
		char	line[SZ_PFLINE];
		char	prompt[SZ_PFLINE];
		int	count;

#if HISTORY
		void	*history = NULL, *add_history();
#endif
	if ( p == NULL ) return;

	dv[0] = '\0';

	dvp = VConvert(&p->pvalue, dv, StringType);
#if HISTORY
	if ( dvp ) history = add_history(history, dvp);
#endif
	/* If the PVALUE of the value is an indirection lookup the
	** PWORTH of it. 
	**/
	pvalue.value = NULL;
	pvalue.indir = NULL;
	VNewValue(&pvalue, p->pvalue.value, p->pvalue.type);
	if ( dvp && VIsIndirect(&pvalue) ) {
	    strcat(dv, " -> ");
	    if ( VAccess(pfile, p, StringType, &pvalue) ) {
		strcat(dv, pvalue.value);
#if HISTORY
		history = add_history(history, pvalue.value);
#endif
	    } else
		strcat(dv, "INDEF");
	}
	if ( pvalue.value ) free(pvalue.value);

	if ( value->indir ) free(value->indir);

#if HISTORY
	if ( value->value && TypeIndex(value->type) == StringType )
		history = add_history(history, value->value);
#endif

	dnp = VConvert(&p->pmin  , dn, StringType);
	dxp = VConvert(&p->pmax  , dx, StringType);

	if ( parerr ) paramerr(0, "pquery", p->pname);

	strcpy(prompt, p->pprompt ? p->pprompt : p->pname);
	strcat(prompt, dnp  || dxp ? " ("    : "");
	strcat(prompt, dnp         ? dn     : "");
	strcat(prompt, dnp  && dxp ? ":"    : "");
	strcat(prompt,         dxp ? dx     : "");
	strcat(prompt, dnp  || dxp ? ")"    : "");
	strcat(prompt, dvp && *dvp != ')' ? " ("    : "");
	strcat(prompt, dvp                ? dv      : "");
	strcat(prompt, dvp                ? ")"    : "");
	strcat(prompt, ": ");

#if HISTORY 
	if ( (count = readline(prompt, line, SZ_PFLINE, history)) >0 
#else
#ifdef vms
	if ( (count = readline(prompt, line, SZ_PFLINE)) >0 
#else
	fputs(prompt, stderr);
	if ( (count = fgets(line, SZ_PFLINE, stdin)) >0 
#endif
#endif
		&& *((char *) line) != '\n' ) {
		line[count] = '\0';

		QuoteValue(line, line);

		VNewValue(value, line, StringType);
	} else
		VNewValue(value, p->pvalue.value, p->pvalue.type);

#if HISTORY
	kil_history(history);
#endif
}


PParseName(fname, xname)
			char	*fname;
			char	*xname;
{
		char	f[SZ_PFLINE];
		char	*subfield;
		int	field;

	strcpy(f, fname);
	subfield = strchr(f, '.');

	if ( subfield ) {
		*subfield++ = '\0';

		/* switch */
		     if ( !strcmp(subfield, "p_mode")   )	field = PMODE;
/*	else if ( !strcmp(subfield, "p_type")  )	field = PTYPE; */
		else if ( !strcmp(subfield, "p_value")  )	field = PVALUE;
		else if ( !strcmp(subfield, "p_min")    )	field = PMIN;
		else if ( !strcmp(subfield, "p_max")    )	field = PMAX;
		else if ( !strcmp(subfield, "p_prompt") )	field = PPROMPT;
		else	{
				parerr = PARSUBFIELD;
				field = 0;
		}
	} else
		field = PWORTH;

	strcpy(xname, f);
	return field;
}


char *QuoteValue(in, out)
			char	*in;
			char	*out;
{
		char	*tbuf, *t;
		int	i;
		int	len = strlen(in);

    t = tbuf = (char *)malloc((len * 2) + 1);

    if ( !strcmp(in, "\"\"") ) {
	tbuf[0] = '\0';
    } else{
	for ( i = 0; i < len; i++ ) {
	    if ( in[i] == '\n' ) {  *t++ = '\\'; *t++ = 'n';  continue; };
	    if ( in[i] == '\t' ) {  *t++ = '\\'; *t++ = 't';  continue; };

	    *t++ = in[i];
	}
	*t = '\0';
   }

    if ( out == NULL ) out = tbuf;
    else {
	strcpy(out, tbuf);
	free(tbuf);
    }

    return out;
}


static char *UnquoteValue(in, out)
			char	*in;
			char	*out;
{
		char	*tbuf, *t;
		int	i;
		int	len = strlen(in);

    t = tbuf = (char *)malloc(len + 1);

    for ( i = 0; i < len; i++ ) {
	if ( in[i] == '\\' ) {
	    if (i + 1 < len)
	    {
	      if ( in[i+1] == 'n' )  { *t++ = '\n';   i++; continue; }
	      if ( in[i+1] == 't' )  { *t++ = '\t';   i++; continue; }
	    }
	    *t++ = '\\';
	} else 		      *t++ = in[i];
    }
    *t = '\0';

    if ( out == NULL ) out = tbuf;
    else {
	strcpy(out, tbuf);
	free(tbuf);
    }

    return out;
}
