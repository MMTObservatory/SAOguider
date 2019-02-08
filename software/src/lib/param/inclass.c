/* 
 *
 * InClassing -- routines to allow classing of indirect parameters
 *
 * It works like this:  you create an ASCII data base that maps the names of
 * parameters in parameter files to other parameters in other parameter files.
 * For example:
 *

 # input pfile		input param	indirect pfile		indirect param
 # -----------		-----------	--------------		--------------
 scale.par		input		$CDS1			input_file
 scale.par		output		$CDS1			output_file
 scale.par		ncolors		$CDS2			num_colors

 * This maps 3 parameters in the scale parameter file to three parameters
 * in param files whose names are contained in environment variables.
 *
 * If the PINCLASSDB environment variable is set to this ASCII file, then
 * the file will be used as the current classing data base.  When a scale
 * parameter is accessed, the data base will be checked for a template match.
 * If a match is made with the inout parameter, the target parameter is checked
 * for. If the target parameter is found, then its value is used in place
 * of the value in the scale param file.  If the parameter is not in the target
 * param file or the target param file does not exist, then the value in the
 * scale param file is used.  In effect, when you turn in indirect classing,
 * you can make several parameters in different files point to the same
 * parameter -- this is the essence of the idea of a "common data set".
 *
 * Note that the use of environment variables in the indirect pfile means
 * that it is easy to change between common data sets, simply by changing
 * the value of the environment variable.  The variable names are arbitrary.
 *
 *
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
#include <sys/stat.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"

/*
 *
 *		Private Routines
 *
 */

#define INDIRECT_CLASS_FILE "PINCLASSDB"
#define MAXIDX 26
#define BUFINC 512

typedef struct indirrec{
  char *pfile;
  char *param;
  char *ipfile;
  char *iparam;
} IndirEntry;

typedef struct indirdbrec{
  int nentry;
  int maxentry;
  IndirEntry *entry;
} IndirDB;

static char *indirect_file=NULL;
static struct stat file_info;
static IndirDB indirect_database[MAXIDX+1][MAXIDX+1];
static char **indirect_pfiles=NULL;
static int npfiles=0;
static int maxpfiles=0;
static indirect_error=0;
static indirect_return=0;
static time_t last_st_mtime;

#ifdef SZ_LINE
#undef SZ_LINE
#endif
#define SZ_LINE 256

/*
 *
 * GetIDBIndex -- get the index into the indirect data base
 *
 */
static void GetIDBIndex(pfile, param, idx1, idx2)
     char *pfile;
     char *param;
     int *idx1;
     int *idx2;
{
  *idx1 = *pfile - 'a';
  if( (*idx1 < 0) || (*idx1 > MAXIDX) )
    *idx1 = MAXIDX;
  *idx2 = *param - 'a';
  if( (*idx2 < 0) || (*idx2 > MAXIDX) )
    *idx2 = MAXIDX;
}

/*
 *
 * CreateIndirectClassingDB -- create the indirect class data base
 *
 */
static void CreateIndirectClassingDB(fname)
     char *fname;
{
  char buf[SZ_LINE+1];
  FILE *fd;
  IndirDB *idb;
  int i;
  int ip;
  int idx1, idx2;
  char pfile[SZ_LINE];
  char param[SZ_LINE];
  char ipfile[SZ_LINE];
  char iparam[SZ_LINE];

  if( (fd = fopen(fname, "r")) == NULL ){
    fprintf(stderr, "Warning: can't open indirect classing file '%s'\n",
	    fname);
    return;
  }
  while( fgets(buf, SZ_PFLINE, fd) ){
    if( (*buf == '#') || (*buf == '\n') )
      continue;
    ip = 0;
    if( Word(buf, pfile, &ip) && Word(buf, param, &ip) &&
        Word(buf, ipfile, &ip) && Word(buf, iparam, &ip) ){
      GetIDBIndex(pfile, param, &idx1, &idx2);
      idb = &(indirect_database[idx1][idx2]);
      if( idb->maxentry  == 0 ){
	idb->maxentry += BUFINC;
	idb->entry = (IndirEntry *)malloc(idb->maxentry);
      }
      if( idb->nentry >= idb->maxentry ){
	idb->maxentry += BUFINC;
	idb->entry = (IndirEntry *)realloc(idb->entry, idb->maxentry);
      }
      idb->entry[idb->nentry].pfile = NewString(pfile);
      idb->entry[idb->nentry].param = NewString(param);
      idb->entry[idb->nentry].ipfile = NewString(ipfile);
      idb->entry[idb->nentry].iparam = NewString(iparam);
      /* enter the target pfile into the pfile list */
      if( maxpfiles == 0 ){
	maxpfiles += BUFINC;
	indirect_pfiles = (char **)malloc(maxpfiles);
      }
      if( npfiles >= maxpfiles ){
	maxpfiles += BUFINC;
	indirect_pfiles = (char **)realloc(indirect_pfiles, maxpfiles);
      }
      for(i=0; i<npfiles; i++){
	if( !strcmp(indirect_pfiles[i], idb->entry[idb->nentry].ipfile) )
	  break;
      }
      if( i == npfiles ){
	indirect_pfiles[npfiles] = idb->entry[idb->nentry].ipfile;
	npfiles++;
      }
      idb->nentry++;
    }
    else{
      fprintf(stderr, "Warning: illegal indirect classing line:\n%s", buf);
    }
  }
  fclose(fd);
}
	
/*
 *
 * DestroyIndirectClassingDB() -- remove all entries from the data base
 * (but leave allocated space alone)
 *
 */
static void DestroyIndirectClassingDB()
{
  int i, j;
  int k;
  IndirDB *idb;
  for(i=0; i<=MAXIDX; i++){
    for(j=0; j<=MAXIDX; j++){
      idb = &(indirect_database[i][j]);
      for(k=0; k<idb->nentry; k++){
	free(idb->entry[k].pfile);
	free(idb->entry[k].param);
	free(idb->entry[k].ipfile);
	free(idb->entry[k].iparam);
      }
      idb->nentry = 0;
    }
  }
  npfiles = 0;
}
  

/*
 *
 * LookupIndirectClassing -- check to see if this parameter matches a class
 * in the indirect data base
 *
 */
static IndirEntry *LookupIndirectClassing(pfile, param)
  char *pfile;
  char *param;
{
  int i;
  int idx1, idx2;
  IndirDB *idb;

  GetIDBIndex(pfile, param, &idx1, &idx2);
  idb = &(indirect_database[idx1][idx2]);
  /* first look for an exact match */
  for(i=0; i<idb->nentry; i++){
    if( !strcmp(pfile, idb->entry[i].pfile) &&
        !strcmp(param, idb->entry[i].param) )
      return(&(idb->entry[i]));
  }
  /* then look for a template match */
  for(i=0; i<idb->nentry; i++){
    if( tmatch(pfile, idb->entry[i].pfile) &&
        tmatch(param, idb->entry[i].param) )
      return(&(idb->entry[i]));
  }
  return(NULL);
}

/*
 *
 *		Public Routines
 *
 */

/*
 *
 * GetIndirectPFiles-- return list of indirect pfiles
 * NB: env variables are not expanded at this point!
 *
 */
int GetIndirectPFiles(pfiles, maxpfiles)
     char **pfiles;
     int maxpfiles;
{
  int i;
  for(i=0; (i<maxpfiles) && (i<npfiles); i++){
    pfiles[i] = indirect_pfiles[i];
  }
  return(i);
}

/*
 *
 * UsingIndirectClassing -- check if we are using indirect classing,
 * and if so, init the data, if necessary
 *
 */
int UsingIndirectClassing()
{
  char *file=NULL;
  char *getenv();

  file = getenv(INDIRECT_CLASS_FILE);
  if( (file == NULL) || (*file == '\0') || !strcmp(file, "NONE") ){
    if( indirect_file != NULL ){
      free(indirect_file);
      indirect_file = NULL;
      /* this data base no longer is valid */
      DestroyIndirectClassingDB();
    }
    indirect_error = 0;
    indirect_return = 0;
    return(indirect_return);
  }
  if( indirect_file == NULL ){
    /* copy in new file name */
    indirect_file = NewString(file);
    /* check for file existence */
    if ( stat(indirect_file, &file_info) !=0 ){
      if( !indirect_error )
	fprintf(stderr,
		"Warning: indirect classing file '%s' does not exist\n",
		indirect_file);
      indirect_error = 1;
      indirect_return = 0;
      return(indirect_return);
    }
    else{
      CreateIndirectClassingDB(indirect_file);
      indirect_error = 0;
      indirect_return = 2;
      return(indirect_return);
    }
  }
  else if( strcmp(indirect_file, file) ){
    free(indirect_file);
    /* copy in new file name */
    indirect_file = NewString(file);
    /* this data base no longer is valid */
    DestroyIndirectClassingDB();
    /* check for file existence */
    if ( stat(indirect_file, &file_info) !=0 ){
      if( !indirect_error )
	fprintf(stderr,
		"Warning: indirect classing file '%s' does not exist\n",
		indirect_file);
      indirect_error = 1;
      indirect_return = 0;
      return(indirect_return);
    }
    else{
      last_st_mtime = file_info.st_mtime;
      CreateIndirectClassingDB(indirect_file);
      indirect_error = 0;
      indirect_return = 2;
      return(indirect_return);
    }
  }
#ifdef INCLASS_STAT
  else{
    if( stat(file, &file_info) !=0 ){
      if( !indirect_error )
	fprintf(stderr,
		"Warning: indirect classing file '%s' does not exist\n",
		indirect_file);
      indirect_error = 1;
      return(0);
    }
    else{
      if( file_info.st_mtime > last_st_mtime ){
	last_st_mtime = file_info.st_mtime;
	DestroyIndirectClassingDB();
	CreateIndirectClassingDB(indirect_file);
	indirect_error = 0;
	return(2);
      }
      else{
	indirect_error = 0;
	return(1);
      }
    }
  }
#else
  else{
    if( indirect_return == 2 )
      indirect_return = 1;
    return(indirect_return);
  }
#endif
}

/*
 *
 * IndirectClassing -- check to see if this parameter should use indirect
 * classing and change the value if it should
 *
 */
int IndirectClassing(pf, p, value, class)
     ParamFile	 pf;
     Parameter	*p;
     Value	*value;
     char	*class;
{
  IndirEntry *entry;
  ParamFile tpf;
  int	mode = HMODE|(pf->mode&HHMODE);
  void	*converted = NULL;
  char *pp;
  char *param;
  char pfile[SZ_LINE];
  char ipfile[SZ_LINE];

  /* init the data base */
  if( !UsingIndirectClassing() )
    return(0);

  /* get the pfile name, without the pathname and without the extension */
  if( (pp = strrchr(pf->psets->filename, '/')) != NULL )
    pp++;
  else
    pp = pf->psets->filename;
  strcpy(pfile, pp);
  if( (pp = strrchr(pfile, '.')) != NULL )
    *pp = '\0';
  /* get the param name */
  param = p->pname;
  /* look for a template match with this param file and param */
  if( (entry=LookupIndirectClassing(pfile, param)) != NULL ){
    /* expand the new param file -- it might be an environment variable */
    ExpandEnv(entry->ipfile, ipfile, SZ_LINE);
    /* try to access the new parameter in the new param file */
    if ( (tpf = ParamFileLookup(c_paramfind(ipfile, "r",
					PARAMEXTN, PARAMPATH))) ){
      converted = ParamGetX(tpf, entry->iparam, value->type, &mode);
    } else {
      if( tpf = (ParamFile)paramopen(ipfile, NULL, 0, "rH") ) {
	converted = ParamGetX(tpf, entry->iparam, value->type, &mode);
	paramclose(tpf);
      }
    }
    /* if we did get a new value, make it stick */
    if( converted ){
      VNewValue(value, converted, value->type);
      strcpy(class, entry->ipfile);
      return(1);
    }
    else
      return(0);
  }
  else
    return(0);
}

/*
 *
 * LookupInClass -- check to see if this parameter matches a class
 * in the indirect data base and return the class and parameter
 *
 */
int LookupInClass(pfile, param, ipfile, iparam)
     char *pfile;
     char *param;
     char **ipfile;
     char **iparam;
{
  IndirEntry *entry;

  entry = LookupIndirectClassing(pfile, param);
  if( entry != NULL ){
    *ipfile = entry->ipfile;
    *iparam = entry->iparam;
    return(1);
  }
  else {
    *ipfile = NULL;
    *iparam = NULL;
    return(0);
  }
}

