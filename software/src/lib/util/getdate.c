/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     tAGO = 258,
     tDAY = 259,
     tDAYZONE = 260,
     tID = 261,
     tMERIDIAN = 262,
     tMINUTE_UNIT = 263,
     tMONTH = 264,
     tMONTH_UNIT = 265,
     tSEC_UNIT = 266,
     tSNUMBER = 267,
     tUNUMBER = 268,
     tFNUMBER = 269,
     tZONE = 270,
     tDST = 271,
     tT = 272
   };
#endif
#define tAGO 258
#define tDAY 259
#define tDAYZONE 260
#define tID 261
#define tMERIDIAN 262
#define tMINUTE_UNIT 263
#define tMONTH 264
#define tMONTH_UNIT 265
#define tSEC_UNIT 266
#define tSNUMBER 267
#define tUNUMBER 268
#define tFNUMBER 269
#define tZONE 270
#define tDST 271
#define tT 272




/* Copy the first part of user declarations.  */
#line 1 "getdate.y"

/* $Revision: 1.15 $
**
**  Originally written by Steven M. Bellovin <smb@research.att.com> while
**  at the University of North Carolina at Chapel Hill.  Later tweaked by
**  a couple of people on Usenet.  Completely overhauled by Rich $alz
**  <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990;
**  send any email to Rich.
**
**  This grammar has eight shift/reduce conflicts.
**
**  This code is in the public domain and has no copyright.
*/
/* SUPPRESS 287 on yaccpar_sccsid *//* Unusd static variable */
/* SUPPRESS 288 on yyerrlab *//* Label unused */

struct tm *breakmjd();

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __GNUC__
#define alloca __builtin_alloca
#else
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#else
#ifdef _AIX /* for Bison */
 #pragma alloca
#else
#define yyoverflow	dateoverflow

static int dateoverflow() {
	return 1;
}

char *alloca ();
#endif
#endif
#endif


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void   caldate();
double juldate();

/* The code at the top of get_date which figures out the offset of the
   current time zone checks various CPP symbols to see if special
   tricks are need, but defaults to using the gettimeofday system call.
   Include <sys/time.h> if that will be used.  */

#if !defined (USG) && !defined (sgi) && !defined (__FreeBSD__)
#include <sys/time.h>
#endif

#if	defined(vms)

#include <types.h>
#include <time.h>

#else

#include <sys/types.h>

#if	defined(BSD4_2) || defined(BSD4_1C) || (defined (hp9000) && !defined (hpux))
#include <sys/time.h>
#else
#if defined(_AIX)
#include <sys/time.h>
#endif
#include <time.h>
#endif	/* defined(BSD4_2) */
#endif	/* defined(vms) */

#if defined (STDC_HEADERS) || defined (USG)
#include <string.h>
#endif

#if sgi
#undef timezone
#endif

#define yyparse getdate_yyparse
#define yylex   getdate_yylex
#define yyerror getdate_yyerror
#define yylval  getdate_yylval

#define HOUR(x)		((int)(x) * 60)
#define SECSPERDAY	(24L * 60L * 60L)

#define MJD1970			  40587
#define MJD2038			  65424

#define MJDtoUNIXTIME(mjd)	((int   ) (((mjd - MJD1970) * SECSPERDAY)))
#define UNIXTIMEtoMJD(unx)	((double)unx/SECSPERDAY + MJD1970)


/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    char	*name;
    int		type;
    time_t	value;
} TABLE;


/*
**  Daylight-savings mode:  on, off, or not yet known.
*/
typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;

/*
**  Meridian:  am, pm, or 24-hour style.
*/
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;


/*
**  Global variables.  We could get rid of most of these by using a good
**  union as the yacc stack.  (This routine was originally written before
**  yacc had the %union construct.)  Maybe someday; right now we only use
**  the %union very rarely.
*/
static char	*yyInput;
static DSTMODE	yyDSTmode;
static time_t	yyDayOrdinal;
static time_t	yyDayNumber;
static int	yyHaveDate;
static int	yyHaveMJD;
static int	yyHaveDay;
static int	yyHaveRel;
static int	yyHaveTime;
static int	yyHaveZone;
static double	yyMJD;
static int	yyNow;
static time_t	yyTimezone;
static time_t	yyDay;
static time_t	yyHour;
static time_t	yyMinutes;
static time_t	yyMonth;
static double	yySeconds;
static time_t	yyYear;
static MERIDIAN	yyMeridian;
static time_t	yyRelMonth;
static time_t	yyRelSeconds;

static int FitsDate = 1;

static int getdate_yyerror();
static int getdate_yylex();


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 162 "getdate.y"
typedef union YYSTYPE {
    double		Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 276 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 288 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   57

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  13
/* YYNRULES -- Number of rules. */
#define YYNRULES  48
/* YYNRULES -- Number of states. */
#define YYNSTATES  67

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   272

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    19,    21,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    24,    29,    34,    41,    49,    56,    58,    60,
      62,    64,    66,    69,    71,    74,    77,    81,    87,    91,
      94,    99,   103,   108,   111,   115,   118,   120,   123,   126,
     128,   131,   134,   136,   139,   142,   144,   146,   147
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      23,     0,    -1,    -1,    23,    24,    -1,    25,    -1,    27,
      -1,    28,    -1,    30,    -1,    29,    -1,    31,    -1,    33,
      -1,    13,     7,    -1,    13,    18,    13,    34,    -1,    13,
      18,    13,    12,    -1,    13,    18,    13,    18,    26,    34,
      -1,    17,    13,    18,    13,    18,    26,    34,    -1,    13,
      18,    13,    18,    26,    12,    -1,    13,    -1,    14,    -1,
      14,    -1,    15,    -1,     5,    -1,    15,    16,    -1,     4,
      -1,     4,    19,    -1,    13,     4,    -1,    13,    20,    13,
      -1,    13,    20,    13,    20,    13,    -1,    13,    12,    12,
      -1,     9,    13,    -1,    13,    21,     9,    12,    -1,     9,
      12,    12,    -1,     9,    13,    19,    13,    -1,    13,     9,
      -1,    13,     9,    13,    -1,    32,     3,    -1,    32,    -1,
      13,     8,    -1,    12,     8,    -1,     8,    -1,    12,    11,
      -1,    13,    11,    -1,    11,    -1,    12,    10,    -1,    13,
      10,    -1,    10,    -1,    13,    -1,    -1,     7,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   176,   176,   177,   180,   183,   184,   187,   190,   193,
     196,   199,   205,   211,   219,   227,   235,   247,   250,   256,
     264,   268,   273,   279,   283,   287,   293,   297,   307,   313,
     317,   322,   327,   332,   336,   343,   347,   350,   353,   356,
     359,   362,   365,   368,   371,   374,   379,   413,   416
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tAGO", "tDAY", "tDAYZONE", "tID",
  "tMERIDIAN", "tMINUTE_UNIT", "tMONTH", "tMONTH_UNIT", "tSEC_UNIT",
  "tSNUMBER", "tUNUMBER", "tFNUMBER", "tZONE", "tDST", "tT", "':'", "','",
  "'/'", "'-'", "$accept", "spec", "item", "time", "UorFNumber", "mjd",
  "zone", "day", "date", "rel", "relunit", "number", "o_merid", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,    58,    44,
      47,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    22,    23,    23,    24,    24,    24,    24,    24,    24,
      24,    25,    25,    25,    25,    25,    25,    26,    26,    27,
      28,    28,    28,    29,    29,    29,    30,    30,    30,    30,
      30,    30,    30,    30,    30,    31,    31,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    33,    34,    34
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     2,     4,     4,     6,     7,     6,     1,     1,     1,
       1,     1,     2,     1,     2,     2,     3,     5,     3,     2,
       4,     3,     4,     2,     3,     2,     1,     2,     2,     1,
       2,     2,     1,     2,     2,     1,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,    23,    21,    39,     0,    45,    42,     0,
      46,    19,    20,     0,     3,     4,     5,     6,     8,     7,
       9,    36,    10,    24,     0,    29,    38,    43,    40,    25,
      11,    37,    33,    44,    41,     0,     0,     0,     0,    22,
       0,    35,    31,     0,    34,    28,    47,    26,     0,     0,
      32,    48,    13,     0,    12,     0,    30,     0,    17,    18,
      47,    27,     0,    16,    14,    47,    15
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,    14,    15,    60,    16,    17,    18,    19,    20,
      21,    22,    54
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -59
static const yysigned_char yypact[] =
{
     -59,     0,   -59,   -16,   -59,   -59,    22,   -59,   -59,    17,
      12,   -59,     2,    16,   -59,   -59,   -59,   -59,   -59,   -59,
     -59,    33,   -59,   -59,    28,    23,   -59,   -59,   -59,   -59,
     -59,   -59,    30,   -59,   -59,    29,    31,    32,    37,   -59,
      34,   -59,   -59,    35,   -59,   -59,    19,    27,    38,    36,
     -59,   -59,   -59,    25,   -59,    40,   -59,    39,   -59,   -59,
      -6,   -59,    25,   -59,   -59,    44,   -59
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -59,   -59,   -59,   -59,    -8,   -59,   -59,   -59,   -59,   -59,
     -59,   -59,   -58
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
       2,    51,    64,    23,     3,     4,    63,    66,     5,     6,
       7,     8,     9,    10,    11,    12,    29,    13,    39,    30,
      31,    32,    33,    34,    35,    26,    51,    27,    28,    40,
      36,    52,    37,    38,    24,    25,    41,    53,    58,    59,
      42,    45,    43,    44,    46,    47,    48,    55,    50,    57,
      56,    51,    49,    61,    65,     0,     0,    62
};

static const yysigned_char yycheck[] =
{
       0,     7,    60,    19,     4,     5,    12,    65,     8,     9,
      10,    11,    12,    13,    14,    15,     4,    17,    16,     7,
       8,     9,    10,    11,    12,     8,     7,    10,    11,    13,
      18,    12,    20,    21,    12,    13,     3,    18,    13,    14,
      12,    12,    19,    13,    13,    13,     9,    20,    13,    13,
      12,     7,    18,    13,    62,    -1,    -1,    18
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    23,     0,     4,     5,     8,     9,    10,    11,    12,
      13,    14,    15,    17,    24,    25,    27,    28,    29,    30,
      31,    32,    33,    19,    12,    13,     8,    10,    11,     4,
       7,     8,     9,    10,    11,    12,    18,    20,    21,    16,
      13,     3,    12,    19,    13,    12,    13,    13,     9,    18,
      13,     7,    12,    18,    34,    20,    12,    13,    13,    14,
      26,    13,    18,    12,    34,    26,    34
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 180 "getdate.y"
    {
	    yyHaveTime++;
	}
    break;

  case 5:
#line 183 "getdate.y"
    { }
    break;

  case 6:
#line 184 "getdate.y"
    {
	    yyHaveZone++;
	}
    break;

  case 7:
#line 187 "getdate.y"
    {
	    yyHaveDate++;
	}
    break;

  case 8:
#line 190 "getdate.y"
    {
	    yyHaveDay++;
	}
    break;

  case 9:
#line 193 "getdate.y"
    {
	    yyHaveRel++;
	}
    break;

  case 11:
#line 199 "getdate.y"
    {
	    yyHour = yyvsp[-1].Number;
	    yyMinutes = 0;
	    yySeconds = 0;
	    yyMeridian = yyvsp[0].Meridian;
	}
    break;

  case 12:
#line 205 "getdate.y"
    {
	    yyHour = yyvsp[-3].Number;
	    yyMinutes = yyvsp[-1].Number;
	    yySeconds = 0;
	    yyMeridian = yyvsp[0].Meridian;
	}
    break;

  case 13:
#line 211 "getdate.y"
    {
	    yyHour = yyvsp[-3].Number;
	    yyMinutes = yyvsp[-1].Number;
	    yySeconds = 0;
	    yyMeridian = MER24;
	    yyDSTmode = DSToff;
	    yyTimezone = - ((int)yyvsp[0].Number % 100 + ((int)yyvsp[0].Number / 100) * 60);
	}
    break;

  case 14:
#line 219 "getdate.y"
    {
		double	f;

	    yyHour = yyvsp[-5].Number;
	    yyMinutes = yyvsp[-3].Number;
	    yySeconds = yyvsp[-1].Number;
	    yyMeridian = yyvsp[0].Meridian;
	}
    break;

  case 15:
#line 227 "getdate.y"
    {
		double	f;

	    yyHour = yyvsp[-5].Number;
	    yyMinutes = yyvsp[-3].Number;
	    yySeconds = yyvsp[-1].Number;
	    yyMeridian = yyvsp[0].Meridian;
	}
    break;

  case 16:
#line 235 "getdate.y"
    {
		double	f;

	    yyHour = yyvsp[-5].Number;
	    yyMinutes = yyvsp[-3].Number;
	    yySeconds = yyvsp[-1].Number;
	    yyMeridian = MER24;
	    yyDSTmode = DSToff;
	    yyTimezone = - ((int)yyvsp[0].Number % 100 + ((int)yyvsp[0].Number / 100) * 60);
	}
    break;

  case 17:
#line 247 "getdate.y"
    {
	    yyval.Number = yyvsp[0].Number;
	}
    break;

  case 18:
#line 250 "getdate.y"
    {
	    yyval.Number = yyvsp[0].Number;
	}
    break;

  case 19:
#line 256 "getdate.y"
    {
	    if ( yyvsp[0].Number > 5000 && yyvsp[0].Number < 100000 ) {
		yyMJD = yyvsp[0].Number;
		yyHaveMJD++;
	    }
	}
    break;

  case 20:
#line 264 "getdate.y"
    {
	    yyTimezone = yyvsp[0].Number;
	    yyDSTmode = DSToff;
	}
    break;

  case 21:
#line 268 "getdate.y"
    {
	    yyTimezone = yyvsp[0].Number;
	    yyDSTmode = DSTon;
	}
    break;

  case 22:
#line 273 "getdate.y"
    {
	    yyTimezone = yyvsp[-1].Number;
	    yyDSTmode = DSTon;
	}
    break;

  case 23:
#line 279 "getdate.y"
    {
	    yyDayOrdinal = 1;
	    yyDayNumber = yyvsp[0].Number;
	}
    break;

  case 24:
#line 283 "getdate.y"
    {
	    yyDayOrdinal = 1;
	    yyDayNumber = yyvsp[-1].Number;
	}
    break;

  case 25:
#line 287 "getdate.y"
    {
	    yyDayOrdinal = yyvsp[-1].Number;
	    yyDayNumber = yyvsp[0].Number;
	}
    break;

  case 26:
#line 293 "getdate.y"
    {
	    yyMonth = yyvsp[-2].Number;
	    yyDay = yyvsp[0].Number;
	}
    break;

  case 27:
#line 297 "getdate.y"
    {
	    if ( FitsDate ) {
		    yyDay = yyvsp[-4].Number;
		    yyMonth = yyvsp[-2].Number;
	    } else {
		    yyMonth = yyvsp[-4].Number;
		    yyDay = yyvsp[-2].Number;
	    }
	    yyYear = yyvsp[0].Number;
	}
    break;

  case 28:
#line 307 "getdate.y"
    {
	    /* ISO 8601 format.  yyyy-mm-dd.  */
	    yyYear = yyvsp[-2].Number;
	    yyMonth = -yyvsp[-1].Number;
	    yyDay = -yyvsp[0].Number;
	}
    break;

  case 29:
#line 313 "getdate.y"
    {
	    yyMonth = yyvsp[-1].Number;
	    yyDay = yyvsp[0].Number;
	}
    break;

  case 30:
#line 317 "getdate.y"
    {
	    yyDay   =  yyvsp[-3].Number;
	    yyMonth =  yyvsp[-1].Number;
	    yyYear  = -yyvsp[0].Number;
	}
    break;

  case 31:
#line 322 "getdate.y"
    {
	    yyMonth =  yyvsp[-2].Number;
	    yyDay   = -yyvsp[-1].Number;
	    yyYear  = -yyvsp[0].Number;
	}
    break;

  case 32:
#line 327 "getdate.y"
    {
	    yyMonth = yyvsp[-3].Number;
	    yyDay = yyvsp[-2].Number;
	    yyYear = yyvsp[0].Number;
	}
    break;

  case 33:
#line 332 "getdate.y"
    {
	    yyMonth = yyvsp[0].Number;
	    yyDay = yyvsp[-1].Number;
	}
    break;

  case 34:
#line 336 "getdate.y"
    {
	    yyDay = yyvsp[-2].Number;
	    yyMonth = yyvsp[-1].Number;
	    yyYear = yyvsp[0].Number;
	}
    break;

  case 35:
#line 343 "getdate.y"
    {
	    yyRelSeconds = -yyRelSeconds;
	    yyRelMonth = -yyRelMonth;
	}
    break;

  case 37:
#line 350 "getdate.y"
    {
	    yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number * 60L;
	}
    break;

  case 38:
#line 353 "getdate.y"
    {
	    yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number * 60L;
	}
    break;

  case 39:
#line 356 "getdate.y"
    {
	    yyRelSeconds += yyvsp[0].Number * 60L;
	}
    break;

  case 40:
#line 359 "getdate.y"
    {
	    yyRelSeconds += yyvsp[-1].Number;
	}
    break;

  case 41:
#line 362 "getdate.y"
    {
	    yyRelSeconds += yyvsp[-1].Number;
	}
    break;

  case 42:
#line 365 "getdate.y"
    {
	    yyRelSeconds++;
	}
    break;

  case 43:
#line 368 "getdate.y"
    {
	    yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	}
    break;

  case 44:
#line 371 "getdate.y"
    {
	    yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	}
    break;

  case 45:
#line 374 "getdate.y"
    {
	    yyRelMonth += yyvsp[0].Number;
	}
    break;

  case 46:
#line 379 "getdate.y"
    {
	    if (yyHaveTime && yyHaveDate && !yyHaveRel)
		yyYear = yyvsp[0].Number;
	    else {
		if( yyvsp[0].Number > 5000 && yyvsp[0].Number < 100000 ) {
		    yyHaveMJD++;
		    yyMJD = yyvsp[0].Number;
		} else 
		if( yyvsp[0].Number > 30000000 ) {
		    yyHaveMJD++;
		    yyMJD = UNIXTIMEtoMJD(yyvsp[0].Number);
		} else 
		if(yyvsp[0].Number>100000) {
		    yyHaveDate++;
		    yyDay= ((int)yyvsp[0].Number)%100;
		    yyMonth= ((int)yyvsp[0].Number/100)%100;
	    	    yyYear = yyvsp[0].Number/10000;
		} else {
		    yyHaveTime++;
		    if (yyvsp[0].Number < 100) {
			yyHour = yyvsp[0].Number;
			yyMinutes = 0;
		    }
		    else {
			yyHour = yyvsp[0].Number / 100;
			yyMinutes = (int)yyvsp[0].Number % 100;
		    }
		}
		yySeconds = 0;
		yyMeridian = MER24;
	    }
	}
    break;

  case 47:
#line 413 "getdate.y"
    {
	    yyval.Meridian = MER24;
	}
    break;

  case 48:
#line 416 "getdate.y"
    {
	    yyval.Meridian = yyvsp[0].Meridian;
	}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1610 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 421 "getdate.y"


/* Month and day table. */
static TABLE const MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "sept",		tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 1 },
    { "tuesday",	tDAY, 2 },
    { "tues",		tDAY, 2 },
    { "wednesday",	tDAY, 3 },
    { "wednes",		tDAY, 3 },
    { "thursday",	tDAY, 4 },
    { "thur",		tDAY, 4 },
    { "thurs",		tDAY, 4 },
    { "friday",		tDAY, 5 },
    { "saturday",	tDAY, 6 },
    { NULL }
};

/* Time units table. */
static TABLE const UnitsTable[] = {
    { "year",		tMONTH_UNIT,	12 },
    { "month",		tMONTH_UNIT,	1 },
    { "fortnight",	tMINUTE_UNIT,	14 * 24 * 60 },
    { "week",		tMINUTE_UNIT,	7 * 24 * 60 },
    { "day",		tMINUTE_UNIT,	1 * 24 * 60 },
    { "hour",		tMINUTE_UNIT,	60 },
    { "minute",		tMINUTE_UNIT,	1 },
    { "min",		tMINUTE_UNIT,	1 },
    { "second",		tSEC_UNIT,	1 },
    { "sec",		tSEC_UNIT,	1 },
    { NULL }
};

/* Assorted relative-time words. */
static TABLE const OtherTable[] = {
    { "tomorrow",	tMINUTE_UNIT,	1 * 24 * 60 },
    { "yesterday",	tMINUTE_UNIT,	-1 * 24 * 60 },
    { "today",		tMINUTE_UNIT,	0 },
    { "now",		tMINUTE_UNIT,	0 },
    { "last",		tUNUMBER,	-1 },
    { "this",		tMINUTE_UNIT,	0 },
    { "next",		tUNUMBER,	2 },
    { "first",		tUNUMBER,	1 },
/*  { "second",		tUNUMBER,	2 }, */
    { "third",		tUNUMBER,	3 },
    { "fourth",		tUNUMBER,	4 },
    { "fifth",		tUNUMBER,	5 },
    { "sixth",		tUNUMBER,	6 },
    { "seventh",	tUNUMBER,	7 },
    { "eighth",		tUNUMBER,	8 },
    { "ninth",		tUNUMBER,	9 },
    { "tenth",		tUNUMBER,	10 },
    { "eleventh",	tUNUMBER,	11 },
    { "twelfth",	tUNUMBER,	12 },
    { "ago",		tAGO,	1 },
    { NULL }
};

/* The timezone table. */
/* Some of these are commented out because a time_t can't store a float. */
static TABLE const TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR( 0) },	/* Universal (Coordinated) */
    { "utc",	tZONE,     HOUR( 0) },
    { "wet",	tZONE,     HOUR( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	/* British Summer */
    { "wat",	tZONE,     HOUR( 1) },	/* West Africa */
    { "at",	tZONE,     HOUR( 2) },	/* Azores */
#if	0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",	tZONE,     HOUR( 3) },	/* Brazil Standard */
    { "gst",	tZONE,     HOUR( 3) },	/* Greenland Standard */
#endif
#if 0
    { "nft",	tZONE,     HOUR(3.5) },	/* Newfoundland */
    { "nst",	tZONE,     HOUR(3.5) },	/* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR(3.5) },	/* Newfoundland Daylight */
#endif
    { "ast",	tZONE,     HOUR( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	/* Yukon Daylight */
    { "hst",	tZONE,     HOUR(10) },	/* Hawaii Standard */
    { "hdt",	tDAYZONE,  HOUR(10) },	/* Hawaii Daylight */
    { "cat",	tZONE,     HOUR(10) },	/* Central Alaska */
    { "ahst",	tZONE,     HOUR(10) },	/* Alaska-Hawaii Standard */
    { "nt",	tZONE,     HOUR(11) },	/* Nome */
    { "idlw",	tZONE,     HOUR(12) },	/* International Date Line West */
    { "cet",	tZONE,     -HOUR(1) },	/* Central European */
    { "met",	tZONE,     -HOUR(1) },	/* Middle European */
    { "mewt",	tZONE,     -HOUR(1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR(1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR(1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR(1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR(1) },	/* French Summer */
    { "eet",	tZONE,     -HOUR(2) },	/* Eastern Europe, USSR Zone 1 */
    { "bt",	tZONE,     -HOUR(3) },	/* Baghdad, USSR Zone 2 */
#if 0
    { "it",	tZONE,     -HOUR(3.5) },/* Iran */
#endif
    { "zp4",	tZONE,     -HOUR(4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR(5) },	/* USSR Zone 4 */
#if 0
    { "ist",	tZONE,     -HOUR(5.5) },/* Indian Standard */
#endif
    { "zp6",	tZONE,     -HOUR(6) },	/* USSR Zone 5 */
#if	0
    /* For completeness.  NST is also Newfoundland Stanard, and SST is
     * also Swedish Summer. */
    { "nst",	tZONE,     -HOUR(6.5) },/* North Sumatra */
    { "sst",	tZONE,     -HOUR(7) },	/* South Sumatra, USSR Zone 6 */
#endif	/* 0 */
    { "wast",	tZONE,     -HOUR(7) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR(7) },	/* West Australian Daylight */
#if 0
    { "jt",	tZONE,     -HOUR(7.5) },/* Java (3pm in Cronusland!) */
#endif
    { "cct",	tZONE,     -HOUR(8) },	/* China Coast, USSR Zone 7 */
    { "jst",	tZONE,     -HOUR(9) },	/* Japan Standard, USSR Zone 8 */
#if 0
    { "cast",	tZONE,     -HOUR(9.5) },/* Central Australian Standard */
    { "cadt",	tDAYZONE,  -HOUR(9.5) },/* Central Australian Daylight */
#endif
    { "east",	tZONE,     -HOUR(10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR(10) },	/* Eastern Australian Daylight */
    { "gst",	tZONE,     -HOUR(10) },	/* Guam Standard, USSR Zone 9 */
    { "nzt",	tZONE,     -HOUR(12) },	/* New Zealand */
    { "nzst",	tZONE,     -HOUR(12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR(12) },	/* New Zealand Daylight */
    { "idle",	tZONE,     -HOUR(12) },	/* International Date Line East */
    {  NULL  }
};

/* Military timezone table. */
static TABLE const MilitaryTable[] = {
    { "a",	tZONE,	HOUR(  1) },
    { "b",	tZONE,	HOUR(  2) },
    { "c",	tZONE,	HOUR(  3) },
    { "d",	tZONE,	HOUR(  4) },
    { "e",	tZONE,	HOUR(  5) },
    { "f",	tZONE,	HOUR(  6) },
    { "g",	tZONE,	HOUR(  7) },
    { "h",	tZONE,	HOUR(  8) },
    { "i",	tZONE,	HOUR(  9) },
    { "k",	tZONE,	HOUR( 10) },
    { "l",	tZONE,	HOUR( 11) },
    { "m",	tZONE,	HOUR( 12) },
    { "n",	tZONE,	HOUR(- 1) },
    { "o",	tZONE,	HOUR(- 2) },
    { "p",	tZONE,	HOUR(- 3) },
    { "q",	tZONE,	HOUR(- 4) },
    { "r",	tZONE,	HOUR(- 5) },
    { "s",	tZONE,	HOUR(- 6) },
    { "t",	tZONE,	HOUR(- 7) },
    { "u",	tZONE,	HOUR(- 8) },
    { "v",	tZONE,	HOUR(- 9) },
    { "w",	tZONE,	HOUR(-10) },
    { "x",	tZONE,	HOUR(-11) },
    { "y",	tZONE,	HOUR(-12) },
    { "z",	tZONE,	HOUR(  0) },
    { NULL }
};

/* ARGSUSED */
static int
yyerror(s)
    char	*s;
{
  return 0;
}

#define ToTOD(Hours, Minutes, Seconds, Meridian)	\
     ( ToSeconds(Hours, Minutes, Seconds, Meridian) / SECSPERDAY )

double ToSeconds(Hours, Minutes, Seconds, Meridian)
    int	Hours;
    int	Minutes;
    double	Seconds;
    MERIDIAN	Meridian;
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 60)
	return -1.0;
    switch (Meridian) {
    case MER24:
	if (Hours < 0 
	 || Hours > 24
	 ||(Hours > 23 && Minutes != 0.0 && Seconds != 0.0))
	    return -1.0;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERam:
	if (Hours < 1 || Hours > 12)
	    return -1.0;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERpm:
	if (Hours < 1 || Hours > 12)
	    return -1.0;
	return ((Hours + 12) * 60L + Minutes) * 60L + Seconds;
    }
    /* NOTREACHED */
}


static double
Convert(Month, Day, Year, Hours, Minutes, Seconds, Meridian, DSTmode)
    int	Month;
    int	Day;
    int	Year;
    int	Hours;
    int	Minutes;
    double	Seconds;
    MERIDIAN	Meridian;
    DSTMODE	DSTmode;
{
    double	Julian;
    double	tod;
    int		j;
    time_t	unx;

    if (Year <   0) Year = -Year;
    if (Year <  50) Year += 2000;
    if (Year < 100) Year += 1900;

    Julian = juldate(Year, Month, Day, &j);
    Julian += ((double ) yyTimezone * 60L) / SECSPERDAY;

    if ((tod = ToTOD(Hours, Minutes, Seconds, Meridian)) < 0)
	return -1;
    else
    	Julian += tod;


    if ( Julian >= MJD1970 && Julian <= MJD2038 ) {
	unx = MJDtoUNIXTIME(Julian);

	if (DSTmode == DSTon
	 || (DSTmode == DSTmaybe && localtime(&unx)->tm_isdst))
	    Julian -= ((double) (60 * 60)) / SECSPERDAY;
    }

    return Julian;
}


static time_t
DSTcorrect(Start, Future)
    time_t	Start;
    time_t	Future;
{
    time_t	StartDay;
    time_t	FutureDay;

    StartDay = (localtime(&Start)->tm_hour + 1) % 24;
    FutureDay = (localtime(&Future)->tm_hour + 1) % 24;
    return (Future - Start) + (StartDay - FutureDay) * 60L * 60L;
}


static double
RelativeDate(Start, DayOrdinal, DayNumber)
    double	Start;
    time_t	DayOrdinal;
    time_t	DayNumber;
{
    struct tm	*tm;
    time_t	unx;
    time_t	now;

    if ( Start >= MJD1970 ) {
	unx = MJDtoUNIXTIME(Start);

	tm = localtime(&unx);
	now += SECSPERDAY * ((DayNumber - tm->tm_wday + 7) % 7);
	now += 7 * SECSPERDAY * (DayOrdinal <= 0 ? DayOrdinal : DayOrdinal - 1);
	return ((double) DSTcorrect(unx, now)) / SECSPERDAY;
    }

    return 0.0;
}


static double RelativeMonth(Start, RelMonth)
    double	Start;
    time_t	RelMonth;
{
    struct tm	tm;
    time_t	Month;
    time_t	Year;
    time_t	unx;
    double	mjd;
    time_t	now;

    if (RelMonth == 0 || Start < MJD1970 )
	return 0.0;

    unx = MJDtoUNIXTIME(Start);
    
    breakmjd(Start, &tm);
    Month = 12 * tm.tm_year + tm.tm_mon + RelMonth;
    Year = Month / 12;
    Month = Month % 12 + 1;
    mjd = Convert(Month, (time_t)tm.tm_mday, Year,
	(time_t)tm.tm_hour, (time_t)tm.tm_min, (double) (time_t)tm.tm_sec,
	MER24, DSTmaybe);

    now = MJDtoUNIXTIME(mjd);

    return ((double) DSTcorrect(unx, now)) / SECSPERDAY;
}


static int
LookupWord(buff)
    char		*buff;
{
    register char	*p;
    register char	*q;
    register const TABLE	*tp;
    int			i;
    int			abbrev;

    if (strcmp(buff, "T") == 0) 
	return tT;

    /* Make it lowercase. */
    for (p = buff; *p; p++)
	if (isupper(*p))
	    *p = tolower(*p);

    if (strcmp(buff, "am") == 0 || strcmp(buff, "a.m.") == 0) {
	yylval.Meridian = MERam;
	return tMERIDIAN;
    }
    if (strcmp(buff, "pm") == 0 || strcmp(buff, "p.m.") == 0) {
	yylval.Meridian = MERpm;
	return tMERIDIAN;
    }

    /* See if we have an abbreviation for a month. */
    if (strlen(buff) == 3)
	abbrev = 1;
    else if (strlen(buff) == 4 && buff[3] == '.') {
	abbrev = 1;
	buff[3] = '\0';
    }
    else
	abbrev = 0;

    for (tp = MonthDayTable; tp->name; tp++) {
	if (abbrev) {
	    if (strncmp(buff, tp->name, 3) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }
	}
	else if (strcmp(buff, tp->name) == 0) {
	    yylval.Number = tp->value;
	    return tp->type;
	}
    }

    for (tp = TimezoneTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    yylval.Number = tp->value;
	    return tp->type;
	}

    if (strcmp(buff, "dst") == 0) 
	return tDST;

    for (tp = UnitsTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    yylval.Number = tp->value;
	    return tp->type;
	}

    /* Strip off any plural and try the units table again. */
    i = strlen(buff) - 1;
    if (buff[i] == 's') {
	buff[i] = '\0';
	for (tp = UnitsTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }
	buff[i] = 's';		/* Put back for "this" in OtherTable. */
    }

    for ( tp = OtherTable; tp->name; tp++ )
	if ( strcmp(buff, tp->name) == 0 ) {
	    if ( !strcmp(tp->name, "now") ) {
		yyNow = 1;
	    }
	    yylval.Number = tp->value;
	    return tp->type;
	}

    /* Military timezones. */
    if (buff[1] == '\0' && isalpha(*buff)) {
	for (tp = MilitaryTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }
    }

    /* Drop out any periods and try the timezone table again. */
    for (i = 0, p = q = buff; *q; q++)
	if (*q != '.')
	    *p++ = *q;
	else
	    i++;
    *p = '\0';
    if (i)
	for (tp = TimezoneTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval.Number = tp->value;
		return tp->type;
	    }

    return tID;
}

int TZX() {
#if HAVE_TM_ZONE
        time_t  unx;
        struct tm *tmx;
        ctime(&unx);
        tmx = localtime(&unx);
        return tmx->tm_gmtoff;
#else
	return timezone;
#endif
}
	

static int
yylex()
{
    register char	c;
    register char	*p;
    char		buff[20];
    int			Count;
    int			sign;
    int			floa = 0;

    if ( FitsDate == 1 ) 
	if ( getenv("STARBASE_CALDATE") ) FitsDate = 0;
	else				  FitsDate = 2;

    for ( ; ; ) {
	while (isspace(*yyInput))
	    yyInput++;

	if (isdigit(c = *yyInput) || c == '-' || c == '+') {
	    if (c == '-' || c == '+') {
		sign = c == '-' ? -1 : 1;
		if (!isdigit(*++yyInput))
		    /* skip the '-' sign */
		    return '-';
	    }
	    else
		sign = 0;
	    for (yylval.Number = 0; isdigit(c = *yyInput++); )
		yylval.Number = 10 * yylval.Number + (c - '0');
	    yyInput--;

	    if ( *yyInput == '.' ) {
		double div = 10 * (sign < 0 ? -1 : 1);

		floa = 1;

		yyInput++;
		for ( ; isdigit(c = *yyInput++); ) {
		    yylval.Number += (c - '0') / div;
		    div *= 10;
		}
		yyInput--;
	    }

	    if (sign < 0)
		yylval.Number = -yylval.Number;

	    if ( floa ) { return tFNUMBER; }

	    return sign ? tSNUMBER : tUNUMBER;
	}
	if (isalpha(c)) {
	    for (p = buff; isalpha(c = *yyInput++) || c == '.'; )
		if (p < &buff[sizeof buff - 1])
		    *p++ = c;
	    *p = '\0';
	    yyInput--;
	    return LookupWord(buff);
	}
	if (c != '(')
	    return *yyInput++;
	Count = 0;
	do {
	    c = *yyInput++;
	    if (c == '\0')
		return c;
	    if (c == '(')
		Count++;
	    else if (c == ')')
		Count--;
	} while (Count > 0);
    }
}


double juldate();

/* Return the MJD as a double.
 */
double mjd(p, now, gmt)
    char		*p;
    double		 now;
    int			 gmt;
{
    double		Start;

    int	day, month, year, hour, min, sec;
    time_t unx;

    localtime(&unx);

    yyInput = p;

    yyMJD	= 0;
    yyNow	= 0;
    yyYear 	= 0;
    yyMonth 	= 0;
    yyDay 	= 0;
    yyTimezone 	= gmt ? 0 : TZX() / 60L;	/* In minutes	*/
    yyDSTmode  	= gmt ? DSToff : DSTmaybe;
    yyHour 	= 0;
    yyMinutes 	= 0;
    yySeconds 	= 0;
    yyMeridian 	= MER24;


    yyRelSeconds = 0;
    yyRelMonth 	= 0;

    yyHaveMJD	= 0;
    yyHaveDate 	= 0;
    yyHaveDay 	= 0;
    yyHaveRel 	= 0;
    yyHaveTime 	= 0;
    yyHaveZone 	= 0;

    if (yyparse()
     || yyHaveMJD > 1 || yyHaveTime > 1 || yyHaveZone > 1 || yyHaveDate > 1 || yyHaveDay > 1) {
	return -1;
    }

    if ( yyHaveMJD ) {
	return yyMJD;
    }

    if ( yyMonth > 12 ) {
	return -1;
    }

    if ( now == -1 ) {
	time_t unx;

	time(&unx);
	now = UNIXTIMEtoMJD(unx);
    }


    if ( yyHaveDate ) {
	Start = Convert(yyMonth, yyDay, yyYear, yyHour, yyMinutes, yySeconds,
		    yyMeridian, yyDSTmode);
    } else {
	if ( yyNow ) {
	    Start = now;
	} else {
	    Start = (int) now;

	    if ( yyHaveTime || yyHaveDay ) {
		Start += ToTOD(yyHour, yyMinutes, yySeconds, yyMeridian);
	    }
	}
    }


    if ( Start < 0 ) return -1;

    Start += ((double) yyRelSeconds) / SECSPERDAY;

    if ( yyRelMonth ) 
	if ( Start < MJD1970 || Start > MJD2038 ) return -1;
    	else
	    Start += RelativeMonth(Start, yyRelMonth);

    if (yyHaveDay && !yyHaveDate) {
	if ( Start < MJD1970 || Start > MJD2038 ) return -1;
	else
	    Start += RelativeDate(Start, yyDayOrdinal, yyDayNumber);
    }

    return Start;
}

struct tm *breakmjd(mjd, tm)
    double	mjd;
    struct tm	*tm;
{
    double	hour;
    double	min;
    int		yjd;
    int		j;

    /* MJD % 7 tells the day of the week
       (4=Sunday, 5=Monday, 6=Tuesday, 0=Wednesday, ...)  */

    caldate(mjd, &tm->tm_year, &tm->tm_mon, &tm->tm_mday, &hour);
    yjd = juldate(tm->tm_year, 1, 1);

    tm->tm_year -= 1900;
    tm->tm_mon--;

    tm->tm_hour = (int) hour + 0.5;
           min  =       (hour - tm->tm_hour) * 60;
    tm->tm_min  = (int)  min + 0.5;
    tm->tm_sec  =      ((min  - tm->tm_min)  * 60) + 0.5;
    tm->tm_wday = ((int) (mjd + 3) % 7) - 1;
    tm->tm_yday = (int)  (mjd - yjd)    - 1;  
    tm->tm_isdst = 0; 

    if ( tm->tm_sec >= 60 ) { tm->tm_sec = 0; tm->tm_min++;  }
    if ( tm->tm_min >= 60 ) { tm->tm_min = 0; tm->tm_hour++; }

    return tm;
}


char *cal(buffer, max, mjd, format)
	char	*buffer;
	int	 max;
	double	 mjd;
	char	*format;
{
	struct   tm tstc;
	struct   tm*tstp = &tstc;

	if ( !format ) format = "%b %e %T %Y";

	if ( mjd >= MJD1970 && mjd <= MJD2038 ) {
		time_t unx = MJDtoUNIXTIME(mjd);

	    tstp = gmtime(&unx);
	} else {
		int	 iy, im, id, j;
		double	 fd;

	    tstc.tm_mday = -1;
	    tstc.tm_wday = -1;
	    tstc.tm_yday = -1;

	    caldate(mjd, &iy, &im, &id, &fd);

	    tstc.tm_year = iy - 1900;
	    tstc.tm_mon  = im - 1;
	    tstc.tm_mday = id;
	    tstc.tm_hour =  ((int) fd / 24) * 24;
	    tstc.tm_min  = (((int) ((fd - tstc.tm_hour) / 60) * 60) + .5);
	    tstc.tm_sec  = ((int) (((fd - (tstc.tm_hour + tstc.tm_min))
			    / 60 * 60 ) * 60 * 60) + .5);
	    if ( tstc.tm_sec >= 60 ) { tstc.tm_sec = 0; tstc.tm_min++;  }
	    if ( tstc.tm_min >= 60 ) { tstc.tm_min = 0; tstc.tm_hour++; }

	    tstc.tm_isdst = 0;

	    breakmjd(mjd, &tstc);
	}

	strftime(buffer, max, format, tstp);

	return buffer;
}


