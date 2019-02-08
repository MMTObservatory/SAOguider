/*
 *
 * Find.c -- find files via the path environment variable
 * (and related routines)
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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "find.h"

/*
 *
 * 	private routines 
 *
 */

static char AccessName[MAXBUFSIZE + 2];

static int amparse(mode)
     char *mode;
{
  int xmode = 0;

  xmode |= ( strpbrk(mode, "r") != NULL ? R_OK 	: 0 );
  xmode |= ( strpbrk(mode, "w") != NULL ? W_OK 	: 0 );
  xmode |= ( strpbrk(mode, "x") != NULL ? X_OK	: 0 );
  xmode |= ( strpbrk(mode, "f") != NULL ? F_OK 	: 0 );

  return xmode;
}	

static char *findpath(name, mode, path)
     char *name;
     char *mode;
     char *path;
{
  char	pathbuff[4096];
  char	namebuff[MAXBUFSIZE];
  char	backmode[10];
  char 	*here, *found;
  int 	 mark = 0;
  int	 skip = strpbrk(mode, ">") != NULL;
  int	 pick = strpbrk(mode, "<") != NULL;

  if ( skip && pick ) return NULL;

  if ( (path==NULL) || ( name[0] == '.' && name[1] == '/' ) || name[0] == '/' )
    return Access(name, mode);

  strcpy(pathbuff, path);
  path = pathbuff;

  if ( here = strpbrk(pathbuff, " :;") ) {
    mark = *here;
    *here++ = '\0';
  }
  while ( path ) {
    if ( path[0] == '$' ) {
      if ( path = getenv(&path[1]) )
	if ( found = findpath(name, mode, path) )
	  return found;
    } else {      
      if ( !skip ) {      
	if ( !strcmp(".", path) ) path[0] = '\0';

	strcpy(namebuff, path);
	if ( path[0] && path[strlen(path) - 1] != '/' )
	  strcat(namebuff, "/");
	strcat(namebuff, name);

	if ( found = Access(namebuff, mode) )
	  return found;
      }
    }

    if ( mark == ';' ) {
      if ( skip ) {
	skip = 0;
	/* Knock down the skip mode to select all
	 * directories in path after the first ";"
	 */
	strcpy(backmode, mode);
	backmode[strlen(backmode) -1] = '\0';
	mode = backmode;
      }
      if ( pick ) return NULL;
    }

    path = here;
    if ( here && (here = strpbrk(here, " :;")) ) {
      mark = *here;
      *here++ = '\0';
    }
  }

  return NULL;
}


/*
 *
 * 	public routines 
 *
 */

/*
 *
 * ResolvePath -- resolve the path to remove . and .. entries
 *
 */
char *ResolvePath(ibuf, obuf)
     char *ibuf;
     char *obuf;
{
  char path[MAXBUFSIZE+2];
  char *part[MAXBUFSIZE];
  char *tbuf;
  int i, j;
  int len;
  int npart=0;

  /* if we have no path separators, we really don't have much to do! */
  if( strchr(ibuf, '/') == NULL ){
    strcpy(obuf, ibuf);
    return(obuf);
  }

  /* if its just "/" or "/.", its easy */
  if( !strcmp(ibuf, "/") || !strcmp(ibuf, "/.") ){
    strcpy(obuf, "/");
    return(obuf);
  }

  /* if we have a relative path to deal with, get current directory */
  if( (*ibuf == '.') || ( (strchr(ibuf, '/') != NULL) && (*ibuf != '/') ) ){
    getcwd(path, MAXBUFSIZE);
  }
  else{
    *path = '\0';
  }

  /* construct the total string we have to deal with */
  len = strlen(path) + strlen(ibuf) + 1;
  tbuf = (char *)malloc(len+1);
  if( *path ){
    strcpy(tbuf, path);
    strcat(tbuf, "/");
    strcat(tbuf, ibuf);
  }
  else{
    strcpy(tbuf, ibuf);
  }
  
  /* construct the parts array from this string, removing / characters
     and null-terminating each part */
  for(i=0; i<len; i++){
    if( tbuf[i] == '/' ){
      tbuf[i] = '\0';
      part[npart] = &tbuf[i+1];
      npart++;
    }
  }

  /* loop through the parts array and resolve the  . and .. entries */
  for(i=0; i<npart; i++){
    /* for ".", just remove it */
    if( !strcmp(part[i], ".") ){
      part[i] = NULL;
    }
    /* for "..", also remove the previous part -- if possible */
    else if( !strcmp(part[i], "..") ){
      part[i] = NULL;
      for(j=i-1; j>=0; j--){
	if( part[j] ){
	  part[j] = NULL;
	  break;
	}
      }
    }
  }

  /* construct a new string from the remaining parts */
  *obuf = '\0';
  for(i=0; i<npart; i++){
    if( part[i] != NULL ){
      strcat(obuf, "/");
      strcat(obuf, part[i]);
    }
  }

  /* free up buffer space */
  free(tbuf);

  /* return the string */
  return(obuf);
}

void ExpandEnv(name, fullname, maxlen)
     char *name;
     char *fullname;
     int maxlen;
{
  char brace[2];
  char tbuf[MAXBUFSIZE];
  char *mip;
  char *ip;
  char *s;
  int len;
  int i, j;

  i=0;
  fullname[i] = '\0';
  for(ip=name; *ip; ip++){
    if( *ip != '$' ){
      fullname[i++] = *ip;
      fullname[i] = '\0';
    }
    else{
      mip = ip;
      /* skip past '$' */
      ip++;
      /* skip past brace, if necessary */
      if( *ip == '{' ){
	brace[0] = '{';
	ip++;
      }
      else if( *ip == '(' ){
	brace[0] = '(';
	ip++;
      }
      else
	brace[0] = '\0';
      /* get variable up to next white space */
      for(*tbuf='\0', j=0;
	  (!isspace(*ip)) && (*ip != '"') && (*ip != '\'') && (*ip);
	  ip++){
	/* look for trailing brace, if necessary */
	if( *brace && *ip == (*brace == '(' ? ')' : '}') ){
	  ip++;
	  break;
	}
	/* a "/" will end the environment variable as well */
	if( *ip == '/' ){
	  break;
	}
	tbuf[j++] = *ip;
	tbuf[j] = '\0';
      }
      /* back up so we can process the white space in the outer loop */
      ip--;
      if( (s = getenv(tbuf)) != NULL ){
	i += strlen(s);
	if( i <= maxlen )
	  strcat(fullname, s);
      }
      /* if we don't recognize this macro, put it back onto the string */
      else{
	len = ip - mip + 1;
	i += len;
	if( i <= maxlen )
	  strncat(fullname, mip, len);
      }
    }
  }
}

char *Access (name, mode)
     char *name;
     char *mode;
{
  struct stat info;
  char fullname[MAXBUFSIZE];

  ExpandEnv(name, fullname, MAXBUFSIZE);
  if ( stat(fullname, &info) !=0 ) return NULL;

  if ( mode ) {
    int m = amparse(mode);

    if ( getuid() == info.st_uid ) {
	if ( m & R_OK && !(info.st_mode & S_IRUSR) ) return NULL;
	if ( m & W_OK && !(info.st_mode & S_IWUSR) ) return NULL;
	if ( m & X_OK && !(info.st_mode & S_IXUSR) ) return NULL;
    } else
      if ( getgid() == info.st_gid ) {
	if ( m & R_OK && !(info.st_mode & S_IRGRP) ) return NULL;
	if ( m & W_OK && !(info.st_mode & S_IWGRP) ) return NULL;
	if ( m & X_OK && !(info.st_mode & S_IXGRP) ) return NULL;
      } else {
	if ( m & R_OK && !(info.st_mode & S_IROTH) ) return NULL;
	if ( m & W_OK && !(info.st_mode & S_IWOTH) ) return NULL;
	if ( m & X_OK && !(info.st_mode & S_IXOTH) ) return NULL;
      }
  }

  ResolvePath(fullname, AccessName);
  return(AccessName);
}

char *Find (name, mode, extn, path)
     char *name;
     char *mode;
     char *extn;
     char *path;
{
  char	extnbuff[MAXBUFSIZE];
  char	namebuff[MAXBUFSIZE];
  char 	*here, *found;

  if ( extn == NULL )
    return findpath(name, mode, path);
  
  strcpy(extnbuff, extn);
  extn = extnbuff;

  if ( here = strpbrk(extnbuff, " :;") ) *here++ = '\0';

  while ( extn ) {
    if ( extn[0] == '$' ) {
      if ( extn = getenv(&extn[1]) )
	if ( found = Find(name, mode, extn, path) )
	  return found;
    } else {
      char *e = strstr(name, extn);
      
      strcpy(namebuff, name);
      if ( (e==NULL) || ( e && *(e + strlen(extn))) )
	strcat(namebuff, extn);
      
      if ( (found = findpath(namebuff, mode, path)) )
	return found;
      
    }
    
    extn = here;
    if ( here && (here = strpbrk(here, " :;")) ) *here++ = '\0';
  }
  
  return NULL;
}
