/* Multiplex i/o
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include "xos.h"
#include "xfile.h"

void muxio(npoll, polls, actions, adata, timeout)
	int	   npoll;
	Polls	  *polls;
	int	(**actions)();
	void	 **adata;
	double	   timeout;
{
	int	i;
	int	nactive = 0;

    for ( i = 0; i < npoll; i++ ) {
	if ( polls[i].f != NULL ) nactive++;
    }

    while ( nactive && Poll(npoll, polls, timeout) > 0 ) {
	for ( i = 0; i < npoll; i++ ) {
	    if ( polls[i].rmode ) {
		if ( (*(actions[i]))(polls[i].f, adata[i], i) != 0 ) {
		    polls[i].f = NULL;
		    nactive--;
		}
	    }
	}
    }
}
