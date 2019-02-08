/* pfile.h
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

#define PARAMPATH	"$PDIRS $PFILES $UPARM ."
#define PARAMEXTN	".par .rdb $PFEXTN"

#define INDEFS          (-32767)
#define INDEFL          ((unsigned long) 0x80000001)
#define INDEFI          INDEFL
#define INDEFR          1.6e38
#define INDEFD          1.6e38
#define INDEF           INDEFR

typedef double	real;
typedef int	bool;

/* The Value structure    */

typedef struct _Value {
	int	 type;
	void	*value;
	char	*indir;
} Value;

typedef int VType;

typedef struct _Converter {
	int	from;
	int	to;
	void	*(*convert)();
} Converter;


/* The parameter structure */

typedef struct _Parameter {
	int	 ptype;
	int	 pflag;

	char	*pname;
	int	 pmode;
	Value	 pvalue;
	Value	 pmin;
	Value	 pmax;
	char	*pprompt;
	void	*extension;	
	Value	*(*phook)();
} Parameter;


typedef struct _ParamList {
	FILE	 	*file;
	char		*filename;
	char            *psetname;
	char		*mode;

	/* Methods 			*/
	void			 (*clos)();
	struct _ParamList	*(*read)();
	void			 (*writ)();

	int		nparam;
	int		nalloc;
	Parameter	*param[10];	
} ParamList;

typedef struct _ParamFile {
	struct _ParamFile *next;
	ParamList	  *psets;
	ParamList	  *alist;

	int	 	   mode;
	void		  *extension;
} *ParamFile;

typedef struct _ParamFormat {
	ParamList	*(*open)();
	char		*extn;
	char	        *(*find)();
} ParamFormat;


typedef struct _vtype {
	char			*name;
	struct _Converter	*converters;
	int	 (*size)();
	Value	*(*access)();
	int	 (*compar)();
} vtype;



/* ParamFile operations 	*/

ParamFile	  ParamFileLookup();
ParamFile	  ParamPSetLookup();
extern ParamList *ParamFileArgs();
extern ParamList *ParamArgsBase();
void PFree();
extern ParamList *ParamFileCant;

extern ParamList *IRAFStrOpen();
extern ParamList *IRAFParFile();
extern ParamList *IRAFParArgs();

VType String2Type();
void *Type2String();
void *Mode2String();



ParamList	*PListAdd();
ParamList	*PListDel();


/* Parameter Construction Utilities */

Parameter *ParamCreate();
Parameter *ParamStruct();
Parameter *ParamLookup();

Parameter *ParamAlloc();
Parameter *ParamName();
Parameter *ParamMode();
Parameter *ParamType();
Parameter *ParamValue();
Parameter *ParamMin();
Parameter *ParamMax();
Parameter *ParamPrompt();

Value	*VIsIndirect();
/*
Value	*ParamOp();
*/
Value 	*Validate();

char	*QuoteValue();

void	*Convert();
void	*VConvert();
char	*VString();

Value	*VNewValue();

char    *NewString();

/* Parameter method access macros (operations on values) */

#define TypeBits	ListType
#define TypeIndex(x) ( x & ~TypeBits)

#define VAccess(pfile, p, type, value)					\
	( (p && VTypeList[TypeIndex(type)]->access) ?		 	\
	  (*(VTypeList[TypeIndex(type)]->access))(pfile, p, type, value)\
	: (Value *) NULL )

#define VCompar(type, value1, value2, op)				\
	  ( VTypeList[TypeIndex(type)]->compar ?			\
	     (*(VTypeList[TypeIndex(type)]->compar))(value1, value2, op) : 0 )


#define VSize(type, value)						\
	  ( VTypeList[TypeIndex(type)]->size ?				\
	     (*(VTypeList[TypeIndex(type)]->size))(value) : 0 )


#define PHooker(p)		( (p)->phook 				 \
					? (*((p)->phook))(p, value, mode)\
					: NULL )


/* Middle level interface */

int	 ParamSetX();
void 	*ParamGetX();


void VDirect();

int    VXSize();
Value *VXAcce();


/* Parameter Types */

extern vtype	*VTypeList[];

#define ListType	0x1000

#define BooleanType	 1
#define IntegerType	 2
#define RealType	 3
#define StringType	 4
#define FileType	 5
#define GType	 	 6
#define StructType	 7
#define GCurType	 8
#define ImCurType	 9
#define ModeType	10
#define PSetType	11
#define RangeType	12
#define CommentType	13

#define MaxTypes	13

extern vtype booleanRec;
extern vtype integerRec;
extern vtype realRec;
extern vtype stringRec;
extern vtype fileRec;
extern vtype gRec;
extern vtype structRec;
extern vtype modeRec;
extern vtype gcurRec;
extern vtype imcurRec;
extern vtype psetRec;
extern vtype isetRec;
extern vtype rangeRec;
extern vtype commentRec;


/* Size of things */

#define PSTAK_SZ	10
#define PFILE_SZ	30
#define FIELD_SZ	2048

/* Parameter Fields */

#define PMODE	1
/*
#define PTYPE	7
*/
#define PVALUE	2
#define PMIN	3
#define PMAX	4
#define PPROMPT	5
#define PWORTH	6


/* Modes */

#define AMODE		1
#define HMODE		2
#define LMODE		4
#define QMODE		8
#define HHMODE		16

#ifndef SZ_PFLINE
#define SZ_PFLINE	2048
#endif

/* ERRORS */

extern int parerr;

#define PARNOERROR	0
#define	PARNOTFOUND	1
#define	PARNOTUNIQ	2
#define	PARNOFILE	3
#define PARARGS		4
#define PARPOSITIONAL	5
#define PARBADTYPE	6
#define PARBADMIN	7
#define PARBADMAX	8
#define PARLIMITMIN	9
#define PARLIMITMAX	10
#define PARNOPOINTER	11
#define PARCANTCONVERT	12
#define PARNOVALUE	13
#define PARBADARGV	14
#define PARBADPFILE	15
#define PARNOCONVERTER	16
#define PARBADMODE	17
#define PARSUBFIELD	18
#define PARRANGEMAX	19
#define PARBADRANGE	20
#define PARCANTRANGE	21
#define PARLIMITRANGE	22
#define PARTOOMANYPOS	23
#define PARCANTPARSE	24
#define PARENUMUNIQ	25
#define PARBADINDIRECT	26
#define PARNOLISTFILE	27
#define PARBADENUM	28
#define PARBACK2FRONT	29
#define PARNOREF	30
#define PARNOPSET	31
#define PARNOREFR	32

extern Value *NextListValue();

typedef struct _listrecord {
	FILE	*file;
	int	n;
} listrecord;


#define IsInList(x)	((x)->phook != NULL )
