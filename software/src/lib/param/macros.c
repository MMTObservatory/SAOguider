/*
 *
 * macros.c -- macro expansion routines
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* SAOMOD_CTYPE -- work around Slackware/RedHat incompatibility */
#ifdef linux
#ifdef isalnum
#undef isalnum
#define isalnum(c) (isalpha(c)||isdigit(c))
#endif
#endif

#define NewString(str) \
    ((str) != (char *)NULL ? ((char *)strcpy((char *)malloc((unsigned)strlen(str) + 1), str)) : (char *)NULL)

#define BUFINC 5000

/*
 *
 * Private Routines
 *
 *
 */

/*
 *
 * AddString -- add a string to a buffer
 *
 */
static void AddString(buf, blen, maxlen, str)
     char **buf;
     int *blen;
     int *maxlen;
     char *str;
{
  int slen;

  slen = strlen(str) + 1;
  while( (*blen + slen) >= *maxlen ){
    *maxlen += BUFINC;
    *buf = (char *)realloc(*buf, *maxlen);
  }
  strcat(*buf, str);
  *blen += slen;
}

/*
 *
 * AddChar -- add a single char to a buffer
 *
 */
static void AddChar(buf, blen, maxlen, c)
     char **buf;
     int *blen;
     int *maxlen;
     char c;
{
  int slen;
  char tbuf[2];

  tbuf[0] = c;
  tbuf[1] = '\0';
  AddString(buf, blen, maxlen, tbuf);
}

/*
 *
 * LookupKeywords -- lookup a name in a list fo keywords and
 * return the associated value.
 * (Should use quarks ...)
 *
 */
static char *LookupKeywords(name, keyword, value, nkey)
     char *name;
     char **keyword;
     char **value;
     int nkey;
{
  int i;
  for(i=0; i<nkey; i++){
    if( !strcmp(name, keyword[i]) )
      return(value[i]);
  }
  return(NULL);
}

/*
 *
 * Public Routines
 *
 *
 */

/*
 *
 * ExpandMacro -- expand a macro using a client's callback
 * returns: expanded macro as an allocated string
 *
 */
char *ExpandMacro(icmd, keyword, value, nkey, client_callback, client_data)
     char *icmd;
     char **keyword;
     char **value;
     int nkey;
     char *(*client_callback)();
     void *client_data;
{
  int  i, j;
  int  maxlen;
  char brace;
  char *result;
  char tbuf[1000];
  char tbuf1[1000];
  char *s;
  char *ip;
  char *mip;
  char *getenv();

  /* make a new string using the command as a base, but substituting
     for "$" values as needed */
  result = (char *)malloc(BUFINC+1);
  maxlen = BUFINC;
  *result = '\0';
  for(i=0, ip=icmd; *ip; ip++){
    if( *ip != '$' ){
      AddChar(&result, &i, &maxlen, *ip);
    }
    else{
      /* save beginning of macro */
      mip = ip;
      /* skip past '$' */
      ip++;
      /* check for brace mode */
      if( *ip == '{' ){
	brace = '{';
	ip++;
      }
      else if( *ip == '(' ){
	brace = '(';
	ip++;
      }
      else
	brace = '\0';
      /* get variable up to next non-alpha character or close brace */
      for(*tbuf='\0', j=0; *ip; ip++ ){
	/* if we are in brace mode, look for trailing brace */
	if( brace && *ip == (brace == '(' ? ')' : '}') ){
	  ip++;
	  break;
	}
	/* else look for a non-alpha character */
	else if( !isalnum(*ip) && *ip != '_'){
	  break;
	}
	else{
	  tbuf[j++] = *ip;
	  tbuf[j] = '\0';
	}
      }
      /* back up so the outer loop adds this delimiting char to the output */
      ip--;
      /* search for keyword from the list */
      if( (nkey > 0) && 
	  (s=LookupKeywords(tbuf, keyword, value, nkey)) != NULL ){
	   AddString(&result, &i, &maxlen, s);
      }
      /* execute the client routine to expand macros */
      else if( (client_callback != NULL) &&
	  ((s=(*client_callback)(tbuf, client_data)) != NULL) ){
	    AddString(&result, &i, &maxlen, s);
      }
      /* look for an environment variable */
      else if( (s = getenv(tbuf)) != NULL ){
	AddString(&result, &i, &maxlen, s);
      }
      /* if we don't recognize this macro, put it back onto the string */
      else{
	int len;
	len = ip - mip + 1;
	strncpy(tbuf1, mip, len);
	tbuf1[len] = '\0';
	AddString(&result, &i, &maxlen, tbuf1);
      }
    }
  }
  /* null terminate and save the string */
  result[i] = '\0';
  result = (char *)realloc(result, i+1);
  return(result);
}

