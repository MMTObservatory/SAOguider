/* getenum.c
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

pgetenum(p, name, dict)
	ParamFile	 p;
	char		*name;
	char		*dict;
{
	char	value[FIELD_SZ];
	int	i;

    pgetstr(p, name, value, FIELD_SZ);
    return strdict(value, dict);
}

strdict(value, dict)
	char	*value;
	char	*dict;
{
	int	alias = 0;
	char	*tok;
	int	i;

	char	v[FIELD_SZ];
	char	d[FIELD_SZ];

    strcpy(d, dict);

    for ( i = 0; dict[i];  i++ ) d[i] = toupper(dict[i]);
    d[i] = '\0';
    for ( i = 0; value[i]; i++ ) v[i] = toupper(value[i]);
    v[i] = '\0';

    for ( i = 1, tok = strtok(d, "|"); tok; i++, tok = strtok(NULL, "|") ) {
	    if ( !strcmp(tok, v) )
		    return i;

	    if ( strstr(tok, v) == tok ) {
		if ( alias ) return 0;
		else	alias = i;
	    }
    }

    return alias;
}
