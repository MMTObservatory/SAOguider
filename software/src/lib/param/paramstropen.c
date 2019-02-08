/* paramstropen.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "putil.h"

paramfile c_paramstropen(str, strname, mode, defaults)
	char	*str;
	char	*strname;
	char	*mode;
	char	*defaults;
{
	int	 length = strlen(str);

	ParamFile  pf;
	ParamList *prefr = NULL;
	ParamList *plist = NULL;


	int	i;
	char	*here, *h;

	char	quote = 0;
	int	delim = 1;
	int	paren = 0;
	int	j;

    str = strdup(str);

    /* Eric adds too many parens and skips the extra new-line */
    if( (mode==NULL) || (*mode == '\0') )
      mode = ",\n";
    
    if ( defaults ) prefr = IRAFStrOpen(strname, defaults);

    j = 0;
    for ( i = 0; i < length; i++ ) {
      if ( quote ) {
	if ( str[i] == quote ) {
	    quote = 0;
	    continue;
	} else
	    str[j++] = str[i];
	    continue;
      }

      if ( paren ) {
	str[j++] = str[i];
	if ( str[i] == ')' )
	  paren--;
	continue;
      }

      if ( str[i] == ' ' 
	|| str[i] == '\t' ) {
	continue;
      }

      if( strchr(mode, str[i]) != NULL ) {
	str[j++] = '\001';
	continue;
      }

      if ( str[i] == '\'' || str[i] == '"' ) {
	quote = str[i];
	continue;
      }

      if ( str[i] == '(' ){
	paren++;
	str[j++] = str[i];
	continue;
      }

      str[j++] = str[i];
    }
    /* nuke the last demim */
    if( j && (str[j-1] == '\001') )
      j--;
    str[j] = '\0';

    plist = ParamFileArgs(prefr, &str, 1);

    free(str);

    if( plist == NULL )
      return((paramfile)NULL);

  /* Never write out a string to disk
   */
  plist->mode = NewString("r");
  plist->file = NULL;

  /* Got a plist now make it a ParamFile
   */
  pf = (struct _ParamFile *)malloc(sizeof(struct _ParamFile));
  if ( prefr ) {
      prefr->mode = NewString("r");
      prefr->file = NULL;

      pf->alist = plist;
      pf->psets = prefr;
  } else {
      pf->alist = NULL;
      pf->psets = plist;
  }

  /* make sure everyone knows we have no filename */
  pf->psets->filename = NULL;
  pf->psets->psetname = NULL;

  /* Promote the mode parameter to the plist structure
   */
  pf->mode = ParamFileMode(pf);

  /* Promote the mode parameter to the plist structure
   */
  pf->mode = ParamFileMode(pf) | (strchr(mode,'H') ? HHMODE : 0);

  return (paramfile) pf;
}
