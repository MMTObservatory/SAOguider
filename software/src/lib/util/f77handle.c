/* f77handle.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"

/* Handle Types */

#define	USED	1
#define EMPTY	2

typedef struct F77Handle {
	int	 type;
	void	*pointer;
} F77Handle;

static nhandles = 0;
static ahandles = 0;
static F77Handle *F77Handles = NULL;

void *f77pointer(hnd)
	int	hnd;
{
	if ( hnd >= nhandles ) {
		return 0;
	}

	return F77Handles[hnd].pointer;
}

int f77handle(ptr)
	void	*ptr;
{
	int i = nhandles;

	if ( nhandles >= ahandles ) {
	    for ( i = 0; i < nhandles; i++ ) {
		if ( F77Handles[i].type == EMPTY ) break;
	    }

	    if ( i >= nhandles ) {
		ahandles *= 2;
		ahandles += 10;

		F77Handles = ReAlloc(F77Handles, ahandles);
		i = nhandles;
	    }
	}

	F77Handles[i].pointer = ptr;
	F77Handles[i].type    = USED;
	nhandles++;

	return i;
}

int f77free(hnd)
	int	hnd;
{
	if ( hnd >= nhandles ) 			return 0;
	if ( F77Handles[hnd].type == EMPTY ) 	return 0; 

	F77Handles[hnd].type = EMPTY;

	return 1;
}
