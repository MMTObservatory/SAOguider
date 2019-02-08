#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <magick/api.h>

/* write a string to socket
 * return <0 on error
 */
static int write2net (int fd, char* buffer)
{
    char *bp;
    int nxfer;
    int nchars;

    bp = buffer;
    while ((nchars = strlen(bp)) > 0) {
	nxfer = write (fd, bp, nchars);
	if (nxfer < 0) {
	    return (-1); /* error */
	} else if (nxfer == 0) {
	    usleep (10000);
	} else { /* must be >= 1 */
	    bp += nxfer;
	}
    }
    return (bp-buffer);
}

/* read bytes up to a newline from a file descriptor.
 * read one byte at a time;
 * the lines are short, so not that inefficient
 * replace the newline with a 0
 * skip \r
 * Return strlen (may be 0) on success,
 */
static int readaline (int fd, char* buffer, int maxchars)
{
    char *bp;
    int nread;

    bp = buffer;
    while (bp < buffer+maxchars) {
	nread = read (fd, bp, 1);
	if (nread < 0) {
	    return (-1);
	} else if (nread > 1) {
	    return (-2);
	} else if (nread == 0) {
	    usleep (10000);
	} else { /* must be == 1 */
	    if (*bp == '\n') {
		*bp = 0;
		return (bp-buffer);
	    }
	    if (*bp != '\r')
		bp++;
	}
    }
    return (-3);
}

/* read maxchars from net into buffer
 * return 0 on success
 */
static int read_net (int fd, unsigned char* buffer, int maxchars)
{
    unsigned char *bp;
    int nread;
    int nchars = maxchars;

    bp = buffer;
    while (bp < buffer+maxchars) {
	nread = read (fd, bp, nchars);
	if (nread < 0) {
	    return (-1);
	} else if (nread == 0) {
	    usleep (10000);
	} else {
	    bp += nread;
	    nchars -= nread;
	}
    }
    return (0);
}

static int jpeg2pixels (unsigned char *jpeg, unsigned char *data, int length) {
    ExceptionInfo exception;
    ImageInfo *image_info;
    Image *image;
    const PixelPacket *pixels;
    int n;
    int status = 0;

    InitializeMagick("camera_videoserver");
    GetExceptionInfo(&exception);
    image_info=CloneImageInfo((ImageInfo *) NULL);
    image = BlobToImage(image_info, jpeg, length, &exception);
    if (image != NULL) {
	pixels = AcquireImagePixels(image, 0, 0, 640, 480, &exception);
	if (pixels != NULL) {
	    for (n = 0; n < 640*480; n++) {
		data[n] = pixels[n].green;
	    }
	} else {
	    status = -2;
	}
	DestroyImage(image);
    } else {
        status = -1;
    }
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();
    return (status);
}

/* get one jpeg from the videoserver and return its pixel data
 * use a fully qualified hostname
 * data should be 640x480
 * returns 0 on success
 */
int videoserver (char *hostname, unsigned char *data)
{
    int     hstatus;
    struct  hostent ret;
    char    buf[1000];
    struct  hostent *result;
    int     h_error;
    int     s;
    int     dstport = 80;
    unsigned short dport;
    struct  sockaddr_in dstaddr;
    char    *Content_Length = "Content-Length: ";
    int     content_length = 0;
    unsigned char *jpeg;
    int	status = 0;

    hstatus = gethostbyname_r (hostname, &ret, buf, sizeof(buf), &result, &h_error);
    if ((hstatus != 0) || (result == NULL)) {
        return (-1);
    }

    if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	return (-2);
    }

    dport = htons((short)dstport);
    memset ((char *)&dstaddr, 0, sizeof(dstaddr));
    dstaddr.sin_family              = AF_INET;
    dstaddr.sin_port                = dport;
    memcpy ((void *)&dstaddr.sin_addr, (void *)ret.h_addr, ret.h_length);
    
    if (connect (s, (struct sockaddr *)&dstaddr, sizeof(dstaddr)) < 0) {
        status = -3;
    } else {
	if (write2net (s, "GET /axis-cgi/jpg/image.cgi HTTP/1.1\nHost: ") < 0) {
	    close (s);
	    return (-4);
	}
	if (write2net (s, hostname) < 0) {
	    close (s);
	    return (-4);
	}
	if (write2net (s, ":80\n\n") < 0) {
	    close (s);
	    return (-4);
	}

	do {
	    if (readaline (s, buf, sizeof(buf)) < 0) {
		close (s);
		return (-5);
	    }
	    if (strncmp (buf, Content_Length, strlen(Content_Length)) == 0) {
		if (sscanf (buf+strlen(Content_Length), "%d", &content_length) != 1) {
		    close (s);
		    return (-6);
		}
	    }
	} while (strlen (buf) > 0);
	if (content_length <= 0) {
	    close (s);
	    return (-7);
	}
	if ((jpeg = malloc (content_length)) == NULL) {
	    status = -8;
	} else {
	    if (read_net (s, jpeg, content_length) != 0) {
		status = -9;
	    } else {
		if (jpeg2pixels (jpeg, data, content_length) != 0)
		    status = -10;
	    }
	    free (jpeg);
	}
    }
    close (s);

    return (status);
}
