/* xos.h
**/

#ifndef os_h
#define os_h

#include <sys/types.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef NULL
#define	NULL	0
#endif

#ifdef __STDC__
#include <stdarg.h>
#define PROTOTYPE(X)	 X
#else
#include <varargs.h>
#define PROTOTYPE(X)	( )
#endif

#if STDC_HEADERS
#include <string.h>
#else
#ifndef HAVE_STRCHR
#define strchr index
#define strrchr rindex
#endif

#ifndef HAVE_MEMCPY
#define memcpy(d, s, n) bcopy ((s), (d), (n))
#define memmove(d, s, n) bcopy ((s), (d), (n))
#else
#define bcopy(s, d, n) memcpy( (d), (s), (n))
#endif

#include <unistd.h>

char *strchr();
char *strrchr();

char *strcpy();
char *strncpy();
char *strdup();
int   strlen();
#endif


#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#else
void qsort();
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
void  *malloc();
void  *calloc();
void  *realloc();
void  free();
#endif 
int   tmatch();


#define Min(x, y)	(((x) < (y)) ? (x) : (y))
#define Max(x, y)	(((x) > (y)) ? (x) : (y))

#define Clip(min, max, val)	 (((val) < (min) ) ? (min) :		\
				 (((val) > (max) ) ? (max) : (val)))

#define Abs(xx)		(((xx) > 0) ? (xx) : -(xx) )

#ifdef MSDOS
#define sleep	SAOSleep
#endif

#ifdef __GNU_C__
#define INLINE inline
#else
#define INLINE
#endif


#define New(space, thing)	Calloc(space, sizeof(thing))
#define NewString(space, str)	(( str == NULL ) 			    \
				    ? NULL				    \
				    : strcpy(Malloc(space, strlen(str) + 1) \
						, str))

#ifdef _EXCEPT_H

extern exception ex_malloc;
#define MEMEX(exc, size, space)	, ((space) ? (space) : 								\
					( except(exc, "cannot allocate %d bytes in %s line %d", size, __FILE__, __LINE__)  \
					, space))
#else
#define MEMEX(exc, size, space)
#endif

#ifdef DEBUG
#define Malloc(space, size) ( ((space) = (void *) malloc(size))    	\
    		, printf("malloc : 0x%09lx %10d bytes - " #space "\n"	\
			, space, size)		   			\
		    , space)


#define Calloc(space, size) ( ((space) = (void *) malloc(size))		\
	    		, printf("calloc : 0x%09lx %10d bytes - " #space "\n"\
				, space, size)		   		\
			    , memset(space, 0, size)			\
			    , space)

#define Free(space)	    ( space != NULL ?				      \
		printf("free   : 0x%09lx                  - " #space "\n", space)  \
		, free((char *) space), (int) (space = NULL)		      \
	:	printf("free   : (null)\n"), (void *) NULL )

#define ReAlloc(space, size) ( space != NULL ?				\
		printf("relloc : 0x%09lx %10d bytes - " #space "\n"	\
		, ((space) = (void *) realloc((void*)space, size))	\
					, size), (void *) space		\
	:	(void *) Malloc(space, size) )

#define D(args)	printf args

#else
#define Malloc(space, size)	( (space) = (void *) malloc(size) MEMEX(ex_malloc, size, space) )
#define ReAlloc(space, size) ( space ? 					    			\
				  (void *)(((space) = (void *) realloc((void*)space, size)) 	\
				  MEMEX(ex_malloc, size, space)) 				\
				: (void *) Malloc(space, size) )

#define Calloc(space, size)	( Malloc(space, size), memset(space, 0, size), space )

#define Free(space)	     ( space ?	( free((char *) space), (void *)(space = NULL))	    \
				: (void *) NULL )

#define D(args)
#endif

typedef double  real;
typedef int     boolean;

typedef struct _complex {
    double          r;
    double          i;
}               complex;

typedef void	(*vfunct) ();
typedef boolean	(*bfunct) ();
typedef short   (*sfunct) ();
typedef int     (*ifunct) ();
typedef off_t   (*ofunct) ();
typedef long    (*lfunct) ();
typedef real    (*rfunct) ();
typedef double  (*dfunct) ();
typedef complex (*xfunct) ();

#define MIN_normal	0.99e-306
#define MAX_normal	0.99e306


/* Types.h
**
** Data type constants
**/

#ifndef INDEFS
#define INDEFS          (-32767)
#endif
#ifndef INDEFL
#define INDEFL          ((unsigned long) 0x80000001)
#endif
#ifndef INDEFI
#define INDEFI          INDEFL
#endif
#ifndef INDEFR
#define INDEFR          1.6e38
#endif
#ifndef INDEFD
#define INDEFD          1.6e308
#endif
#ifndef INDEF
#define INDEF           INDEFR
#endif


#define	TY_CHAR		0
#define	TY_SHORT	1
#define	TY_INT		2
#define	TY_LONG		3
#define	TY_REAL		4
#define	TY_DOUBLE	5

#define TY_UCHAR	6
#define TY_USHORT	7
#define TY_UINT		8
#define TY_ULONG	9


#define	SZ_CHAR		1
#define	SZ_SHORT	2
#define	SZ_INT		4
#define	SZ_LONG		4
#define	SZ_REAL		4
#define	SZ_DOUBLE	8

double SAOstrtod();

#define X__PI	3.14159265358979323846
#define X_2PI	( 2 * X__PI )
#define X_R2D	(X_2PI / 360.0)
#define X_R2H	(X_2PI /  24.0)
#define X_H2D	(360.0 /  24.0)

#define r2h(r)	( (r) / X_R2H )
#define h2r(d)	( (d) * X_R2H )
#define r2d(r)	( (r) / X_R2D )
#define d2r(d)	( (d) * X_R2D )
#define h2d(r)	( (r) * X_H2D )
#define d2h(d)	( (d) / X_H2D )


void *MemSegment();

#define SigAction(sig, func, mask, flag)        {                       \
                struct  sigaction sigact;                               \
                sigact.sa_handler = func;                               \
                sigact.sa_flags   = flag;                               \
                memcpy(&sigact.sa_mask, mask, sizeof(sigset_t));        \
                                                                        \
        sigaction(sig, &sigact, NULL);          }

#endif

int	coprocess();
int	coroutine();
void	muxio();

int Poll();

/* end os_h */
