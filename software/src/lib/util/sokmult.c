
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <xos.h>
#include <xfile.h>

/* xfile mutlicast class
 */

#include "xos.h"
#include "xfile.h"
#include "sokmult.h"

#ifdef __STDC__
int FileINetParse(char *file, int port, struct sockaddr_in *adrinet);
#endif

typedef struct MSok {
    struct sockaddr_in sockaddr;
} *MSok;

int xfile_freeclose(f)
	File    f; { Free(xFile(f)->data);

	xFileDone(f);
	return close(f);
}


struct _xFileClass xFileMulti = {
	/* ivector write;	*/	  NULL
	/* ivector read;	*/	, NULL
	/* ivector seek;	*/	, NULL
	/* ivector flush;	*/	, NULL
	/* ivector close;	*/	, xfile_freeclose
	/* ivector getc;	*/	, NULL
	/* ivector putc;	*/	, NULL
	/* ivector ungetc;	*/	, NULL
	/* ivector print;	*/	, NULL
	/* ivector setbuffer;	*/	, NULL
	/* ivector left;	*/	, NULL
	/* void	 *data;		*/	, NULL
};

struct        sockaddr_in *SokAddr(File file)
{
    return &((MSok) xFile(file)->data)->sockaddr;
}

File SokMult(char *name, int port, int mode)
{
    File	  f;
    MSok   	  msok;

    int		  type;
    unsigned char ttl = 1;
    int 	  use = 1;

    int		  sok;
    struct        sockaddr_in sockaddr;

    if (!(type = FileINetParse(name, port, &sockaddr))) {
	return NULL;
    }

    if ( (sok = socket(PF_INET, SOCK_DGRAM, 0)) < 0 ) {
	return NULL;
    }
    if ( mode & XFCREAT ) {

	setsockopt(sok, SOL_SOCKET, SO_REUSEADDR,      &use, sizeof(use));
	setsockopt(sok, IPPROTO_IP, IP_MULTICAST_TTL,  &ttl, sizeof(ttl));

        if (bind(sok, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
	    close(sok);
            return 0;
        }

    } else {
	struct ip_mreq imr;

	imr.imr_multiaddr.s_addr = sockaddr.sin_addr.s_addr;
	imr.imr_interface.s_addr = htonl(INADDR_ANY);
#ifdef SOLARIS
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	setsockopt(sok, SOL_SOCKET, SO_REUSEADDR, &use, sizeof(use));

        if (bind(sok, (struct sockaddr *) &sockaddr
		    , sizeof(sockaddr)) < 0) {
	    close(sok);
            return 0;
        }

	if (setsockopt(sok, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, \
		    sizeof(struct ip_mreq)) < 0) {
	    close(sok);
	    return NULL;
	}
    }

    f = OpenFd(sok, "rw");

    Malloc(msok, sizeof(struct MSok));
    msok->sockaddr = sockaddr;

    xFileInit(f, &xFileMulti);
    xFile(f)->data = msok;
    return f;
}
