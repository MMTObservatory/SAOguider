/* Creation routines.
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

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"
#include "range.h"

static char *PParseFile();

Parameter *ParamAlloc()
{
		Parameter *p;

	p = (Parameter *)malloc(sizeof(Parameter));

	p->pname	= NULL;
	p->ptype	= 0;
	p->phook	= NULL;
	p->pmode	= 0;
	p->pvalue.value	= NULL;
	p->pvalue.indir	= NULL;
	p->pvalue.type	= 0;
	p->pmin.value 	= NULL;
	p->pmin.indir	= NULL;
	p->pmin.type 	= 0;
	p->pmax.value	= NULL;
	p->pmax.indir	= NULL;
	p->pmax.type	= 0;
	p->pprompt	= NULL;
	p->pflag	= 0;
	return p;
}


Parameter *ParamName(p, name)
			Parameter	*p;
			char		*name;
{
	if ( p == NULL ) p = ParamAlloc();

	p->pname	= name;

	return p;
}



Parameter *ParamType(p, type)
			Parameter	*p;
			VType		type;
{
	if ( type == 0 ) return NULL;
	if ( p == NULL ) p = ParamAlloc();

	p->ptype	= type;

	return p;
}



Parameter *ParamMode(p, mode)
			Parameter	*p;
			int		*mode;
{
	if ( mode == NULL ) return NULL;
	if ( p == NULL ) p = ParamAlloc();

	p->pmode	= *mode;

	return p;
}



Parameter *ParamMin(p, type, min)
			Parameter	*p;
			VType		type;
			void		*min;
{
	if ( p == NULL ) p = ParamAlloc();

	p->pmin.type	= type;
	p->pmin.value	= min;

	return p;
}



Parameter *ParamMax(p, type, max)
			Parameter	*p;
			VType		type;
			char		*max;
{
	if ( p == NULL ) p = ParamAlloc();

	p->pmax.type	= type;
	p->pmax.value	= max;

	return p;
}



Parameter *ParamValue(p, type, value)
			Parameter	*p;
			VType		type;
			char		*value;
{
	if ( p == NULL ) p = ParamAlloc();

	p->pvalue.type	= type;
	p->pvalue.value = value;

	return p;
}



Parameter *ParamPrompt(p, prompt)
			Parameter	*p;
			char		*prompt;
{
	if ( p == NULL ) p = ParamAlloc();

	p->pprompt = prompt;

	return p;
}


Parameter *ParamStruct(param, line)
			Parameter	*param;
			char		*line;
{
	param->pvalue.value = line;

	return param;
}


int paramvalid(pfile, pname, pvalue, cvalue)
		ParamFile pfile;
		char *pname;
		char *pvalue;
		char *cvalue;		/* Cannonical value		*/
{
	Parameter	*param;
	Value  		 value;
	Value 		*check;

  if ( pfile == NULL ) return 0;

  value.type  = StringType;
  value.value = NewString(pvalue);
  value.indir = NULL;

  if ( (param = ParamLookup(pfile->psets, pname))==NULL ) return 0;

  if ( check = Validate(pfile, param, &value) ) {
	if ( cvalue ) {
		Value            tvalue;

	    tvalue.type  = StringType;
	    tvalue.value = pvalue;
	    tvalue.indir = NULL;

	    if ( VIsIndirect(&tvalue) )
		strcpy(cvalue, pvalue);
	    else
		Convert(check->value, check->type, cvalue, StringType);	

	    free(value.value);
	    if( check != &value )
	      free(check);
	    return 1;
 	}
	return 1;
  } else {
	free(value.value);
	return 0;
  }
}



/* Methods */

int VXSize(value)
		void *value;
{
	return sizeof(int);
}


Value *Validate(pfile, p, value)
			ParamFile	 pfile;
			Parameter	*p;
			Value		*value;
{

	if ( p == NULL ) 					return NULL;

	if ( p->ptype & ListType && !IsInList(p) )
		if ( !VAccess(pfile, p, FileType | ListType, value) )
			return NULL;
		else
			return value;

	if ( !VAccess(pfile, p, p->ptype, value) )	return NULL;

	if ( !VRange(pfile, p, &p->pmin) ) 
	    return PRangeMinMax(value, VAccess(pfile, p, p->ptype, &p->pmin)
				     , VAccess(pfile, p, p->ptype, &p->pmax));

	if ( p->pmax.value ) {
		parerr = PARRANGEMAX;
		return NULL;
	}

	if ( p->pmin.value == NULL ) {
		parerr = PARBADRANGE;
		return NULL;
	}

	return VInRange(p->pmin.value, value);
}

Value *VXAcce(pfile, p, type, value)
			ParamFile	 pfile;
			Parameter	*p;
			VType	 	 type;
			Value		*value;
{
		void	*converted;

	if ( type ==    0  ) 			return NULL;
	if ( value == NULL )			return NULL;

	VDirect(pfile, p, value);
	if ( value->value == NULL )		return NULL;

	if ( p->ptype & ListType && p->phook == NULL ) {
		if ( TypeIndex(value->type) != StringType
		  && !Access(value->value, "r") ) {
			parerr = PARNOLISTFILE;
			return NULL;
		} else {
			p->phook = NextListValue;
			p->extension =(listrecord *)malloc(sizeof(listrecord));

			if ( (((listrecord *) p->extension)->file
				= fopen(value->value, "r")) == NULL ) {
				p->phook = NULL;
				free(p->extension);
				parerr = PARNOLISTFILE;

				return NULL;
			}
		}
	}

	if ( value->type == type )	return value;

	if ( (converted = VConvert(value, NULL, type)) == NULL ) return NULL;

	return VNewValue(value, converted, type);
}


Value *VNewValue(pvalue, value, type)
			Value	*pvalue;
			void	*value;
			VType	 type;
{
		void	*new;
		int	 size;

	if ( pvalue == NULL ) return NULL;
	if ( value == NULL ) return NULL;
	if ( type ==    0  ) return NULL;

	size = VSize(type, value);
	new = (char *)malloc(size);
	bcopy(value, new, size);

	if ( pvalue->value ) free(pvalue->value);

	pvalue->type  = type;
	pvalue->value = new;

	return pvalue;
}


Value *VIsIndirect(value)
			Value	*value;
{
        if( value && value->indir )
	        return value;
	if ( value
	  && value->value
	  && TypeIndex(value->type) == StringType
	  && *(char *) value->value == ')' )
		return value;
	else
		return NULL;
}


void VDirect(pfile, param, value)
			ParamFile	 pfile;
     			Parameter	*param;
			Value		*value;
{
		char	fname[SZ_PFLINE];
		char	pname[SZ_PFLINE];
		int	mode = HMODE|(pfile->mode&HHMODE);
		void	*converted = NULL;
		char    *indirect;

               char     class[1024];

	/* check for indirect classing and change value if necessary */
        IndirectClassing(pfile, param, value, class);

	if ( !VIsIndirect(value) ) return;

	if ( value->indir ){
	  if( value->value )
	    free(value->value);
	  value->type = StringType;
	  value->value = value->indir;
	  value->indir = NULL;
	}

        /* look for )) -- execute command and return output */
	indirect = (char *)value->value;
	indirect++;
	if(  *indirect == ')' ){
	  int got;
	  char *s;
	  char buf[SZ_PFLINE*2];
	  FILE *fd;

	  value->indir = value->value;
	  value->value = NULL;

	  indirect++;
	  if( (fd = popen(indirect, "r")) == NULL ){
	    parerr = PARBADINDIRECT;
	    return;
	  }
	  got = 0;
	  *buf = '\0';
	  while( (s=fgets(&buf[got], SZ_PFLINE, fd)) ){
	    got += strlen(s);
	    if( buf[got-1] == '\n' )
	      buf[got-1] = '\0';
	    if( got > SZ_PFLINE ){
	      buf[SZ_PFLINE] = '\0';
	      break;
	    }
	  }
	  pclose(fd);
	  VNewValue(value, buf, StringType);
	  return;
	}

	if ( PParseFile(value->value, fname, pname) ) {

	    value->indir = value->value;
	    value->value = NULL;

	    if ( *fname == '\0' )		/* Local indirection	*/
	        if( strcmp(pname, param->pname) ){
		  converted = ParamGetX(pfile, pname, value->type, &mode);
		}
	        else{
		  parerr = PARBADINDIRECT;
		  return;
		}
	    else {	
		if ( (pfile = ParamFileLookup(c_paramfind(fname, "r",
					PARAMEXTN, PARAMPATH))) ){
		    converted = ParamGetX(pfile, pname, value->type, &mode);
		} else
		    if ( pfile = (ParamFile) paramopen(fname, NULL, 0, "rH") ) {
		    	converted = ParamGetX(pfile, pname, value->type, &mode);
			paramclose(pfile);
		    }
	    }

	    if ( converted ) VNewValue(value, converted, value->type);
	    else {
		parerr = PARBADINDIRECT;
	    }

	    return;
	}	

	parerr = PARNOFILE;
	free(value->value);
	return;
}


static char *PParseFile(value, fname, pname)
                        char    *value;
                        char    *fname;
                        char    *pname;
{
                char    *pn;
		char    *ext=NULL;

        if ( value == NULL )            return NULL;
        if ( *value != ')' )            return NULL;
        value++;

        if ( *value == '*' ) value++;

        strcpy(fname, value);

        if ( pn = strrchr(fname, '.') ) {
	  if ( !strcmp(pn, ".p_mode") ||
	       !strcmp(pn, ".p_value")||
	       !strcmp(pn, ".p_min")  ||
	       !strcmp(pn, ".p_max")  ||
	       !strcmp(pn, ".p_prompt") ){
	    ext = pn+1;
	    *pn = '\0';
	    pn = strrchr(fname, '.');
	  }
	  if( pn ){
	    *pn++ = '\0';
	    strcpy(pname, pn);
	  }
	  else{
	    strcpy(pname, fname);
	    *fname = '\0';
	  }
	  if( ext ){
	    strcat(pname, ".");
	    strcat(pname, ext);
	  }
        } else {
                strcpy(pname, fname);
                *fname = '\0';
        }

        return fname;
}

/* conversion buffer.  it's double instead of char to avoid alignment
   problems if a double gets dump at the start */
static double PConverterBuffer[SZ_PFLINE / sizeof(double) + 1];
/* static char PConverterBuffer[SZ_PFLINE]; */

void *Convert(value, from, copy, to)
			void	*value;
			VType	from;
			void	*copy;
			VType	to;
{
		Converter	*conv;

	if ( (value==NULL) || (to==0) ) {
		if ( copy ) *((char *) copy) = '\0';
		return NULL;
	}

	if ( copy == NULL )  copy = PConverterBuffer;

	if ( TypeIndex(from) == TypeIndex(to) ) {
		bcopy(value, copy, VSize(from, value));
		return copy;
	} else
		for ( conv = VTypeList[TypeIndex(to)]->converters
		    ; conv->from; conv++ )
			if ( TypeIndex(conv->from) == TypeIndex(from) )
				return (*conv->convert)(value, copy);

	parerr = PARNOCONVERTER;
	return NULL;
}
 

void *VConvert(value, copy, to)
			Value	*value;
			void	*copy;
			VType	to;
{
	if ( value == NULL ) return NULL;

	return Convert(value->value, value->type, copy, to);
}
