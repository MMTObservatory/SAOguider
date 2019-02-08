/* 
  module:	tmatch.c
  author:	egm
  date:		10/30/86
  purpose:	match templates ala the c shell

  routines:
	tmatch, checkrange
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

#include <stdio.h>
#include <string.h>

/* #define DEBUG */

#define ALL '*'
#define ANY '?'
#define RANGE '['
#define ENDRANGE ']'
#define RANGEDELIM '-'
#define NOTRANGE '~'
#define EOS '\0'

#define TRUE 1
#define FALSE 0
#define ERROR 1

/*
	tmatch - match string to a template
	return non-zero if match, zero otherwise
	the legal meta characters in a template are just like the
	C-shell meta characters, i.e:
	?   		match any character, but there must be one
	*		match anything, or nothing
	[<c>...]	match an inclusive set
*/

int tmatch(string, template)
     char *string;
     char *template;
{
	char *lastmeta=0;
	char *nonabsorbed=0;
	int sptr=0;
	int tptr=0;

	/* loop through string and template */
	while( (template[tptr] != EOS) || (string[sptr] != EOS) ){
#ifdef DEBUG
		printf("s,t: %s,%s\n", &string[sptr], &template[tptr]);
#endif
		/* if exact match, just bump both pointers */
		if( string[sptr] == template[tptr] ){
			sptr++; tptr++; continue;
		}
		/* if range character, check ranges */
		if( template[tptr] == RANGE ){
			if( checkrange(template, &tptr, string[sptr]) == FALSE ){
			        /* no match - was there a meta character before */
				if( lastmeta == 0 ) return(FALSE);
				/* if so, back up to it and try again */
				template = lastmeta; tptr=0;
				/* begin checking at the non-absorbed point */
				string = nonabsorbed; sptr=0;
				continue;
			}
			/* got a match, so bump past */
			else{
				sptr++; continue;
			}
		}
		/* if ANY, any character if fine, but there must be one */
		if( template[tptr] == ANY ){
			if( string[sptr] == EOS )
				return(FALSE);
			else{
				sptr++; tptr++; continue;
			}
		}			
		/* if ALL, we can match anything */
		if( template[tptr] == ALL ){
			/*  remember where the * is */
			lastmeta = &template[tptr];
			tptr++;
			/* no more template after this means a win */
			if( template[tptr] == EOS ) return(TRUE);
			/* if the next template char is not a meta,
	       			we skip up to its match in the string */
			if( template[tptr] == RANGE){
				while( checkrange(template, &tptr, string[sptr]) == FALSE ){
					/* missing the next template char */
					if( string[sptr] == EOS ) return(FALSE);
					sptr++;
				}
				/* remember the first non-absorbed character */
				nonabsorbed = &string[sptr];nonabsorbed++;
				sptr++;
				continue;
			}
			/* skip past characters, if next template char is not a meta */
			else if( template[tptr] != ANY && template[tptr] != ALL ){
				while(string[sptr] != template[tptr]){
					/* not finding the next template char
						is bad */
					if( string[sptr] == EOS ) return(FALSE);
					sptr++;
					}
				/* remember the first non-absorbed character */
				nonabsorbed = &string[sptr];nonabsorbed++;
				continue;
			}
			else{
				/* remember the first non-absorbed character */
				nonabsorbed = &string[sptr];nonabsorbed++;
				continue;
			}
		}
		/* no match, no meta char - see if we once had a meta */
		else{
			if( lastmeta == 0 ) return(FALSE);
			/* if so, back up to it and try again */
			template = lastmeta; tptr=0;
			/* begin checking at the non-absorbed point */
			string = nonabsorbed; sptr=0;
			continue;
		}
	}
	/* matched to the nulls - we win */
	return(TRUE);
}

/*
	checkrange - see if character is in specified range
*/
int checkrange(template, ptr, c)
     char *template;
     int *ptr;
     int c;
{
	int inrange, notrange;
	char lorange, hirange;
	int tptr;

	tptr = *ptr;	
	/* make sure we have a close bracket */
	if( strchr(&template[tptr], ENDRANGE) == (char *)0 )
		return(FALSE);
	/* check for negation - match if not in range */
	if( template [tptr+1] == NOTRANGE ){
		notrange = 1; tptr++;
	}
	else
		notrange = 0;
	/* start pessimistically */
	inrange = 0;
	/* point past RANGE character */
	tptr++;
	while( template[tptr] != ENDRANGE ){
		/* get lo range */
 		lorange = template[tptr];
		/* and hi range */
		tptr++;
		if( template[tptr] != RANGEDELIM )
			hirange = lorange;
		else{
			tptr++;hirange = template[tptr];tptr++;
		}
		if( (c>=lorange) && (c<=hirange) ){
			inrange = 1; break;
		}
	}
	/* only exclusive OR of inrange and notrange is ok */
	if( (inrange ^ notrange) ==0 )
		return(FALSE);
	else{
	        *ptr = strchr(&template[tptr],']') - template + 1;
		return(TRUE);
	}
}
