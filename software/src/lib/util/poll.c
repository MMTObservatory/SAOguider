/* Poll
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include <sys/time.h>
#include "xos.h"
#include "xfile.h"

int Poll(nfds, polls, timeout)
	 int nfds;		/* size of polls array */
	 Polls *polls;		/* array of file descriptors to poll */
	 double timeout;	/* timeout -- time to wait for polling */
{
    int i;			/* index variable */
    int ret;			/* return code */
    int width = getdtablesize(); /* number of descriptors for select() */

    fd_set rfds;		/* files ready for reading */
    fd_set wfds;		/* files ready for writing */
    fd_set xfds;		/* files with exceptions pending */

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&xfds);

    ret = 0;

    for (i = nfds; i--;) {
	polls[i].rmode = 0;
	if (polls[i].f == NULL)
	    continue;

	if (polls[i].qmode & XFREAD)
	    FD_SET(FileFd(polls[i].f), &rfds);
	if (polls[i].qmode & XFWRITE)
	    FD_SET(FileFd(polls[i].f), &wfds);
    }

    if ((ret = Select(width, &rfds, &wfds, &xfds, timeout)) == -1)
	return -1;

    for (i = nfds; ret && i--;) {
	if (polls[i].f == NULL)
	    continue;

	if (FD_ISSET(FileFd(polls[i].f), &rfds))
	    polls[i].rmode |= XFREAD;
	if (FD_ISSET(FileFd(polls[i].f), &wfds))
	    polls[i].rmode |= XFWRITE;
    }

    return ret;
}

int Select(width, rfds, wfds, xfds, timeout)
	int	width;
	fd_set *rfds;
	fd_set *wfds;
	fd_set *xfds;
	double	timeout;
{
    struct timeval to, *timeoutp;

    if ( timeout == -1.0 ) {
	timeoutp = NULL;
    } else {
	timeoutp = &to;

	to.tv_sec  = (int) timeout;
	to.tv_usec = (timeout - to.tv_sec) * 1000000;
    }

    return select(width, rfds, wfds, xfds, timeoutp);
}
