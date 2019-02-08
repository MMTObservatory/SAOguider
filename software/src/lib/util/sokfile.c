 /* sokFile.c
  */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif

#include <errno.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <netdb.h>

#ifdef BERKLEY
#include <sys/time.h>
#endif

#include "xos.h"
#include "xfile.h"

#ifdef __STDC__
int FileINetParse(char *file, int port, struct sockaddr_in *adrinet);
#endif

File SokOpen(name, port, mode)
	char *name;		/* "host:port" socket to open */
	int   port;
	int   mode;		/* mode of socket to open */
{
    int             xfd;	/* socket descriptor */
    int             type;	/* type returned from FileINetParse */
    struct sockaddr_in adrinet;	/* socket structure parsed from name */
    int 	    use = 1;
    int 	    ttl = 1;


    File            f;		/* returned file descriptor */

    if (!(type = FileINetParse(name, port, &adrinet)))
	return NULL;

    if ( mode & XFCREAT && mode & XFREAD && !(mode & XFWRITE) ) {
	if ( type == 1 ) {
	    adrinet.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	if ( ((xfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	  ||  setsockopt(xfd, SOL_SOCKET, SO_REUSEADDR,
	             (char *) &use, sizeof(use)) < 0
	  || ( (mode & XFMULTI) && 
		setsockopt(xfd, IPPROTO_IP, IP_MULTICAST_TTL,  &ttl, sizeof(ttl))
	     )
	  || (bind(xfd, (struct sockaddr *) & adrinet
			 ,sizeof(adrinet)) != 0)
	  ||  listen(xfd, 5) ) {
	    close(xfd);
	    return NULL;
	}
      } else {
	if (((xfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		   || (connect(xfd, (struct sockaddr *) & adrinet
			       ,sizeof(adrinet)) != 0)) {
	    close(xfd);
	    return NULL;
	}
    }

    f = OpenFd(xfd, "r+");

    return f;
}


int FileINetParse(file, port, adrinet)
	char *file;		/* host/socket pair to parse? */
	int   port;
	struct sockaddr_in *adrinet; /* socket info structure to fill? */
{
    struct hostent *hp;		/* -> hostent structure for host */
    char            hostname[MAXHOSTNAMELEN + 12]; /* name of host */
    char           *portstr;	/* internet port number (ascii) */
    int             type = 2;	/* return code */

    if ( !strncmp(file, "http://", 7) ) {
	file += 7;
	if ( port == -1 ) port  = 80;
    }

    strcpy(hostname, file);

#ifdef msdos
    /* This is a DOS disk discriptor, not a machine name */
    if ((!(file[0] == '.')) && file[1] == ':')
	return 0;
#endif

    if ( portstr = strchr(hostname, '/') ) {
	*portstr = '\0';
    }

    if ( portstr = strchr(hostname, ':') ) {
	*portstr++ = '\0';

        if ((port = strtol(portstr, NULL, 0)) == 0) {
	    struct servent *getservbyname();
	    struct servent *service;

	    if ((service = getservbyname(portstr, NULL)) == NULL)
	        return 0;
	    port = service->s_port;
	}
    }

    if ( port == -1 ) return 0;

    if (hostname[0] == '\0' || hostname[0] == '.') {
	type = 1;
	if (gethostname(hostname, MAXHOSTNAMELEN) == -1)
	    return 0;
    }

    if ((hp = gethostbyname(hostname)) == NULL)
	return 0;

    memset(adrinet, 0, sizeof(struct sockaddr_in));
    adrinet->sin_family = AF_INET;
    adrinet->sin_port = htons(port);
    memcpy(&adrinet->sin_addr, hp->h_addr, hp->h_length);

    return type;
}

static char *iaddrstr(addr, addrlen)
	struct sockaddr_in *addr; /* socket info structure to fill? */
	int	addrlen;
 {
     struct hostent *hp = NULL;
     char           *name;
 
    if (!(hp = gethostbyaddr((char *) &addr->sin_addr, sizeof(addr->sin_addr)
			, AF_INET))) {
	unsigned char  *a = (unsigned char *) &addr->sin_addr;

	Malloc(name, 32);
	sprintf(name, "%d.%d.%d.%d:%d", a[0], a[1], a[2], a[3]
		, ntohs(addr->sin_port));
    } else {
	Malloc(name, strlen(hp->h_name) + 10);
	sprintf(name, "%s:%d", hp->h_name, ntohs(addr->sin_port));
    }

    return name;
}

File Accept(f, mode, name)
       File f;			/* file opened by SokOpen() */
       char   *mode;		/* string containing mode e.g. "rw" */
	char **name;		/* Machine being connected.		*/
{
    struct sockaddr adrinet;	/* new socket description structure */
    int	            adrilen = sizeof(adrinet);
    int             xfd;	/* socket descriptor */

    File            client;	/* resulting stream file descriptor */


    if ((xfd = accept(FileFd(f), &adrinet, &adrilen)) == -1)
	return NULL;

    client = OpenFd(xfd, mode);

    if ( name )
	*name = iaddrstr((struct sockaddr_in *) &adrinet, adrilen);

    return client;
}

