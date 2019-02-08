
/*  A Bison parser, made from iarg.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_valu	258
#define	T_name	259
#define	T_mark	260
#define	T_posi	261

#line 2 "iarg.y"


#include <pargs.h>
#include "pfile.h"

extern Parameter *ParamPositional();

static ParamList *result;
static ParamFile  reference;
static int	  position;
int t_valu;

#define MODE 3

#define YYINITDEPTH 256
#define YYMAXDEPTH  257

#define YYDEBUG 1

/* This is the bit eric told me about.
 */
static char nstr[1];

#line 31 "iarg.y"
typedef union {
	ParamList	*plist;
	char		*stri;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		14
#define	YYFLAG		-32768
#define	YYNTBASE	8

#define YYTRANSLATE(x) ((unsigned)(x) <= 261 ? iargtranslate[x] : 10)

static const char iargtranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     7,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
     6
};

#if YYDEBUG != 0
static const short iargprhs[] = {     0,
     0,     2,     4,     7,    10,    14,    17,    21,    25
};

static const short iargrhs[] = {     9,
     0,     6,     0,     9,     6,     0,     4,     7,     0,     9,
     4,     7,     0,     4,     5,     0,     9,     4,     5,     0,
     4,     7,     3,     0,     9,     4,     7,     3,     0
};

#endif

#if YYDEBUG != 0
static const short iargrline[] = { 0,
    42,    47,    59,    70,    78,    86,    94,   102,   111
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const iargtname[] = {   "$","error","$undefined.","T_valu",
"T_name","T_mark","T_posi","'='","pfile","plist", NULL
};
#endif

static const short iargr1[] = {     0,
     8,     9,     9,     9,     9,     9,     9,     9,     9
};

static const short iargr2[] = {     0,
     1,     1,     2,     2,     3,     2,     3,     3,     4
};

static const short iargdefact[] = {     0,
     0,     2,     1,     6,     4,     0,     3,     8,     7,     5,
     9,     0,     0,     0
};

static const short iargdefgoto[] = {    12,
     3
};

static const short iargpact[] = {    -4,
    -2,-32768,     0,-32768,     5,     2,-32768,-32768,-32768,     7,
-32768,     1,    11,-32768
};

static const short iargpgoto[] = {-32768,
-32768
};


#define	YYLAST		11


static const short iargtable[] = {     1,
    13,     2,     4,     6,     5,     7,     9,     8,    10,    11,
    14
};

static const short iargcheck[] = {     4,
     0,     6,     5,     4,     7,     6,     5,     3,     7,     3,
     0
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

#define iargerrok		(iargerrstatus = 0)
#define iargclearin	(iargchar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto iargerrlab1
/* Like YYERROR except do call iargerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto iargerrlab
#define YYRECOVERING()  (!!iargerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (iargchar == YYEMPTY && iarglen == 1)				\
    { iargchar = (token), iarglval = (value);			\
      iargchar1 = YYTRANSLATE (iargchar);				\
      YYPOPSTACK;						\
      goto iargbackup;						\
    }								\
  else								\
    { iargerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		iarglex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		iarglex(&iarglval, &iarglloc, YYLEX_PARAM)
#else
#define YYLEX		iarglex(&iarglval, &iarglloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		iarglex(&iarglval, YYLEX_PARAM)
#else
#define YYLEX		iarglex(&iarglval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	iargchar;			/*  the lookahead symbol		*/
YYSTYPE	iarglval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE iarglloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int iargnerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int iargdebug;			/*  nonzero means print parse trace	*/
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
int iargparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __iarg_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__iarg_memcpy (to, from, count)
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
__iarg_memcpy (char *to, char *from, int count)
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
   into iargparse.  The argument should have type void *.
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
iargparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int iargstate;
  register int iargn;
  register short *iargssp;
  register YYSTYPE *iargvsp;
  int iargerrstatus;	/*  number of tokens to shift before error messages enabled */
  int iargchar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	iargssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE iargvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *iargss = iargssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *iargvs = iargvsa;	/*  to allow iargoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE iarglsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *iargls = iarglsa;
  YYLTYPE *iarglsp;

#define YYPOPSTACK   (iargvsp--, iargssp--, iarglsp--)
#else
#define YYPOPSTACK   (iargvsp--, iargssp--)
#endif

  int iargstacksize = YYINITDEPTH;

#ifdef YYPURE
  int iargchar;
  YYSTYPE iarglval;
  int iargnerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE iarglloc;
#endif
#endif

  YYSTYPE iargval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int iarglen;

#if YYDEBUG != 0
  if (iargdebug)
    fprintf(stderr, "Starting parse\n");
#endif

  iargstate = 0;
  iargerrstatus = 0;
  iargnerrs = 0;
  iargchar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  iargssp = iargss - 1;
  iargvsp = iargvs;
#ifdef YYLSP_NEEDED
  iarglsp = iargls;
#endif

/* Push a new state, which is found in  iargstate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
iargnewstate:

  *++iargssp = iargstate;

  if (iargssp >= iargss + iargstacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *iargvs1 = iargvs;
      short *iargss1 = iargss;
#ifdef YYLSP_NEEDED
      YYLTYPE *iargls1 = iargls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = iargssp - iargss + 1;

#ifdef iargoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if iargoverflow is a macro.  */
      iargoverflow("parser stack overflow",
		 &iargss1, size * sizeof (*iargssp),
		 &iargvs1, size * sizeof (*iargvsp),
		 &iargls1, size * sizeof (*iarglsp),
		 &iargstacksize);
#else
      iargoverflow("parser stack overflow",
		 &iargss1, size * sizeof (*iargssp),
		 &iargvs1, size * sizeof (*iargvsp),
		 &iargstacksize);
#endif

      iargss = iargss1; iargvs = iargvs1;
#ifdef YYLSP_NEEDED
      iargls = iargls1;
#endif
#else /* no iargoverflow */
      /* Extend the stack our own way.  */
      if (iargstacksize >= YYMAXDEPTH)
	{
	  iargerror("parser stack overflow");
	  return 2;
	}
      iargstacksize *= 2;
      if (iargstacksize > YYMAXDEPTH)
	iargstacksize = YYMAXDEPTH;
      iargss = (short *) alloca (iargstacksize * sizeof (*iargssp));
      __iarg_memcpy ((char *)iargss, (char *)iargss1, size * sizeof (*iargssp));
      iargvs = (YYSTYPE *) alloca (iargstacksize * sizeof (*iargvsp));
      __iarg_memcpy ((char *)iargvs, (char *)iargvs1, size * sizeof (*iargvsp));
#ifdef YYLSP_NEEDED
      iargls = (YYLTYPE *) alloca (iargstacksize * sizeof (*iarglsp));
      __iarg_memcpy ((char *)iargls, (char *)iargls1, size * sizeof (*iarglsp));
#endif
#endif /* no iargoverflow */

      iargssp = iargss + size - 1;
      iargvsp = iargvs + size - 1;
#ifdef YYLSP_NEEDED
      iarglsp = iargls + size - 1;
#endif

#if YYDEBUG != 0
      if (iargdebug)
	fprintf(stderr, "Stack size increased to %d\n", iargstacksize);
#endif

      if (iargssp >= iargss + iargstacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (iargdebug)
    fprintf(stderr, "Entering state %d\n", iargstate);
#endif

  goto iargbackup;
 iargbackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* iargresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  iargn = iargpact[iargstate];
  if (iargn == YYFLAG)
    goto iargdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* iargchar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (iargchar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (iargdebug)
	fprintf(stderr, "Reading a token: ");
#endif
      iargchar = YYLEX;
    }

  /* Convert token to internal form (in iargchar1) for indexing tables with */

  if (iargchar <= 0)		/* This means end of input. */
    {
      iargchar1 = 0;
      iargchar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (iargdebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      iargchar1 = YYTRANSLATE(iargchar);

#if YYDEBUG != 0
      if (iargdebug)
	{
	  fprintf (stderr, "Next token is %d (%s", iargchar, iargtname[iargchar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, iargchar, iarglval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  iargn += iargchar1;
  if (iargn < 0 || iargn > YYLAST || iargcheck[iargn] != iargchar1)
    goto iargdefault;

  iargn = iargtable[iargn];

  /* iargn is what to do for this token type in this state.
     Negative => reduce, -iargn is rule number.
     Positive => shift, iargn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (iargn < 0)
    {
      if (iargn == YYFLAG)
	goto iargerrlab;
      iargn = -iargn;
      goto iargreduce;
    }
  else if (iargn == 0)
    goto iargerrlab;

  if (iargn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (iargdebug)
    fprintf(stderr, "Shifting token %d (%s), ", iargchar, iargtname[iargchar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (iargchar != YYEOF)
    iargchar = YYEMPTY;

  *++iargvsp = iarglval;
#ifdef YYLSP_NEEDED
  *++iarglsp = iarglloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (iargerrstatus) iargerrstatus--;

  iargstate = iargn;
  goto iargnewstate;

/* Do the default action for the current state.  */
iargdefault:

  iargn = iargdefact[iargstate];
  if (iargn == 0)
    goto iargerrlab;

/* Do a reduction.  iargn is the number of a rule to reduce with.  */
iargreduce:
  iarglen = iargr2[iargn];
  if (iarglen > 0)
    iargval = iargvsp[1-iarglen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (iargdebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       iargn, iargrline[iargn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = iargprhs[iargn]; iargrhs[i] > 0; i++)
	fprintf (stderr, "%s ", iargtname[iargrhs[i]]);
      fprintf (stderr, " -> %s\n", iargtname[iargr1[iargn]]);
    }
#endif


  switch (iargn) {

case 1:
#line 43 "iarg.y"
{ result = iargval.plist	;
    break;}
case 2:
#line 48 "iarg.y"
{ 
		  if ( reference == NULL ) {
			parerr = PARNOREFR;
			return 1;
		  }

		  iargval.plist = PListAdd(NULL
		   , ParamPositional(reference->psets, position++, iargvsp[0].stri));

		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 3:
#line 60 "iarg.y"
{ 
		  if ( reference == NULL ) {
			parerr = PARNOREFR;
			return 1;
		  }

		  iargval.plist = PListAdd(iargvsp[-1].plist
		   , ParamPositional(reference->psets, position++, iargvsp[0].stri));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 4:
#line 71 "iarg.y"
{ 		int mode;
		  mode = QMODE;
		  iargval.plist = PListAdd(NULL
		   , ParamCreate(ParamMatch(reference, iargvsp[-1].stri)
				 , StringType, &mode, NewString(nstr), NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 5:
#line 79 "iarg.y"
{  		int mode;
		  mode = QMODE;
		  iargval.plist = PListAdd(iargvsp[-2].plist
		   , ParamCreate(ParamMatch(reference, iargvsp[-1].stri)
				 , StringType, &mode, NewString(nstr), NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 6:
#line 87 "iarg.y"
{  		int mode;
		  mode = LMODE;
		  iargval.plist = PListAdd(NULL
		   , ParamCreate(ParamMatch(reference, iargvsp[-1].stri)
				 , StringType, &mode, iargvsp[0].stri, NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 7:
#line 95 "iarg.y"
{  		int mode;
		  mode = LMODE;
		  iargval.plist = PListAdd(iargvsp[-2].plist
		   , ParamCreate(ParamMatch(reference, iargvsp[-1].stri)
				 , StringType, &mode, iargvsp[0].stri, NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 8:
#line 103 "iarg.y"
{  		int mode;
		  mode = LMODE;

		  iargval.plist = PListAdd(NULL
		   , ParamCreate(ParamMatch(reference, iargvsp[-2].stri)
				 , StringType, &mode, iargvsp[0].stri, NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
case 9:
#line 112 "iarg.y"
{  		int mode;
		  mode = LMODE;
		  iargval.plist = PListAdd(iargvsp[-3].plist
				, ParamCreate(ParamMatch(reference, iargvsp[-2].stri)
				  , StringType, &mode, iargvsp[0].stri, NULL));
		  if ( iargval.plist == NULL ) return 1
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/home/john/lib/bison.simple"

  iargvsp -= iarglen;
  iargssp -= iarglen;
#ifdef YYLSP_NEEDED
  iarglsp -= iarglen;
#endif

#if YYDEBUG != 0
  if (iargdebug)
    {
      short *ssp1 = iargss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != iargssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++iargvsp = iargval;

#ifdef YYLSP_NEEDED
  iarglsp++;
  if (iarglen == 0)
    {
      iarglsp->first_line = iarglloc.first_line;
      iarglsp->first_column = iarglloc.first_column;
      iarglsp->last_line = (iarglsp-1)->last_line;
      iarglsp->last_column = (iarglsp-1)->last_column;
      iarglsp->text = 0;
    }
  else
    {
      iarglsp->last_line = (iarglsp+iarglen-1)->last_line;
      iarglsp->last_column = (iarglsp+iarglen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  iargn = iargr1[iargn];

  iargstate = iargpgoto[iargn - YYNTBASE] + *iargssp;
  if (iargstate >= 0 && iargstate <= YYLAST && iargcheck[iargstate] == *iargssp)
    iargstate = iargtable[iargstate];
  else
    iargstate = iargdefgoto[iargn - YYNTBASE];

  goto iargnewstate;

iargerrlab:   /* here on detecting error */

  if (! iargerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++iargnerrs;

#ifdef YYERROR_VERBOSE
      iargn = iargpact[iargstate];

      if (iargn > YYFLAG && iargn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -iargn if nec to avoid negative indexes in iargcheck.  */
	  for (x = (iargn < 0 ? -iargn : 0);
	       x < (sizeof(iargtname) / sizeof(char *)); x++)
	    if (iargcheck[x + iargn] == x)
	      size += strlen(iargtname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (iargn < 0 ? -iargn : 0);
		       x < (sizeof(iargtname) / sizeof(char *)); x++)
		    if (iargcheck[x + iargn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, iargtname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      iargerror(msg);
	      free(msg);
	    }
	  else
	    iargerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	iargerror("parse error");
    }

  goto iargerrlab1;
iargerrlab1:   /* here on error raised explicitly by an action */

  if (iargerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (iargchar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (iargdebug)
	fprintf(stderr, "Discarding token %d (%s).\n", iargchar, iargtname[iargchar1]);
#endif

      iargchar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  iargerrstatus = 3;		/* Each real token shifted decrements this */

  goto iargerrhandle;

iargerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  iargn = iargdefact[iargstate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (iargn) goto iargdefault;
#endif

iargerrpop:   /* pop the current state because it cannot handle the error token */

  if (iargssp == iargss) YYABORT;
  iargvsp--;
  iargstate = *--iargssp;
#ifdef YYLSP_NEEDED
  iarglsp--;
#endif

#if YYDEBUG != 0
  if (iargdebug)
    {
      short *ssp1 = iargss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != iargssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

iargerrhandle:

  iargn = iargpact[iargstate];
  if (iargn == YYFLAG)
    goto iargerrdefault;

  iargn += YYTERROR;
  if (iargn < 0 || iargn > YYLAST || iargcheck[iargn] != YYTERROR)
    goto iargerrdefault;

  iargn = iargtable[iargn];
  if (iargn < 0)
    {
      if (iargn == YYFLAG)
	goto iargerrpop;
      iargn = -iargn;
      goto iargreduce;
    }
  else if (iargn == 0)
    goto iargerrpop;

  if (iargn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (iargdebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++iargvsp = iarglval;
#ifdef YYLSP_NEEDED
  *++iarglsp = iarglloc;
#endif

  iargstate = iargn;
  goto iargnewstate;
}
#line 120 "iarg.y"


ParamList *IRAFParArgs(pfile, argv, argc)
			ParamFile	 pfile;
			int	 	 argc;
			char		*argv[];
{
		extern FILE *iargin;
		static int parser = 0;

/*
		extern iargdebug;
	iargdebug = 1;
 */

	*nstr = '\0';

	result	= NULL;
	reference	= pfile;
	position	= 1;
	t_valu		= T_posi;

	if ( parser )
		iargrestart(OpenArgv(0, '\001', argv, argc));
	else {
		iargin = (FILE *) OpenArgv(0, '\001', argv, argc);
		parser = 1;
	}


	if ( iargparse() ) {
		PFree(result);
		result = NULL;
	} else {
	    result->file     = NULL;
	    result->filename = NULL;
	    result->psetname = NULL;
	}

	CloseArgv(iargin);


	return result;		
}

iargerror()
{
	fprintf(stderr, "Sorry, Arguments not ok, try again.\n");
}
