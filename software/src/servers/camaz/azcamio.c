#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern int h_errno;

/* AZCAM network IO routines
 */

#define LEN_CMDBUF      300
#define WDELAY          100000
#define RDELAY          100000

int	azcamio_fd;

/* open a connection to the camera
 */
azcamio_open ()
{
	char	*hostname;
	char	*portno;
	int	port = 2402;
	unsigned short sport;
	int	status;
	int	s;
	struct  sockaddr_in sockaddr;
	struct  hostent *hp;

	status = 0;
	azcamio_fd = 0;

	hostname = getenv ("AZCAMHOST");
	if (hostname == NULL) {
	    status = 3;
	    return (status);
	}
	portno = getenv ("AZCAMHOSTPORT");
	if (portno != NULL) {
	    port = atoi(portno);
	}

	hp = gethostbyname (hostname);
	if (hp == NULL) {
	    errno = h_errno;
	    perror ("gethostbyname");
	    status = 4;
	    return (status);
	}
	sport = htons((short)port);

	/* start connection
	 */
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror ("socket");
	    status = 5;
	    return (status);
	}
	memset ((char *)&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family		= AF_INET;
	sockaddr.sin_port		= sport;
	memcpy ((void *)&sockaddr.sin_addr, (void *)hp->h_addr, hp->h_length);

	if (connect (s, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
	    perror ("connect");
	    status = 6;
	    return (status);
	}
	azcamio_fd = s;
	return (status);
}

azcamio_close ()
{
	if (azcamio_fd)
	    close (azcamio_fd);
	azcamio_fd = 0;
	return (0);
}

azcamio_command_w (command)
char	*command;
{
	char	cmdbuf[LEN_CMDBUF+1];
	char	*bp;
	int	fd, nbytes, nxfer;
	int	status;

	status = 0;
	fd = azcamio_fd;
	if (fd <= 0) {
	    status = 2;
	    return (status);
	}

	sprintf (cmdbuf, "%s\r\n", command);
	bp = cmdbuf;
	/* nbytes = strlen(cmdbuf)+1; */
	nbytes = strlen(cmdbuf);
	while (nbytes) {
	    nxfer = write (fd, bp, nbytes);
	    if (nxfer < 0) {
		perror ("azcamio write");
		break;
	    }
	    if (nxfer == 0) {
		usleep (WDELAY);
	    } else {
		bp     += nxfer;
		nbytes -= nxfer;
	    }
	}
	if (nxfer < 0) {
	    status = 2;
	    return (status);
	}

	return (status);
}

azcamio_command_r (reply, len_reply)
char	*reply;
int	len_reply;
{
	char	rplbuf[LEN_CMDBUF+2];
	char	*bp;
	int	fd, nbytes, nxfer;
	int	status;

	status = 0;
	fd = azcamio_fd;
	if (fd <= 0) {
	    status = 2;
	    return (status);
	}

	/* now read a reply line
	 */
	bp = rplbuf;
	nbytes = LEN_CMDBUF;
	while (nbytes) {
	    nxfer = read (fd, bp, nbytes);
	    if (nxfer < 0) {
		perror ("azcamio read");
		break;
	    }
	    if (nxfer == 0) {
		usleep (RDELAY);
	    } else {
		bp     += nxfer;
		nbytes -= nxfer;
		if (*(bp-1) == '\n') {
		    *(bp-1) = 0;
		    if (bp-rplbuf > 1)
			*(bp-2) = 0; /* whack CR too */
		    break;
		}
	    }
	}
	if (nxfer < 0) {
	    status = 3;
	    return (status);
	}
	if (nbytes == 0) { /* TOO LONG - probably fatal */
	    status = 4;
	    return (status);
	}
	strncpy (reply, rplbuf, len_reply);

	return (status);
}

azcamio_command (command, reply, len_reply)
char	*command;
char	*reply;
int	len_reply;
{
	int	status;

	if (status = azcamio_command_w (command))
	    return (status);
	if (status = azcamio_command_r (reply, len_reply))
	    return (status);
	return (status);
}

azcamio_get (command, reply, len_reply)
char	*command;
char	*reply;
int	len_reply;
{
	int	status;
	char	rplbuf[LEN_CMDBUF+2];

	rplbuf[0] = 0;
	status = azcamio_command (command, rplbuf, LEN_CMDBUF);
	if (status)
	    return (status);

	if (!strncmp (rplbuf, "OK", 2)) {
	    strncpy (reply, rplbuf+2, len_reply);
	    status = 0;
	} else if (!strncmp (rplbuf, "ERROR", 5)) {
	    strncpy (reply, rplbuf+5, len_reply);
	    status = -1;
	} else {
	    status = -2;
	}
	return (status);
}

azcamio_set_w (command)
char	*command;
{
	int	status;

	status = azcamio_command_w (command);
	if (status)
	    return (status);

	return (status);
}

azcamio_set_r ()
{
	int	status;
	char	rplbuf[LEN_CMDBUF+2];

	rplbuf[0] = 0;
	status = azcamio_command_r (rplbuf, LEN_CMDBUF);
	if (status)
	    return (status);

	if (!strncmp (rplbuf, "OK", 2)) {
	    status = 0;
	} else if (!strncmp (rplbuf, "ERROR", 5)) {
	    status = -1;
	} else {
	    status = -2;
	}
	return (status);
}

azcamio_set (command)
char	*command;
{
	int	status;

	if (status = azcamio_set_w (command))
	    return (status);
	if (status = azcamio_set_r ())
	    return (status);

	return (status);
}


int	azcamio_lfd;
int	azcamio_afd;

/* open a listener to get data from the camera
 */
azcamio_lopen ()
{
	int	port = 2403;
	unsigned short sport;
	int	status;
	int	s;
	struct  sockaddr_in sockaddr;
	struct  hostent *hp;
	int	on;

	status = 0;
	azcamio_lfd = 0;

	sport = htons((short)port);

	/* start connection
	 */
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror ("socket");
	    status = 5;
	    return (status);
	}
	on = 1;
	if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on)) < 0) {
	    perror ("setsockopt");
	    status = 4;
	    return (status);
	}
	memset ((char *)&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family		= AF_INET;
	sockaddr.sin_port		= sport;
	sockaddr.sin_addr.s_addr        = INADDR_ANY;

	if (bind (s, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
	    perror ("bind");
	    status = 6;
	    return (status);
	}
	if (listen (s, 10) < 0) {
	    perror ("listen");
	    status = 7;
	    return (status);
	}
	azcamio_lfd = s;
	return (status);
}

azcamio_aopen ()
{
	int	status;
	int	s;

	status = 0;
	azcamio_afd = 0;

	s = accept (azcamio_lfd, (struct sockaddr *)0, (int *)0);
	if (s < 0) {
	    perror ("accept");
	    status = 7;
	    azcamio_lclose ();
	    return (status);
	}
	azcamio_afd = s;
	return (status);
}

azcamio_lclose ()
{
	if (azcamio_lfd)
	    close (azcamio_lfd);
	azcamio_lfd = 0;
	return (0);
}

azcamio_aclose ()
{
	if (azcamio_afd)
	    close (azcamio_afd);
	azcamio_afd = 0;
	return (0);
}

azcamio_startdataread ()
{
	char	rplbuf[256];
	char	*bp;
	int	fd, nbytes, nxfer;
	int	status;

	status = 0;
	fd = azcamio_afd;
	if (fd <= 0) {
	    status = 2;
	    return (status);
	}

	/* read header
	 */
	bp = rplbuf;
	nbytes = sizeof(rplbuf);
	while (nbytes) {
	    nxfer = read (fd, bp, nbytes);
	    if (nxfer < 0) {
		perror ("azcamio startdataread");
		break;
	    }
	    if (nxfer == 0) {
		usleep (RDELAY);
	    } else {
		bp     += nxfer;
		nbytes -= nxfer;
	    }
	}

	if (nxfer < 0) {
	    status = 3;
	    return (status);
	}

	/* reply to client
	 */
	bp = "0               ";
	nbytes = 16;
	while (nbytes) {
	    nxfer = write (fd, bp, nbytes);
	    if (nxfer < 0) {
		perror ("azcamio startdataread reply");
		break;
	    }
	    if (nxfer == 0) {
		usleep (WDELAY);
	    } else {
		bp     += nxfer;
		nbytes -= nxfer;
	    }
	}

	if (nxfer < 0) {
	    status = 8;
	    return (status);
	}

	return (status);
}

azcamio_dataread (buffer, nbytes)
char	*buffer;
int	nbytes;
{
	char	*bp;
	int	fd, nxfer;
	int	status;
	int	ntotal;

	status = 0;
	fd = azcamio_afd;
	if (fd <= 0) {
	    status = 0;
	    return (status);
	}

	/* now read some data
	 */
	ntotal = 0;
	bp = buffer;
	while (nbytes) {
	    nxfer = read (fd, bp, nbytes);
	    if (nxfer < 0) {
		perror ("azcamio readdata");
		break;
	    }
	    if (nxfer == 0) {
		usleep (RDELAY);
	    } else {
		bp     += nxfer;
		nbytes -= nxfer;
		ntotal += nxfer;
	    }
	}
	if (nxfer < 0) {
	    status = 0;
	    return (status);
	}

	return (ntotal);
}

/* translate a hostname string into a dotted decimal ip string
 */
host2ip (hostname, ipname)
char	*hostname;
char	*ipname;
{
	struct	hostent	*he;

	he = gethostbyname (hostname);
	sprintf (ipname, "%d.%d.%d.%d", (unsigned char)he->h_addr[0],
					(unsigned char)he->h_addr[1],
					(unsigned char)he->h_addr[2],
					(unsigned char)he->h_addr[3]);
}
