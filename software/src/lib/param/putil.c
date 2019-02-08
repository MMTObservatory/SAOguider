/*
 *
 * putil.c -- refugees from the old Os directory
 *
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXBUFSIZE 1024

#ifndef vms
int Lock(f)
     FILE *f;
{
#ifdef LOCK
  struct flock lock;

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  return fcntl(fileno(f), F_SETLKW, &lock);
#endif
}


int UnLock(f)
     FILE *f;
{
#ifdef LOCK
  struct flock lock;

  lock.l_type = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  return fcntl(fileno(f), F_SETLKW, &lock);
#endif
}
#endif


/*
 * 
 * list.c
 *
 */

/* list.c
**/

#include "list.h"


void *ListInsert(list, member, at)
     List *list;
     List *member;
     List *at;
{
		List	*this;
		List	*prev = NULL;

		List	*mlast;

	for ( this = list; this != at; this = this->next )
		prev = this;

	mlast = member;
	if ( mlast ) 
		while ( mlast->next != NULL ) mlast = mlast->next;

	if ( prev ) {
		prev->next 	= member;
		mlast->next 	= this;
	} else {
		if ( member ) {
			mlast->next 	= list;
			list		= member;
		};
	}

	return (void *) list;
}
	

void *ListDelete (list, member)
     List *list;
     List *member;
{
		List	*this;
		List	*prev = NULL;

	if ( list == NULL )
	  return NULL;

	for ( this = list; this != member; this = this->next )
		prev = this;

	if ( prev )
		prev->next 	= this->next;
	else
		list		= this->next;

	member->next = NULL;
	return (void *) list;
}

/*
 *
 * pargs.c
 *
 */

#include "pargs.h"

ArgStream OpenArgv(nth, separator, argv, argc)
    int             nth;
    char            separator;
    char           *argv[];
    int             argc;
{
    ArgStream       a;

    a = (ArgStream)malloc(sizeof(struct _ArgStream));

    a->ungotten = 0;

    a->here = argv[nth];
    a->this = nth;
    a->argv = argv;
    a->argc = argc;
    a->sep = separator;

    return a;
}


void CloseArgv(a)
    ArgStream       a;
{
    free(a);
    a = NULL;
}

char *NewString(str)
     char *str;
{
  char *s;
  if( str == NULL )
    return(NULL);
  s = (char *)malloc((unsigned)strlen(str)+1);
  strcpy(s, str);
  return(s);
}

PSkipWhite(file)
			FILE	*file;
{
		char ch = getc(stdin);

	while ( ch == ' ' 
	   ||   ch == '\n'
	   ||   ch == '\t'
	   ||   ch == ':'
	   ||   ch == '\0'
	   ||   ch == EOF
	) {
		if ( ch == '\0' ) return -1;
		if ( ch == EOF   ) return -1;
		ch = getc(stdin);
	}

	ungetc(ch, stdin);

	return 1;
}


PReadTok(file, token, max)
			FILE	*file;
			char	*token;
			int	max;
{
		int	i;

	for ( i = 0; i < max; i++ ) {

		token[i] = getc(file);

		if ( !i && token[i] == '\0' ) return -1;

		if (  token[i] == ' ' 
		   || token[i] == '\n'
		   || token[i] == '\t'
		   || token[i] == ':'
		   || token[i] == '\001'
		   || token[i] == '\0'  ) break;
	}

	token[i] = '\0';

	return 1;
}

/*
 *
 * Word -- cheap spaced parser
 *
 */
int Word(lbuf, tbuf, lptr)
     char *lbuf;
     char *tbuf;
     int *lptr;
{
  int ip;
  int i;
  char quotes;

  /* if no string was specified, just return */
  if( lbuf == NULL )
    return(0);

  /* just a more convenient pointer ... */
  ip = *lptr;

  /* if we are at the end of string, just return */
  if( lbuf[ip] == '\0' )
    return(0);

  /* skip over white space */
  while( isspace(lbuf[ip]) ){
    if( lbuf[ip] == '\0' )
      return(0);
    else
      ip++;
  }

  /* check for an explicit quote */
  quotes = '\0';
  if( lbuf[ip] == '"' ){
    quotes = '"';
  }
  if( lbuf[ip] == '\'' ){
    quotes = '\'';
  }

  /* grab next token */
  if( quotes  != '\0' ){
    /* bump past quotes */
    ip++;
    /* grab up to next quotes -- but skip escaped quotes */
    for(i=0; lbuf[ip] != '\0'; i++, ip++){
      if( (lbuf[ip] == quotes) && (lbuf[ip-1] != '\\') )
	break;
      else
	tbuf[i] = lbuf[ip];
    }
  }
  else{
    /* grab up to next whitespace */
    for(i=0; lbuf[ip] && !isspace(lbuf[ip]); i++, ip++)
      tbuf[i] = lbuf[ip];
  }
  /* bump past delimiter (but not null terminator) */
  if( lbuf[ip] )
    ip++;

  /* null terminate */
  tbuf[i] = '\0';
  
  /* return number of chars in parsed string (might be 0) */
  *lptr = ip;
  return(1);
}

