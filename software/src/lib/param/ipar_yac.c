
/*  A Bison parser, made from ipar.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_comment	258
#define	T_struct	259
#define	T_word	260
#define	T_line	261
#define	T_nl	262

#line 1 "ipar.y"

/* ipar.y
**/

#include <sys/types.h>
#include <sys/stat.h>

#include "pfile.h"

extern int *String2Mode();

/* These defines must be kept in sync with the %s statement 
** in the lexical analyser
**/

#define NAME	1
#define TYPE	2
#define VALUE	3
#define LINE	4
#define DQUOTE	5
#define SQUOTE	6

static ParamList *result;
/* char *malloc(); */

int	iparlineno;

Parameter *ParamCreate();
Parameter *ParamMinMaxPromptPart();

static int filesize();

#define YYDEBUG 1
#define YYINITDEPTH 256
#define YYMAXDEPTH  257

#line 38 "ipar.y"
typedef union {
	ParamList	*plist;
	Parameter	*param;
	char		*stri;
} YYSTYPE;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		39
#define	YYFLAG		-32768
#define	YYNTBASE	9

#define YYTRANSLATE(x) ((unsigned)(x) <= 262 ? ipartranslate[x] : 18)

static const char ipartranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     8,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7
};

#if YYDEBUG != 0
static const short iparprhs[] = {     0,
     0,     2,     4,     7,     9,    11,    14,    16,    26,    35,
    36,    48,    51,    53,    54,    56,    59,    63,    65
};

static const short iparrhs[] = {    10,
     0,    11,     0,    10,    11,     0,    12,     0,    13,     0,
     3,     7,     0,     7,     0,     5,     8,     5,     8,     5,
     8,     5,    16,     7,     0,     5,     8,     5,     8,     5,
     8,    16,     7,     0,     0,     5,     8,     4,     8,     5,
     8,     5,    16,     7,    14,    15,     0,     6,     7,     0,
     7,     0,     0,    17,     0,    17,    17,     0,    17,    17,
    17,     0,     8,     0,     8,     5,     0
};

#endif

#if YYDEBUG != 0
static const short iparrline[] = { 0,
    63,    67,    74,    82,    83,    84,    85,    93,   101,   108,
   119,   124,   126,   130,   132,   136,   142,   151,   153
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const ipartname[] = {   "$","error","$undefined.","T_comment",
"T_struct","T_word","T_line","T_nl","','","pfile","plist","param","ptypedp",
"pstruct","@1","lstruct","pminmaxprompt","piece", NULL
};
#endif

static const short iparr1[] = {     0,
     9,    10,    10,    11,    11,    11,    11,    12,    12,    14,
    13,    15,    15,    16,    16,    16,    16,    17,    17
};

static const short iparr2[] = {     0,
     1,     1,     2,     1,     1,     2,     1,     9,     8,     0,
    11,     2,     1,     0,     1,     2,     3,     1,     2
};

static const short ipardefact[] = {     0,
     0,     0,     7,     1,     2,     4,     5,     6,     0,     3,
     0,     0,     0,     0,     0,     0,     0,    14,    14,    14,
    18,     0,    15,     0,     0,    19,     9,    16,    10,     8,
    17,     0,     0,    13,    11,    12,     0,     0,     0
};

static const short ipardefgoto[] = {    37,
     4,     5,     6,     7,    32,    35,    22,    23
};

static const short iparpact[] = {    -1,
    -4,    -7,-32768,    -1,-32768,-32768,-32768,-32768,     4,-32768,
     7,     8,    12,    13,    11,    14,    15,     2,    16,    16,
    18,    19,    16,    20,    21,-32768,-32768,    16,-32768,-32768,
-32768,     5,    22,-32768,-32768,-32768,    25,    30,-32768
};

static const short iparpgoto[] = {-32768,
-32768,    17,-32768,-32768,-32768,-32768,    -6,   -23
};


#define	YYLAST		30


static const short ipartable[] = {    28,
     9,     1,     8,     2,    31,     3,    20,    11,    12,    21,
    33,    34,    24,    25,    13,    14,    15,    16,    17,    19,
    10,    18,    26,    21,    38,    27,    29,    30,    36,    39
};

static const short iparcheck[] = {    23,
     8,     3,     7,     5,    28,     7,     5,     4,     5,     8,
     6,     7,    19,    20,     8,     8,     5,     5,     8,     5,
     4,     8,     5,     8,     0,     7,     7,     7,     7,     0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/home/john/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define iparerrok		(iparerrstatus = 0)
#define iparclearin	(iparchar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto iparerrlab1
/* Like YYERROR except do call iparerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto iparerrlab
#define YYRECOVERING()  (!!iparerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (iparchar == YYEMPTY && iparlen == 1)				\
    { iparchar = (token), iparlval = (value);			\
      iparchar1 = YYTRANSLATE (iparchar);				\
      YYPOPSTACK;						\
      goto iparbackup;						\
    }								\
  else								\
    { iparerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		iparlex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		iparlex(&iparlval, &iparlloc, YYLEX_PARAM)
#else
#define YYLEX		iparlex(&iparlval, &iparlloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		iparlex(&iparlval, YYLEX_PARAM)
#else
#define YYLEX		iparlex(&iparlval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	iparchar;			/*  the lookahead symbol		*/
YYSTYPE	iparlval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE iparlloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int iparnerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int ipardebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int iparparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __ipar_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__ipar_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__ipar_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/home/john/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into iparparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
iparparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int iparstate;
  register int iparn;
  register short *iparssp;
  register YYSTYPE *iparvsp;
  int iparerrstatus;	/*  number of tokens to shift before error messages enabled */
  int iparchar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	iparssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE iparvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *iparss = iparssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *iparvs = iparvsa;	/*  to allow iparoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE iparlsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *iparls = iparlsa;
  YYLTYPE *iparlsp;

#define YYPOPSTACK   (iparvsp--, iparssp--, iparlsp--)
#else
#define YYPOPSTACK   (iparvsp--, iparssp--)
#endif

  int iparstacksize = YYINITDEPTH;

#ifdef YYPURE
  int iparchar;
  YYSTYPE iparlval;
  int iparnerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE iparlloc;
#endif
#endif

  YYSTYPE iparval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int iparlen;

#if YYDEBUG != 0
  if (ipardebug)
    fprintf(stderr, "Starting parse\n");
#endif

  iparstate = 0;
  iparerrstatus = 0;
  iparnerrs = 0;
  iparchar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  iparssp = iparss - 1;
  iparvsp = iparvs;
#ifdef YYLSP_NEEDED
  iparlsp = iparls;
#endif

/* Push a new state, which is found in  iparstate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
iparnewstate:

  *++iparssp = iparstate;

  if (iparssp >= iparss + iparstacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *iparvs1 = iparvs;
      short *iparss1 = iparss;
#ifdef YYLSP_NEEDED
      YYLTYPE *iparls1 = iparls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = iparssp - iparss + 1;

#ifdef iparoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if iparoverflow is a macro.  */
      iparoverflow("parser stack overflow",
		 &iparss1, size * sizeof (*iparssp),
		 &iparvs1, size * sizeof (*iparvsp),
		 &iparls1, size * sizeof (*iparlsp),
		 &iparstacksize);
#else
      iparoverflow("parser stack overflow",
		 &iparss1, size * sizeof (*iparssp),
		 &iparvs1, size * sizeof (*iparvsp),
		 &iparstacksize);
#endif

      iparss = iparss1; iparvs = iparvs1;
#ifdef YYLSP_NEEDED
      iparls = iparls1;
#endif
#else /* no iparoverflow */
      /* Extend the stack our own way.  */
      if (iparstacksize >= YYMAXDEPTH)
	{
	  iparerror("parser stack overflow");
	  return 2;
	}
      iparstacksize *= 2;
      if (iparstacksize > YYMAXDEPTH)
	iparstacksize = YYMAXDEPTH;
      iparss = (short *) alloca (iparstacksize * sizeof (*iparssp));
      __ipar_memcpy ((char *)iparss, (char *)iparss1, size * sizeof (*iparssp));
      iparvs = (YYSTYPE *) alloca (iparstacksize * sizeof (*iparvsp));
      __ipar_memcpy ((char *)iparvs, (char *)iparvs1, size * sizeof (*iparvsp));
#ifdef YYLSP_NEEDED
      iparls = (YYLTYPE *) alloca (iparstacksize * sizeof (*iparlsp));
      __ipar_memcpy ((char *)iparls, (char *)iparls1, size * sizeof (*iparlsp));
#endif
#endif /* no iparoverflow */

      iparssp = iparss + size - 1;
      iparvsp = iparvs + size - 1;
#ifdef YYLSP_NEEDED
      iparlsp = iparls + size - 1;
#endif

#if YYDEBUG != 0
      if (ipardebug)
	fprintf(stderr, "Stack size increased to %d\n", iparstacksize);
#endif

      if (iparssp >= iparss + iparstacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (ipardebug)
    fprintf(stderr, "Entering state %d\n", iparstate);
#endif

  goto iparbackup;
 iparbackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* iparresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  iparn = iparpact[iparstate];
  if (iparn == YYFLAG)
    goto ipardefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* iparchar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (iparchar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (ipardebug)
	fprintf(stderr, "Reading a token: ");
#endif
      iparchar = YYLEX;
    }

  /* Convert token to internal form (in iparchar1) for indexing tables with */

  if (iparchar <= 0)		/* This means end of input. */
    {
      iparchar1 = 0;
      iparchar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (ipardebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      iparchar1 = YYTRANSLATE(iparchar);

#if YYDEBUG != 0
      if (ipardebug)
	{
	  fprintf (stderr, "Next token is %d (%s", iparchar, ipartname[iparchar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, iparchar, iparlval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  iparn += iparchar1;
  if (iparn < 0 || iparn > YYLAST || iparcheck[iparn] != iparchar1)
    goto ipardefault;

  iparn = ipartable[iparn];

  /* iparn is what to do for this token type in this state.
     Negative => reduce, -iparn is rule number.
     Positive => shift, iparn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (iparn < 0)
    {
      if (iparn == YYFLAG)
	goto iparerrlab;
      iparn = -iparn;
      goto iparreduce;
    }
  else if (iparn == 0)
    goto iparerrlab;

  if (iparn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (ipardebug)
    fprintf(stderr, "Shifting token %d (%s), ", iparchar, ipartname[iparchar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (iparchar != YYEOF)
    iparchar = YYEMPTY;

  *++iparvsp = iparlval;
#ifdef YYLSP_NEEDED
  *++iparlsp = iparlloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (iparerrstatus) iparerrstatus--;

  iparstate = iparn;
  goto iparnewstate;

/* Do the default action for the current state.  */
ipardefault:

  iparn = ipardefact[iparstate];
  if (iparn == 0)
    goto iparerrlab;

/* Do a reduction.  iparn is the number of a rule to reduce with.  */
iparreduce:
  iparlen = iparr2[iparn];
  if (iparlen > 0)
    iparval = iparvsp[1-iparlen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (ipardebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       iparn, iparrline[iparn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = iparprhs[iparn]; iparrhs[i] > 0; i++)
	fprintf (stderr, "%s ", ipartname[iparrhs[i]]);
      fprintf (stderr, " -> %s\n", ipartname[iparr1[iparn]]);
    }
#endif


  switch (iparn) {

case 1:
#line 64 "ipar.y"
{ result = iparvsp[0].plist			;
    break;}
case 2:
#line 68 "ipar.y"
{ if ( iparvsp[0].param && (iparvsp[0].param->pvalue.value==NULL) )
			VNewValue(&iparvsp[0].param->pvalue, "", StringType);

		  if ( iparvsp[0].param != NULL )
			iparval.plist = PListAdd(NULL, iparvsp[0].param);
		;
    break;}
case 3:
#line 75 "ipar.y"
{ if ( iparvsp[0].param && (iparvsp[0].param->pvalue.value==NULL) )
			VNewValue(&iparvsp[0].param->pvalue, "", StringType);

		  if ( iparvsp[0].param != NULL )
			iparval.plist = PListAdd(iparvsp[-1].plist, iparvsp[0].param);	;
    break;}
case 7:
#line 86 "ipar.y"
{ 		int	mode = 0;
		  iparval.param = ParamCreate(strdup("")
				, CommentType 
				, &mode, strdup(""), NULL); 
		;
    break;}
case 8:
#line 94 "ipar.y"
{ 		int	mode;
		  iparval.param = ParamCreate(iparvsp[-8].stri
				, String2Type(iparvsp[-6].stri)
				, String2Mode(iparvsp[-4].stri, &mode), iparvsp[-2].stri, iparvsp[-1].param);
			if ( iparvsp[-6].stri != NULL ) free(iparvsp[-6].stri);
			if ( iparvsp[-4].stri != NULL ) free(iparvsp[-4].stri);
		;
    break;}
case 9:
#line 102 "ipar.y"
{ 		int	mode;
		  iparval.param = ParamCreate(iparvsp[-7].stri
				, String2Type(iparvsp[-5].stri)
				, String2Mode(iparvsp[-3].stri, &mode), NULL, iparvsp[-1].param); ;
    break;}
case 10:
#line 109 "ipar.y"
{ 		int	mode = 0;
		  iparval.param = ParamCreate(iparvsp[-8].stri
				, String2Type(iparvsp[-6].stri)
				, String2Mode(iparvsp[-4].stri, &mode), NULL, iparvsp[-1].param);
			if ( iparvsp[-6].stri != NULL ) free(iparvsp[-6].stri);
			if ( iparvsp[-4].stri != NULL ) free(iparvsp[-4].stri);

		  iparbegin(LINE);
		;
    break;}
case 11:
#line 119 "ipar.y"
{ iparval.param = ParamStruct(iparvsp[-1].param, iparvsp[0].stri);
		  iparbegin(NAME)
		;
    break;}
case 12:
#line 125 "ipar.y"
{ iparval.stri = iparvsp[-1].stri;		;
    break;}
case 13:
#line 127 "ipar.y"
{ iparval.stri = NULL;		;
    break;}
case 14:
#line 131 "ipar.y"
{ iparval.param = NULL;					;
    break;}
case 15:
#line 133 "ipar.y"
{ iparval.param = NULL;
		  if ( iparvsp[0].stri ) ParamMin(NULL, StringType, iparvsp[0].stri);
		;
    break;}
case 16:
#line 137 "ipar.y"
{ 
		  iparval.param = NULL;
		  if ( iparvsp[-1].stri ) iparval.param = ParamMin(NULL, StringType, iparvsp[-1].stri);
		  if ( iparvsp[0].stri ) iparval.param = ParamMax(  iparval.param, StringType, iparvsp[0].stri);
		;
    break;}
case 17:
#line 143 "ipar.y"
{
		  iparval.param = NULL;
		  if ( iparvsp[-2].stri ) iparval.param = ParamMin( NULL, StringType, iparvsp[-2].stri);
		  if ( iparvsp[-1].stri ) iparval.param = ParamMax(   iparval.param, StringType, iparvsp[-1].stri);
		  if ( iparvsp[0].stri ) iparval.param = ParamPrompt(iparval.param, iparvsp[0].stri);
		;
    break;}
case 18:
#line 152 "ipar.y"
{ iparval.stri = NULL					;
    break;}
case 19:
#line 154 "ipar.y"
{ iparval.stri = iparvsp[0].stri					;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/home/john/lib/bison.simple"

  iparvsp -= iparlen;
  iparssp -= iparlen;
#ifdef YYLSP_NEEDED
  iparlsp -= iparlen;
#endif

#if YYDEBUG != 0
  if (ipardebug)
    {
      short *ssp1 = iparss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != iparssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++iparvsp = iparval;

#ifdef YYLSP_NEEDED
  iparlsp++;
  if (iparlen == 0)
    {
      iparlsp->first_line = iparlloc.first_line;
      iparlsp->first_column = iparlloc.first_column;
      iparlsp->last_line = (iparlsp-1)->last_line;
      iparlsp->last_column = (iparlsp-1)->last_column;
      iparlsp->text = 0;
    }
  else
    {
      iparlsp->last_line = (iparlsp+iparlen-1)->last_line;
      iparlsp->last_column = (iparlsp+iparlen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  iparn = iparr1[iparn];

  iparstate = iparpgoto[iparn - YYNTBASE] + *iparssp;
  if (iparstate >= 0 && iparstate <= YYLAST && iparcheck[iparstate] == *iparssp)
    iparstate = ipartable[iparstate];
  else
    iparstate = ipardefgoto[iparn - YYNTBASE];

  goto iparnewstate;

iparerrlab:   /* here on detecting error */

  if (! iparerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++iparnerrs;

#ifdef YYERROR_VERBOSE
      iparn = iparpact[iparstate];

      if (iparn > YYFLAG && iparn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -iparn if nec to avoid negative indexes in iparcheck.  */
	  for (x = (iparn < 0 ? -iparn : 0);
	       x < (sizeof(ipartname) / sizeof(char *)); x++)
	    if (iparcheck[x + iparn] == x)
	      size += strlen(ipartname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (iparn < 0 ? -iparn : 0);
		       x < (sizeof(ipartname) / sizeof(char *)); x++)
		    if (iparcheck[x + iparn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, ipartname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      iparerror(msg);
	      free(msg);
	    }
	  else
	    iparerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	iparerror("parse error");
    }

  goto iparerrlab1;
iparerrlab1:   /* here on error raised explicitly by an action */

  if (iparerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (iparchar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (ipardebug)
	fprintf(stderr, "Discarding token %d (%s).\n", iparchar, ipartname[iparchar1]);
#endif

      iparchar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  iparerrstatus = 3;		/* Each real token shifted decrements this */

  goto iparerrhandle;

iparerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  iparn = ipardefact[iparstate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (iparn) goto ipardefault;
#endif

iparerrpop:   /* pop the current state because it cannot handle the error token */

  if (iparssp == iparss) YYABORT;
  iparvsp--;
  iparstate = *--iparssp;
#ifdef YYLSP_NEEDED
  iparlsp--;
#endif

#if YYDEBUG != 0
  if (ipardebug)
    {
      short *ssp1 = iparss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != iparssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

iparerrhandle:

  iparn = iparpact[iparstate];
  if (iparn == YYFLAG)
    goto iparerrdefault;

  iparn += YYTERROR;
  if (iparn < 0 || iparn > YYLAST || iparcheck[iparn] != YYTERROR)
    goto iparerrdefault;

  iparn = ipartable[iparn];
  if (iparn < 0)
    {
      if (iparn == YYFLAG)
	goto iparerrpop;
      iparn = -iparn;
      goto iparreduce;
    }
  else if (iparn == 0)
    goto iparerrpop;

  if (iparn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (ipardebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++iparvsp = iparlval;
#ifdef YYLSP_NEEDED
  *++iparlsp = iparlloc;
#endif

  iparstate = iparn;
  goto iparnewstate;
}
#line 156 "ipar.y"



extern char *iparinchar;

ParamList *IRAFParseStr(str)
			char	*str;
{
		static parser = 0;

/*
		extern ipardebug;

	ipardebug = 1;
 */

	result = NULL;

	iparlineno = 1;

	if ( parser ) {
		iparreparse();
		iparrestart(str);
		iparinchar = str;
	} else {
		iparinchar = str;
		parser = 1;
	}

	if ( iparparse() ) {
	  PFree(result);
	  return(NULL);
	}

	return result;
}

ParamList *IRAFParFile(f)
			FILE	*f;
{
		ParamList	*plist;
		int		 size;
		char		*text;


        if ( size = filesize(f) ) {
                text = malloc(size + 1);
                if ( fread(text, 1, size, f) != size ) {
			free(text);
			return NULL;
                }
 
                text[size] = '\0';
        }

	plist = IRAFParseStr(text);
	free(text);

	return plist;
}

typedef struct stat FileInfo;
 
#define FileStat(f, info)       fstat(fileno(f), info)
#define FileInfoSize(info)      info.st_size

static int filesize(file)
                        FILE    *file;
{
        FileInfo info;
 
        if ( FileStat(file, &info) < 0 )        return 0;
        else                                    return FileInfoSize(info);
}


iparerror()
{
	extern int iparfield;
	static char	*fields[] = {
		"Name", "Type", "Mode", "Value", "Min", "Max", "Prompt"
	};

	if ( iparfield > 6 )
	  fprintf(stderr, "Error in parameter file: line %d: too many fields\n"
		 , iparlineno);
	else
	  fprintf(stderr, "Error in parameter file: line %d: field %s\n"
		 , iparlineno
		 , fields[iparfield]);
}
