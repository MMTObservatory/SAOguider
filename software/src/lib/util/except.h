/* except.h
 *
 * A simple exception handler.
 */

#ifndef _EXCEPT_H
#define _EXCEPT_H

#ifdef __STDC__
# ifndef __P
#  define __P(x)  x
# endif
#else
# define __P(x)  ( )
#endif

#include <setjmp.h>

#define ERRSTRLEN	2048

typedef struct excbox {
    struct excbox  *next;
    jmp_buf         jmp;
}               excbox;

typedef char   *exception;
typedef int   (*evector)();

extern exception error_ex;		/* Name the fatal exception	 	*/
extern exception fatal_ex;		/* Name the error exception		*/

extern excbox   *excstk;		/* The stack of error handelers		*/
extern exception excval;		/* Value of the current exception	*/
extern void	 excjmp();
extern evector   excvec();		/* Change the error handler?		*/

extern char    *errmsg();		/* return the error string.		*/
extern char    *exc_oserror();		/* return currnet OS error string.	*/

extern void     error __P((char *format, ...));
extern void     fatal __P((char *format, ...));
extern void	except __P((exception e, char *format, ...));

#define iferr(code) {						\
		       excbox	excmrk;				\
		       if ( !setjmp(excpsh(&excmrk)) ) {  	\
			  code ;				\
			  excclr();				\
		       }                                        \
                       excpop();				\
		    }						\
                    if ( excchk() )

#define excpsh(stk)	((stk)->next = excstk, excstk = stk, excstk->jmp)
#define excpop()	( (excstk != NULL) ? (excstk = excstk->next)	\
                                 : (fatal("exception stack pops past top\n") , excstk))

#define excchk()	excval != NULL
#define excclr()	excval  = NULL

#define ex_check(code)	iferr ( code ) { if ( 0 ) 0;
#define ex_if(except)   else if ( excval == except )
#define ex_end		else excjmp(); }

/* EXCEPT_H */
#endif
