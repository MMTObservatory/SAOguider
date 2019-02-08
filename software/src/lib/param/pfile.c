/* PFile.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "putil.h"

extern ParamList *IRAFParOpen();
extern ParamList *IRAFStrOpen();
extern ParamList * RDBParOpen();
extern char *locatepfile();

void c_paramwrite();
void c_paramlist();

void ParamFree ();

int		parerr;			/* Global error indicator	*/
paramfile	PFList;			/* Global list pointer		*/
paramfile	PFFile;			/* Global file pointer		*/

ParamFormat ParamFormats[3] = {
	 IRAFParOpen, ".par", paramfind
	, RDBParOpen, ".rdb", paramfind
	  , NULL		};

struct _ParamList _ParamFileCant;
ParamList *ParamFileCant = &_ParamFileCant;

static char pfile_last[129];

int  ParamFileMode();

static char *ParamFName(filepath)
			char	*filepath;
{
		char	*filename;

#ifdef vms
	if ( filename = strrchr(filepath, ']') ) {
			char	*dot;

		filename++;
		if ( dot = strrchr(filename, '.') )
			*dot = '\0';

		return filename;
	}
#else
	if ( filename = strrchr(filepath, '/') )
		return ++filename;
#endif
	else
		return filepath;
}


static char *ParamFPath(filepath)
     char	*filepath;
{
  char	*filename;

#ifdef vms
  if ( filename = strrchr(filepath, ']') ) {
    char	*dot;

    *(++filename) = '\0';

    return filepath;
  }
#else
  if ( filename = strrchr(filepath, '/') ) {
    *(++filename) = '\0';
    return filepath;
#endif
  } else
    return NULL;
}

paramfile c_paramopen(filename, argv, argc, filemode)
     char	*filename;
     char	*argv[];
     int	 argc;
     char	*filemode;
{
  int		 i;
  ParamFile	 pf;
  ParamList	*plist = NULL;
  ParamList	*prefr = NULL;

  char		*filepath;
  char		 frontmode[10];
  char		 pbackmode[10];
  char		 fullname[256];
  char		 mode[10];

  char		*defaults = NULL;
  int		 plister = 0;

  if ( filemode == NULL ) filemode = "rw";

  for ( i = 0; i < 10 && filemode[i] && filemode[i] != '\n'; i++ ) 
	mode[i] = filemode[i];
  mode[i] = '\0';

  if ( filemode[i] == '\n' ) defaults = &filemode[i+1];
  else			     defaults = NULL;

  strcpy(frontmode, mode);	/* Build up the search modes		*/
  strcpy(pbackmode, "r>");
  strcat(frontmode, "<");

  parerr = 0;

  /* if filename is not specified, we take it from the arg list */
  if ( (filename == NULL) || ( filename && *filename == '\0' ) ) {
    if ( argv && argc >= 2 && argv[1][0] == '@' && argv[1][1] == '@' ) {
	filename = ParamFName(&argv[1][2]);
	argv++;
	argc--;
    } 
    else {
      if( (argv != NULL) && (argv[0] != NULL) )
	filename = ParamFName(argv[0]);
    }
  }

  if ( argc == 2 && !strcmp(argv[1], "+") ) {
	plister++;
  }

  if ( filename != NULL && *filename != '\0' ) {
      /* save for errors */
      strncpy(pfile_last, filename, 128);
      pfile_last[128] = '\0';

      /* Look to see if the file is already open.
       */
      if( !strpbrk(mode, "f") ){
	char	*checkpath = mode;		/* Any path will do	*/

	if ( strpbrk(mode, "R") )			/* unless Reference	*/
	  checkpath = pbackmode;
	else
	  if ( (pf = ParamPSetLookup(filename) ) )
	    return pf;


	if ( (pf = ParamFileLookup(c_paramfind(filename, checkpath
					       , PARAMEXTN
					       , PARAMPATH))) )
	  return pf;
      }

      /* Try each parameter file parser in turn.
       */
      for(i = 0;
	  (plist == NULL) && (prefr==NULL) && ParamFormats[i].open != NULL; i++ ){

	/* If the open search mode is Reference or the pfile can't be found
	 * in the front part of the search path.
	 */
	if ( (plist = (*ParamFormats[i].open)(filename
		    , (*ParamFormats[i].find)(  filename, frontmode
					  , ParamFormats[i].extn
					  , PARAMPATH)
		  , mode)) == NULL
	    || strpbrk(mode, "R") ) {

	  /* Try to find the file in the back path
	   */
	  char	*refrname, *refrbase = NULL;

	  refrbase = (char *)malloc(strlen(filename) + 1);	/* Kill any path */
	  strcpy(refrbase, filename);
	  refrname = ParamFName(refrbase);

	  prefr = (*ParamFormats[i].open)(refrname
	    , (*ParamFormats[i].find)(refrname, pbackmode
				     , ParamFormats[i].extn
				     , PARAMPATH)
	     , pbackmode);

	  free(refrbase);
	  refrbase = NULL;
	}
      }
  } 

  if ( (plist == NULL) && (prefr == NULL) ) {	/* Can't find any pfile	*/
    if ( !defaults ) {
	parerr = PARNOFILE; 
	return NULL;
    } else {
	prefr = IRAFStrOpen(filename, defaults);
    }
  }

  /* If reference is needed make sure it's ok
   */
  if ( strpbrk(mode, "R") && ((prefr == NULL) || prefr == ParamFileCant) ) {
    parerr = PARNOREF;
    return NULL;
  }

  if ( plist == ParamFileCant ) 
    /* Something is wrong with the parameter file 
     */
    if ( strpbrk(mode, "R") )
      /* Bogus user copy but the reference is OK.
       */
      plist = NULL;
    else {
      parerr = PARCANTPARSE;
      return NULL;
    }

  if ( prefr ) {	/* If prefr was accessed then we are in R mode
			 * or plist was not found
			 */
    if ( plist ) {
      /* Open existing pfile from reference; 	"R" mode
       */
      fclose(prefr->file);	/* Get rid of the reference file's fp	*/

      prefr->file = plist->file;/* put user file's fp into ref's plist	*/
      prefr->mode = NewString("w");
      plist->file = NULL;

      PFree(plist);		/* Toss out the users plist		*/
      plist = prefr;
    } else {
      /* Open new pfile from reference;		"R" mode or found only in back
       */

      /* The front path search failed but a reference copy was opened
       *
       * If no path was given in the input file name
       * look through front path for a writable directory
       */
      if ( filename == NULL || !strpbrk(mode, "w") ) {
	filepath = NULL;	/* prefr is from a defaults string 
				   w/no pfile */
      } else {
	  char *pathbase = NULL;

	  pathbase = (char *)malloc(strlen(filename) + 1);
	  strcpy(pathbase, filename);

	  /* if no path is given ... */
	  if ( (filepath = ParamFPath(pathbase)) == NULL ) {
	    /* look for a writable directory in the "front" */
	    filepath = Find(".", "wr<", NULL, PARAMPATH);
	    if( (filepath == NULL) || !strcmp(filepath, ".") ){
	      /* if no writable, look for any directory in the "front" */
	      filepath = Find(".", "r<", NULL, PARAMPATH);
	      /* no directories at all => don't write */
	      if( (filepath == NULL) || !strcmp(filepath, ".") ){
		filepath = NULL;
	      }
	      /* if we found directories, but not writable, that's bad */
	      else{
		parerr = PARBACK2FRONT;
		return NULL;
	      }
	    } else{
	      /* Nuke the dot */
	      if( filepath[strlen(filepath) -1] == '.' )
		filepath[strlen(filepath) -1] = '\0';
	    }
	  }

          free(pathbase);
      }

      /* Substitute the reference file's file pointer with a newly opened
       * one in a user writable directory
       */
      plist = prefr;		/* The reference list is the new file	*/
      if ( filepath == NULL || plist->filename == NULL ) {
	/* ensure that we do not write the results out */
	plist->mode = NewString("r");
      } else {
	/* copy the reference file to the writable directory */
	plist->mode = NewString(mode);
	strcpy(fullname, filepath);
	if( fullname[strlen(fullname)-1] != '/' )
	  strcat(fullname, "/");
	strcat(fullname, ParamFName(plist->filename));
	if ( plist->file ) fclose(plist->file);
	/*open the file in the writable directory */
	if ( (plist->file = fopen(fullname, "w")) == NULL ) {
	  parerr = PARBACK2FRONT;
	  return NULL;
	}
	/* Write the new guy out
	 */
	(*plist->writ)(plist->file, plist, "");
      }
    }
  }

  /* Got a plist now make it a ParamFile
   */
  pf = (struct _ParamFile *)malloc(sizeof(struct _ParamFile));
  pf->alist = NULL;
  pf->psets = plist;

  /* Promote the mode parameter to the plist structure
   */
  pf->mode = ParamFileMode(pf) | (strchr(mode,'H') ? HHMODE : 0);

  if ( plister ) {
	c_paramlist(pf);
	exit(0);
  }

  if ( argc > 1 ) {
   if ( argc > 1 ) {
    if ( argv[1][0] == '%' ) {
        pf->alist = ParamArgsBase(pf,        &argv[1], argc -1);
    } else {
        /*pf->alist = ParamFileArgs(pf->psets, &argv[1]   , argc -1); */
        pf->alist = ParamFileArgs(pf, &argv[1]   , argc -1);
    }

    /* Leak Leak Leak */
    if ( pf->alist == NULL ) return NULL;
   }
  } else
    pf->alist = NULL;


  /* Promote the mode parameter to the plist structure
   */
  pf->mode = ParamFileMode(pf) | (strchr(mode,'H') ? HHMODE : 0);

  /* Push the file on the Big list
   */
  pf->next = NULL;
  PFList = ListPush(PFList, pf);


  return pf;
}

void c_paramunlock(pf)
     ParamFile pf;
{
  if ( pf == NULL ) return;

  UnLock(pf->psets->file);
}


void c_paramclose(pf)
     ParamFile	pf;
{
  ParamList	*plist;

  if ( pf == NULL ) return;

  if (  pf->psets && strchr(pf->psets->mode, 'w') )
    c_paramwrite(pf, "l");

  ParamDataBase(pf);

  /* Delete the parameter file from the global list	*/
  PFList = ListDelete(PFList, pf);

  if ( pf->psets != NULL ) PFree(pf->psets);
  if ( pf->alist != NULL ) PFree(pf->alist);
  free(pf);
}




/* Sync with pfile.h */

static char *parammessage[] =  {
   "parameter error?"
 , "parameter not found"
 , "parameter not unique"
 , "parameter file not found"
 , "parameter argument error"
 , "positional parameters must come before absolutes"
 , "parameter has unknown type"
 , "parameter has bad minimum value"
 , "parameter has bad maximum value"
 , "parameter below minimum"
 , "parameter above maximum"
 , "parameter pointer missing"
 , "cannot convert parameter value"
 , "parameter has no value"
 , "command arguments missunderstood"
 , "no parameter file"
 , "no parameter converter"
 , "bad value for parameter"
 , "bad subfield name"
 , "can't specify range and max field"
 , "bad range specification"
 , "parameter type doesn't support range"
 , "value must be in range"
 , "to many positional arguments"
 , "error reading paramfile"
 , "enumerated value not unique"
 , "can't evaluate indirect parameter"
 , "can't open parameter list file"
 , "invalid enumerated value"
 , "can't copy reference parameter file to user directory"
 , "can't find reference parameter file"
 , "can't open parameter set"
 , "positional parameter with no reference list"
};


static int exit_on_paramerror=1;
static void (*errvector)() = NULL;
typedef void (*vector)();

vector paramerract(newact)
	void (*newact)();
{
  void (*oldact)();

  oldact = errvector;
  errvector = newact;
  return oldact;
}

int set_paramerror(new)
     int new;
{
  int old;
  old = exit_on_paramerror;
  exit_on_paramerror = new;
  return(old);
}

void c_paramerr(level, message, name)
     int	 level;
     char	*message;
     char	*name;
{
  if ( errvector == NULL ) {
      fprintf(stderr, "%s: %s : %s\n", message, parammessage[parerr], name);
      fflush(stderr);

      if ( level && exit_on_paramerror ) exit(level);
  } else {
	(*errvector)(level, message, name);
  }

      parerr = PARNOERROR;
}


char *c_paramerrstr()
{
  char *message = parammessage[parerr];

  parerr = 0;
  return message;
}


int ParamFileMode(pf)
     ParamFile pf;
{
  int    hmode = HMODE;
  int   *pmode;
 
  if ( pmode = ParamGetX(pf, "mode", ModeType, &hmode) )
    return *pmode;
 
  pf->psets = PListAdd(pf->psets, ParamCreate(strdup("mode"), StringType, &hmode
       , strdup("ql"), NULL));
 
  parerr = 0;
  return QMODE | LMODE;
}


void c_paramwrite(pf, mode)
     ParamFile	 pf;
     char		*mode;
{
  if ( (pf->psets==NULL) || (strchr(pf->psets->mode, 'w')==NULL) )
    return;
  if ( pf->psets->writ ) (*pf->psets->writ)(pf->psets->file, pf->psets, mode);
}


ParamList *ParamFileArgs(pfile, argv, argc)
     ParamFile	pfile;
     char		*argv[];
     int		 argc;
{
  return IRAFParArgs(pfile, argv, argc);
}


ParamFile ParamFileLookup(fname)
     char	*fname;
{
  ParamFile pf;

  if ( fname == NULL )	return NULL;
  if ( *fname == '\0' )	return PFList;

  for ( pf = PFList; pf; pf = pf->next )
    if ( pf->psets->filename && !strcmp(fname, pf->psets->filename) ) break;


  return pf;
}

ParamFile ParamPSetLookup(psetn)
     char       *psetn;
{
  ParamFile pf;
 
  if ( psetn == NULL )  return NULL;
  if ( *psetn == '\0' ) return PFList;
 
  for ( pf = PFList; pf; pf = pf->next ) {
    if ( pf->psets->psetname && !strcmp(psetn, pf->psets->psetname) ) break;
  }
 
  return pf;
}


char *c_paramfind(name, mode, extn, path)
     char *name, *mode, *extn, *path;
{
  char *retname;

  if ( retname = Find(name, mode, extn, path) )
    return retname;

  retname = locatepfile(name);
  return retname;
}


void c_paramlist(pf)
     ParamFile pf;
{
  Parameter 	**params;
  char		cbuf[SZ_PFLINE];
  int 		len;
  int		tlen;

  char		class[1024];	/* Only I could do it */

  if ( pf == NULL ) return;

  *cbuf = '\0';
  len = 0;
  fprintf(stdout, "\nParameters for %s\n\n", pf->psets->filename);

  for ( params = pf->psets->param; *params; params++ ) {

    Value	 pvalue;
    Value	*xvalue;
    char	 dv[SZ_PFLINE];
    char	 *dvp;

    if ( TypeIndex((*params)->ptype) == CommentType ) {
      tlen = strlen((char *)(*params)->pvalue.value)+1;
      if( len + tlen < SZ_PFLINE ){
	strcat(cbuf, (char *)(*params)->pvalue.value);
	strcat(cbuf, "\n");
	len += tlen + 1;
      }
      continue;
    }

    if ( ((*params)->pmode & HMODE) != 0 ){
      *cbuf = '\0';
      len = 0;
      continue;
    }

    if( *cbuf ){
      fprintf(stdout, "%s", cbuf);
      *cbuf = '\0';
      len = 0;
    }

    dv[0] = '\0';


    pvalue.value = NULL;
    pvalue.indir = NULL;

    if ( IndirectClassing(pf, (*params), &pvalue, class) ) {
	strcpy(dv, class);
	strcat(dv, ":");
        dvp = VConvert(&pvalue, class, StringType);
	strcat(dv, class);
    } else {
        dvp = VConvert(&(*params)->pvalue, dv, StringType);
        VNewValue(&pvalue, (*params)->pvalue.value, (*params)->pvalue.type);
    }

    /* If the PVALUE of the value is an indirection lookup the
     * PWORTH of it. 
     */
    if ( dvp && VIsIndirect(&pvalue) ) {
      strcat(dv, " -> ");
      if ( xvalue = VAccess(pf, *params, StringType, &pvalue) ) {
	char	*foo;
	strcat(dv, foo = QuoteValue(pvalue.value, NULL));
	free(foo);
      } else
	strcat(dv, "INDEF");
    }
    if ( pvalue.value ) free(pvalue.value);

    fprintf(stdout,   "%14s = %-16s %s\n"
	   , (*params)->pname
	   , dvp        ? dv     : ""
	   , (*params)->pprompt ? (*params)->pprompt : "");
  }

  *cbuf = '\0';
  len = 0;
  for ( params = pf->psets->param; *params; params++ ) {

    Value	 pvalue;
    Value	*xvalue;
    char	 dv[SZ_PFLINE];
    char	 name[SZ_PFLINE];
    char	 value[SZ_PFLINE];
    char	 *dvp;

    if ( TypeIndex((*params)->ptype) == CommentType ) {
      tlen = strlen((char *)(*params)->pvalue.value)+1;
      if( len + tlen < SZ_PFLINE ){
	strcat(cbuf, (char *)(*params)->pvalue.value);
	strcat(cbuf, "\n");
	len += tlen + 1;
      }
      continue;
    }

    if ( ((*params)->pmode & HMODE) == 0 ){
      *cbuf = '\0';
      len = 0;
      continue;
    }

    if( *cbuf ){
      fprintf(stdout, "%s", cbuf);
      *cbuf = '\0';
      len = 0;
    }

    dv[0] = '\0';
    dvp = VConvert(&(*params)->pvalue, dv, StringType);


    pvalue.value = NULL;
    pvalue.indir = NULL;

    if ( IndirectClassing(pf, (*params), &pvalue, class) ) {
	strcpy(dv, class);
	strcat(dv, ":");
        dvp = VConvert(&pvalue, class, StringType);
	strcat(dv, class);
    } else {
        VNewValue(&pvalue, (*params)->pvalue.value, (*params)->pvalue.type);
    }

    /* If the PVALUE of the value is an indirection lookup the
     * PWORTH of it. 
     */
    if ( dvp && VIsIndirect(&pvalue) ) {
      strcat(dv, " -> ");
      if ( xvalue = VAccess(pf, *params, StringType, &pvalue) )
	strcat(dv, pvalue.value);
      else
	strcat(dv, "INDEF");
    }
    if ( pvalue.value ) free(pvalue.value);

    strcpy(name, "(");
    strncat(name, (*params)->pname, SZ_PFLINE-2);
    name[SZ_PFLINE-1] = '\0';
    if( dvp ){
      strncpy(value, dv, SZ_PFLINE);
      value[SZ_PFLINE-1] = '\0';
    }
    else
      *value = '\0';
    strcat(value, ")");
    fprintf(stdout, "%14s = %-16s %s\n"
	   , name
	   , value
	   , (*params)->pprompt ? (*params)->pprompt : "");
  }
  fprintf(stdout, "\n");
}


/* Lookup a Parameter in a plist with exact strcmp
 */
Parameter *ParamLookup(plist, pname)
			ParamList	*plist;
			char		*pname;
{
  Parameter	**param;

  if ( plist == NULL ) return NULL;

  for ( param = plist->param; *param; param++ ) {
    if ( !strcmp((*param)->pname, pname) ) return (*param);
  }
  for ( param = plist->param; *param; param++ ) {
	if ( (*param)->ptype == PSetType ) {
		fprintf(stderr, "%s is of type pset\n", (*param)->pname);
	}
  }
  
  return NULL;
}

/* Lookup a Parameter in a plist with uniq match
 */
char *ParamMatch(pfile, pname)
	ParamFile	 pfile;
	char		*pname;
{
  ParamList	*plist = pfile->psets;
  Parameter	*a = NULL;
  Parameter	*b = NULL;
  Parameter	**param;
  char 		name[256];
  char 		*pn;
  int		mode = HMODE|(pfile->mode&HHMODE);

  if ( plist == NULL ) return NewString(pname);

  /* don't try to look up extension part */
  strcpy(name, pname);
  if ( pn = strrchr(name, '.') ) {
	  if ( !strcmp(pn, ".p_mode") ||
	       !strcmp(pn, ".p_value")||
	       !strcmp(pn, ".p_min")  ||
	       !strcmp(pn, ".p_max")  ||
	       !strcmp(pn, ".p_prompt") ){
	    *pn = '\0';
	  }
  }

  for ( param = plist->param; *param; param++ ) {

    if ( !strcmp((*param)->pname, name) )	return pname;

    if ( ( strstr((*param)->pname, name) == (*param)->pname ) )
      if ( a && (b==NULL) )	b = *param;
      else     			a = *param;
  }

  if ( a && b ) {
    fprintf(stderr, "parammatch : parameter name %s not uniq (%s, %s)\n"
	   , pname
	   , a->pname, b->pname);

    parerr = PARNOTUNIQ;
    return NULL;
  }

  for ( param = plist->param; *param; param++ ) {
	if ( (*param)->ptype == PSetType ) {
		void	*converted = NULL;
		    converted = ParamGetX(pfile, (*param)->pname, (*param)->ptype, &mode);
		
		fprintf(stderr, "%s is of type pset: %s\n", (*param)->pname, converted);
	}
  }


  if ( a == NULL ) {
    fprintf(stderr, "parammatch : can't find parameter %s in reference list\n"
	   , pname);
    return NULL;
  }

  free(pname);

  pname = NewString(a->pname);
  return pname;
}


/* Parameter structure contruction routines */

void PFree(plist)
			ParamList	*plist;
{
			Parameter	**param;

      if ( plist == NULL ) return;

      for ( param = plist->param; *param; param++ )
	    ParamFree(*param);

      if ( plist->file )     fclose(plist->file);
      if ( plist->filename ) free(plist->filename);
      if ( plist->psetname ) free(plist->psetname);
      if ( plist->mode )     free(plist->mode);
      free(plist);
}


void ParamFree(param)
     Parameter	*param;
{
  if ( param->pname )	 	free(param->pname);
  if ( param->pvalue.value ) 	free(param->pvalue.value);
  if ( param->pmin.value ) 	free(param->pmin.value);
  if ( param->pmax.value ) 	free(param->pmax.value);
  if ( param->pprompt ) 	free(param->pprompt);
	
  free(param);
}


ParamList *PListAdd(plist, param)
     ParamList	*plist;
     Parameter	*param;
{
  if ( param == NULL ) return NULL;

  if ( plist == NULL ) {
    plist = (ParamList *)malloc(sizeof(ParamList));
    plist->mode   = NULL;
    plist->nalloc = 10;
    plist->nparam = 0;
  }

  if ( plist->nparam + 1 >= plist->nalloc ) {
    plist->nalloc += PFILE_SZ;
    plist = (ParamList *)realloc(plist,
	     sizeof(ParamList) + sizeof(Parameter *) * (plist->nalloc - 10));
  }

  plist->param[plist->nparam++] = param;
  plist->param[plist->nparam]   = NULL;

  return plist;
}

ParamList *PListDel(plist, param)
     ParamList	*plist;
     Parameter	*param;
{
  Parameter	**p = plist->param;

  while ( *p && *p != param ) p++;

  if ( *p ) {
    int	n;
    n = plist->nparam - (++p - plist->param);

    bcopy(p, p - 1, n);
    plist->param[--plist->nparam] = NULL;
  }

  return plist;
}



int	c_paccess(pfile, pname)
     ParamFile	 pfile;
     char		*pname;
{
  if ( ParamLookup(pfile->psets, pname) ) {
    return 1;
  } else {
    parerr = 0;
    return 0;
  }
}

char *pfilelast()
{
  return(pfile_last);
}

