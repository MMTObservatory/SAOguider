#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>


#include "ximtool.h"
#include "iis.h"

/*
 * IIS.C -- IRAF/IIS "imtool" protocol module.  This code is responsible for
 * accepting connections from remote network clients and communicating with
 * them via the imtool/iis image display server communications prototcol.
 *
 *	   fd = xim_iisopen (xim)
 *	       xim_iisclose (xim)
 *		  xim_iisio (xim, &fd, &id)
 *
 *           xim_frameLabel (xim)
 *            xim_encodewcs (xim, sx, sy, sz, obuf)
 *         xim_retCursorVal (xim, sx, sy, frame, wcs, key, strval)
 *
 *	       xim_iisiomap (w, iomap, &iomap_len)
 *	    xim_iiscolormap (w, r, g, b, &first, &ngray, &rgb_len)
 *
 * xim_iisio is a callback procedure called by Xt when there is input to be
 * processed on the stream used to communicate with the remote client.
 */

#define	MEMORY		01		/* frame buffer i/o		*/
#define	LUT		02		/* lut i/o			*/
#define	FEEDBACK	05		/* used for frame clears	*/
#define	IMCURSOR	020		/* logical image cursor		*/
#define	WCS		021		/* used to set WCS		*/
#define	SZ_IOBUF	65536		/* max size data transfer	*/
#define IO_TIMEOUT	30		/* i/o not getting anywhere     */
#define	MAXCONN		5


#define	SZ_IMCURVAL	160
#define	PACKED		0040000
#define	COMMAND		0100000
#define	IIS_READ	0100000
#define	IMC_SAMPLE	0040000
#define	IMT_FBCONFIG	077
#define	XYMASK		077777

struct	iism70 {
	short	tid;
	short	thingct;
	short	subunit;
	short	checksum;
	short	x, y, z;
	short	t;
};

extern int errno;
static void set_fbconfig();
static int decode_frameno();
static int bswap2();

static IoChanPtr open_fifo(), open_inet(), open_unix();
static int xim_connectClient();
static void xim_disconnectClient();
static IoChanPtr get_iochan();


/* XIM_IISOPEN -- Initialize the IIS protocol module and ready the module to
 * accept client connections and begin processing client requests.  Clients
 * may connect to the server using a fifo connection or an internet or
 * UNIX domain socket connection.  All three types of server ports are
 * simultaneously ready to receive client connections.
 */
xim_iisopen (xim)
register XimDataPtr xim;
{
	int nopen = 0;

	if (open_fifo (xim))
	    nopen++;
	if (open_inet (xim))
	    nopen++;
	if (open_unix (xim))
	    nopen++;

	return (nopen);
}


/* XIM_IISCLOSE -- Close down the IIS protocol module.
 */
void
xim_iisclose (xim)
register XimDataPtr xim;
{
	register IoChanPtr chan;
	register int i;

	for (i=0, chan=NULL;  i < MAX_CLIENTS;  i++) {
	    chan = &xim->chan[i];
	    if (chan->id) {
		xim_removeInput (xim, chan->id);
		chan->id = NULL;
	    }

	    switch (chan->type) {
	    case IO_FIFO:
		if (chan->keepalive >= 0)
		    close (chan->keepalive);
		if (chan->datain >= 0)
		    close (chan->datain);
		if (chan->dataout >= 0)
		    close (chan->dataout);
		chan->type = 0;
		break;

	    case IO_INET:
		close (chan->datain);
		chan->type = 0;
		break;

	    case IO_UNIX:
		close (chan->datain);
		unlink (chan->path);
		chan->type = 0;
		break;
	    }
	}
}


/* OPEN_FIFO -- Ooen the (x)imtool fifo port and make ready to accept client
 * connections and begin processing client requests.  There is no client
 * yet at this stage.
 */
static IoChanPtr
open_fifo (xim)
register XimDataPtr xim;
{
	register IoChanPtr chan;
	int datain, dataout;
	int keepalive;

	/* Setting the input fifo to "none" or the null string disables
	 * fifo support.
	 */
	if (!xim->input_fifo[0] || strcmp(xim->input_fifo,"none")==0)
	    return (NULL);

	datain = dataout = -1;

	/* Open the output fifo (which is the client's input fifo).  We have
	 * to open it ourselves first as a client to get around the fifo
	 * open-no-client error.  
	 */
	if ((datain = open (xim->input_fifo, O_RDONLY|O_NDELAY)) != -1) {
	    if ((dataout = open (xim->input_fifo, O_WRONLY|O_NDELAY)) != -1)
		fcntl (dataout, F_SETFL, O_WRONLY);
	    else
		goto done;
	    close (datain);
	} else
	    goto done;

	/* Open the input stream, a FIFO pseudodevice file used by
	 * applications to send us commands and data.
	 */
	if ((datain = open (xim->output_fifo, O_RDONLY|O_NDELAY)) == -1)
	    goto done;
	else {
	    /* Clear O_NDELAY for reading. */
	    fcntl (datain, F_SETFL, O_RDONLY);

	    /* Open the client's output fifo as a pseudo-client to make it
	     * appear that a client is connected.
	     */
	    keepalive = open (xim->output_fifo, O_WRONLY);
	}
done:
	/* Allocate and fill in i/o channel descriptor. */
	if (datain > 0 && dataout > 0 && (chan = get_iochan(xim))) {
	    chan->xim = (XtPointer) xim;
	    chan->type = IO_FIFO;
	    chan->datain = datain;
	    chan->dataout = dataout;
	    chan->keepalive = keepalive;
	    chan->reference_frame = 1;
	} else {
	    fprintf (stderr, "Warning: cannot open %s\n", xim->output_fifo);
	    chan = NULL;
	}

	/* Register input callback. */
	if (chan) {
	    chan->id = xim_addInput (xim, chan->datain, xim_iisio,
		(XtPointer)chan);
	} else {
	    if (datain > 0)
		close (datain);
	    if (dataout > 0)
		close (dataout);
	}

	return (chan);
}


/* OPEN_INET -- Set up a port to be used for incoming client connections
 * using internet domain sockets.
 */
static IoChanPtr
open_inet (xim)
register XimDataPtr xim;
{
	register int s = 0;
	register IoChanPtr chan;
	struct sockaddr_in sockaddr;
        int      reuse_addr = 1;


	/* Setting the port to zero disables inet socket support. */
	if (xim->port <= 0)
	    return (NULL);

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	    goto err;

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                     (char *) &reuse_addr, sizeof(reuse_addr));

	memset ((void *)&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons((short)xim->port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (s, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	    goto err;

	if (listen (s, MAXCONN) < 0)
	    goto err;

	/* Allocate and fill in i/o channel descriptor. */
	if (chan = get_iochan(xim)) {
	    chan->xim = (XtPointer) xim;
	    chan->type = IO_INET;
	    chan->datain = s;
	    chan->dataout = s;
	    chan->reference_frame = 1;

	    /* Register connectClient callback. */
	    chan->id = xim_addInput (xim,s,xim_connectClient,(XtPointer)chan);
	    return (chan);
	}
err:
	fprintf (stderr, "ximtool: cannot open socket on port %d, errno=%d\n",
	    xim->port, errno);
	perror("ximtool");
	if (s)
	    close (s);
	return (NULL);
}


/* OPEN_UNIX -- Set up a port to be used for incoming client connections
 * using unix domain sockets.
 */
static IoChanPtr
open_unix (xim)
register XimDataPtr xim;
{
	register int s = 0;
	register IoChanPtr chan;
	struct sockaddr_un sockaddr;
	char path[256];

	/* Setting the addr to "none" or the null string disables unix
	 * socket support.
	 */
	if (!xim->unixaddr[0] || strcmp(xim->unixaddr,"none")==0)
	    return (NULL);

	/* Get path to be used for the unix domain socket. */
	sprintf (path, xim->unixaddr, getuid());
	unlink (path);

	if ((s = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
	    goto err;

	memset ((void *)&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sun_family = AF_UNIX;
	strcpy (sockaddr.sun_path, path);
	if (bind (s, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	    goto err;

	if (listen (s, MAXCONN) < 0)
	    goto err;

	/* Allocate and fill in i/o channel descriptor. */
	if (chan = get_iochan(xim)) {
	    chan->xim = (XtPointer) xim;
	    chan->type = IO_UNIX;
	    chan->datain = s;
	    chan->dataout = s;
	    chan->reference_frame = 1;
	    strncpy (chan->path, path, SZ_FNAME);

	    /* Register connectClient callback. */
	    chan->id = xim_addInput (xim,s,xim_connectClient,(XtPointer)chan);
	    return (chan);
	}
err:
	fprintf (stderr, "ximtool: cannot open socket on port %s, errno=%d\n",
	    path, errno);
	if (s)
	    close (s);
	return (NULL);
}


/* XIM_CONNECTCLIENT -- Called when a client has attempted a connection on
 * a socket port.  Accept the connection and set up a new i/o channel to
 * communicate with the new client.
 */
static int
xim_connectClient (source, chan_port)
int *source;
IoChanPtr chan_port;
{
	register XimDataPtr xim = (XimDataPtr) chan_port->xim;
	register IoChanPtr chan;
	register int s;

	int	fd = *source;

	/* Accept connection. */
	if ((s = accept ( fd, (struct sockaddr *)0, (int *)0)) < 0)
	    return;
	if (fcntl (s, F_SETFD, O_RDWR|O_NDELAY) < 0) {
	    close (s);
	    return 1;
	}

	/* Allocate and fill in i/o channel descriptor. */
	if (chan = get_iochan(xim)) {
	    chan->xim = (XtPointer) xim;
	    chan->type = chan_port->type;
	    chan->datain = s;
	    chan->dataout = s;
	    chan->reference_frame = 1;
	    chan->rf_p = &xim->frame[1];
	    chan->id = xim_addInput (xim, s, xim_iisio, (XtPointer)chan);
	}

    	xim_open(chan); 
	return 1;
}


/* XIM_DISCONNECTCLIENT -- Called to close a client connection when EOF is
 * seen on the input port.  Close the connection and free the channel
 * descriptor.
 */
static void
xim_disconnectClient (chan)
register IoChanPtr chan;
{
	if ( chan == chan->xim->cursor_chan )
	    xim_cursorMode (chan->xim, chan->reference_frame, 0);

    	xim_close(chan); 

	switch (chan->type) {
	case IO_INET:
	case IO_UNIX:
	    close (chan->datain);
	    if (chan->id) {
		xim_removeInput (chan->xim, chan->id);
		chan->id = NULL;
	    }
	    chan->type = 0;
	    break;
	default:
	    break;
	}
}


/* GET_IOCHAN --- Get an i/o channel descriptor.
 */
static IoChanPtr
get_iochan (xim)
register XimDataPtr xim;
{
	register IoChanPtr chan;
	register int i;

	for (i=0;  i < MAX_CLIENTS;  i++)
	    if (!xim->chan[i].type)
		return (&xim->chan[i]);

	return (NULL);
}


/* XIM_IISIO -- Xt file i/o callback procedure, called when there is input
 * pending on the data stream to the ximtool client.
 */
int
xim_iisio (fd_addr, chan)
int *fd_addr;
IoChanPtr chan;
{
	register XimDataPtr xim = (XimDataPtr) chan->xim;
	register int sum, i;
	register short *p;
	int	datain = *fd_addr;
	int	dataout = chan->dataout;
	int	ndatabytes, nbytes, n, ntrys=0;
	static	int errmsg=0, bswap=0;
	struct	iism70 iis;
	char	buf[SZ_FIFOBUF];

	/* Get the IIS header. */
	if ((n = read (datain, (char *)&iis, sizeof(iis))) < sizeof(iis)) {
	    if (n <= 0)
		xim_disconnectClient (chan);
	    else
		fprintf (stderr, "imtool: command input read error\n");

	    return 0;
	} else if (bswap)
	    bswap2 ((char *)&iis, (char *)&iis, sizeof(iis));

	/* Verify the checksum.  If it fails swap the bytes and try again.
	 */
	for (;;) {
	    for (i=0, sum=0, p=(short *)&iis;  i < 8;  i++)
		sum += *p++;
	    if ((sum & 0177777) == 0177777)
		break;

	    if (ntrys++) {
		if (!errmsg++) {
		    fprintf (stderr, "imtool: bad data header checksum\n");
		    if (bswap)
			bswap2 ((char *)&iis, (char *)&iis, sizeof(iis));
		    fprintf (stderr, "noswap:");
		    for (i=0, p=(short *)&iis;  i < 8;  i++)
			fprintf (stderr, " %6o", p[i]);
		    fprintf (stderr, "\n");

		    bswap2 ((char *)&iis, (char *)&iis, sizeof(iis));
		    fprintf (stderr, "  swap:");
		    for (i=0, p=(short *)&iis;  i < 8;  i++)
			fprintf (stderr, " %6o", p[i]);
		    fprintf (stderr, "\n");
		}
		break;

	    } else {
		bswap2 ((char *)&iis, (char *)&iis, sizeof(iis));
		bswap = !bswap;
	    }
	}

	ndatabytes = -iis.thingct;
	if (!(iis.tid & PACKED))
	    ndatabytes *= 2;

	switch (iis.subunit & 077) {
	case FEEDBACK:
	    /* The feedback unit is used only to clear a frame.
	     */
	    xim_setReferenceFrame (chan, decode_frameno (iis.z & 07777));
	    if (decode_frameno (iis.z & 07777) == chan->reference_frame)
	      xim_eraseFrame (xim, chan->reference_frame);
	    break;

	case LUT:
	    /* Data mode writes to the frame lookup tables are not implemented.
	     * A command mode write to the LUT subunit is used to connect
	     * image memories up to the RGB channels, i.e., to select the frame
	     * to be displayed.  We ignore any attempt to assign multiple
	     * frames to multiple color channels, and just do a simple frame
	     * select.
	     */
	    if (iis.subunit & COMMAND) {
		int	frame, z, n;
		short	x[14];

		if (read (datain, (char *)x, ndatabytes) == ndatabytes) {
		    if (bswap)
			bswap2 ((char *)x, (char *)x, ndatabytes);

		    z = x[0];
		    if (!z) z = 1;
		    for (n=0;  !(z & 1);  z >>= 1)
			n++;

		    frame = max (1, n + 1);

		    xim_setDisplayFrame (xim, frame);
		    return 1;
		}
	    }

	case MEMORY:
	    /* Load data into the frame buffer.  Data is assumed to be byte
	     * packed.
	     */
	    if (iis.tid & IIS_READ) {
		/* Read from the display.
		 */
		unsigned char *ip, iobuf[SZ_IOBUF];
		int     nbytes, nleft, n, x, y;
		long    starttime;

		/* Get the frame to be read from. */
		xim_setReferenceFrame (chan, decode_frameno (iis.z & 07777));

		nbytes = ndatabytes;
		x = iis.x & XYMASK;
		y = iis.y & XYMASK;

		if (x < 0 || x >= chan->rf_p->width || y < 0 || y >= chan->rf_p->height) {
		    fprintf (stderr,
			"ximtool: buffer %dx%d\n", chan->rf_p->width, chan->rf_p->height);
		    fprintf (stderr,
			"ximtool: attempted read out of bounds on framebuf\n");
		    fprintf (stderr,
			"read %d bytes at [%d,%d]\n", nbytes, x, y);
		    memset ((void *)iobuf, 0, nbytes);
		} else {
		    xim_ReadPixels (xim, chan->reference_frame, iobuf, 8, x, y,
			  min(chan->rf_p->width-x,nbytes)
			, max(1,nbytes/chan->rf_p->width));
		}

		/* Return the data from the frame buffer. */
		starttime = time(0);
		for (nleft=nbytes, ip=iobuf;  nleft > 0;  nleft -= n) {
		    n = (nleft < SZ_FIFOBUF) ? nleft : SZ_FIFOBUF;
		    if ((n = write (dataout, ip, n)) <= 0) {
			if (n < 0 || (time(0) - starttime > IO_TIMEOUT)) {
			    fprintf (stderr, "IMTOOL: timeout on write\n");
			    break;
			}
		    } else
			ip += n;
		}

		return 1;

	    } else {
		/* Write to the display.
		 */
		unsigned char *op, iobuf[SZ_IOBUF];
		int     nbytes, nleft, n, x, y;
		long    starttime;

		/* Get the frame to be written into (encoded with a bit for
		 * each frame, 01 is frame 1, 02 is frame 2, 04 is frame 3,
		 * and so on).
		 */
		xim_setReferenceFrame (chan, decode_frameno (iis.z & 07777));

		nbytes = ndatabytes;
		x = iis.x & XYMASK;
		y = iis.y & XYMASK;

		/* Read the data into the frame buffer.
		 */
		starttime = time(0);
		for (nleft=nbytes, op=iobuf;  nleft > 0;  nleft -= n) {
		    n = (nleft < SZ_FIFOBUF) ? nleft : SZ_FIFOBUF;
		    if ((n = read (datain, op, n)) <= 0) {
			if (n < 0 || (time(0) - starttime > IO_TIMEOUT))
			    break;
		    } else
			op += n;
		}

		if (x < 0 || x >= chan->rf_p->width || y < 0 || y >= chan->rf_p->height) {
		    fprintf (stderr,
			"ximtool: buffer %dx%d\n", chan->rf_p->width, chan->rf_p->height);
		    fprintf (stderr,
			"ximtool: attempted write out of bounds on framebuf\n");
		    fprintf (stderr,
			"write %d bytes at [%d,%d]\n", nbytes, x, y);
		    memset ((void *)iobuf, 0, nbytes);
		} else {
		    xim_WritePixels (xim, chan->reference_frame, iobuf, 8, x, y,
			  min(chan->rf_p->width-x,nbytes)
			, max(1,nbytes/chan->rf_p->width));
		}
		return 1;
	    }
	    break;

	case WCS:
	    /* Read or write the WCS for a frame.  The frame number to
	     * which the WCS applies is passed in Z and the frame buffer
	     * configuration in T.  The client changes the frame buffer
	     * configuration in a WCS set.  The WCS text follows the header
	     * as byte packed ASCII data.
	     */
	    if (iis.tid & IIS_READ) {
		/* Return the WCS for the referenced frame.
		 */
		char emsg[SZ_FNAME];
		char *text;
		int frame;

		frame = decode_frameno (iis.z & 07777);
		xim_setReferenceFrame (chan, frame);
		xim_getWCS (chan, frame);

		if (chan->rf_p->frameno <= 0)
		    strcpy (text=emsg, "[NOSUCHFRAME]\n");
		else
		    text = chan->rf_p->wcsbuf;

		write (dataout, text, SZ_WCSBUF);

	    } else {
		/* Set the WCS for the referenced frame.
		 */
		register CtranPtr ct;
		int fb_config, frame;

		frame = decode_frameno (iis.z & 07777);
		fb_config = (iis.t & 0777) + 1;

		/* Read in and set up the WCS. */
		/* xim_setReferenceFrame (chan, frame); */

		if (read (datain, buf, ndatabytes) == ndatabytes)
		    strncpy (chan->rf_p->wcsbuf, buf, SZ_WCSBUF);

		xim_setWCS (chan, frame);

		strcpy (chan->rf_p->ctran.format, W_DEFFORMAT);
		chan->rf_p->ctran.imtitle[0] = '\0';
		chan->rf_p->ctran.valid = 0;

		/* xim_setFrameTitle (xim, frame, ct->imtitle); */
	    }
	    return 1;

	case IMCURSOR:
	    /* Read or write the logical image cursor.  This is an extension
	     * added to provide a high level cursor read facility; this is
	     * not the same as a low level access to the IIS cursor subunit.
	     * Cursor reads may be either nonblocking (immediate) or blocking,
	     * using the keyboard or mouse to terminate the read, and
	     * coordinates may be returned in either image (world) or frame
	     * buffer pixel coordinates.
	     */
	    if (iis.tid & IIS_READ) {
		/* Read the logical image cursor.  In the case of a blocking
		 * read all we do is initiate a cursor read; completion occurs
		 * when the user hits a key or button.
		 */
		if (iis.tid & IMC_SAMPLE) {
		    /* Sample the cursor position and return the cursor value
		     * on the output datastream encoded in a fixed size
		     * ascii buffer.
		     */
		    int wcs = iis.z;
		    int raster, frame;
		    float sx, sy;
		    IoChanPtr sv_chan;

		    /* Save the cursor channel so that sampled cursor reads
		     * can occur on one channel without affecting any
		     * interactive cursor reads in progress on another
		     * channel.
		     */
		    sv_chan = xim->cursor_chan;
		    xim->cursor_chan = chan;
		    xim_getCursorPos (xim, chan->reference_frame, &sx, &sy);
		    xim_retCursorVal (xim, sx, sy, frame, wcs, 0, "");
		    xim->cursor_chan = sv_chan;

		} else {
		    /* Initiate a user triggered cursor read. */
		    /*
		    if (xim->cursor_chan) {
			int frame = xim->cursor_chan->reference_frame;
			xim_retCursorVal (xim, 0., 0., frame, 0, EOF, "");
		    }
			*/
		    xim->cursor_chan = chan;
		    xim_cursorMode (xim, chan->reference_frame, 1);
		}

	    } else {
		/* Write (set) the logical image cursor position. */
		register CtranPtr ct;
		int sx = iis.x, sy = iis.y;
		float wx = sx, wy = sy;
		int wcs = iis.z;

		xim_setCursorPos (xim, chan->reference_frame, sx, sy);
	    }
	    return 1;

	default:
	    /* Ignore unsupported command input.
	     */
	    break;
	}

	/* Discard any data following the header. */
	if (!(iis.tid & IIS_READ))
	    for (nbytes = ndatabytes;  nbytes > 0;  nbytes -= n) {
		n = (nbytes < SZ_FIFOBUF) ? nbytes : SZ_FIFOBUF;
		if ((n = read (datain, buf, n)) <= 0)
		    break;
	    }

	return 1;
}




/* DECODE_FRAMENO -- Decode encoded IIS register frame number.
 */
static
decode_frameno (z)
register int	z;
{
	register int	n;

	/* Get the frame number, encoded with a bit for each frame, 01 is
	 * frame 1, 02 is frame 2, 04 is frame 3, and so on.
	 */
	if (!z) z = 1;
	for (n=0;  !(z & 1);  z >>= 1)
	    n++;

	return (max (1, n + 1));
}


/* BSWAP2 - Move bytes from array "a" to array "b", swapping successive
 * pairs of bytes.  The two arrays may be the same but may not be offset
 * and overlapping.
 */
static
bswap2 (a, b, nbytes)
char 	*a, *b;		/* input array			*/
int	nbytes;		/* number of bytes to swap	*/
{
	register char *ip=a, *op=b, *otop;
	register unsigned temp;

	/* Swap successive pairs of bytes.
	 */
	for (otop = op + (nbytes & ~1);  op < otop;  ) {
	    temp  = *ip++;
	    *op++ = *ip++;
	    *op++ = temp;
	}

	/* If there is an odd byte left, move it to the output array.
	 */
	if (nbytes & 1)
	    *op = *ip;
}


/* XIM_RETCURSORVAL -- Return the cursor value on the output datastream to
 * the client which requested the cursor read.
 */
xim_retCursorVal (xim, sx, sy, frame, wcs, key, strval)
register XimDataPtr xim;
float	sx, sy;			/* cursor screen coordinates */
int	frame;			/* frame number */
int	wcs;			/* nonzero if WCS coords desired */
int	key;			/* keystroke used as trigger */
char	*strval;		/* optional string value */
{
	register CtranPtr ct;
	int dataout, wcscode;
	char curval[SZ_IMCURVAL];
	char keystr[20];
	float wx, wy;

	if (xim->cursor_chan)
	    dataout = xim->cursor_chan->dataout;
	else
	    return;

	    wx = sx;
	    wy = sy;

	/* Compute WCS code. */
	wcscode = frame * 100 + wcs;

	/* Encode the cursor value. */
	if (key == EOF)
	    sprintf (curval, "EOF\n");
	else {
	    if (isprint (key) && !isspace(key)) {
		keystr[0] = key;
		keystr[1] = '\0';
	    } else
		sprintf (keystr, "\\%03o", key);

	    sprintf (curval, "%10.3f %10.3f %d %s %s\n",
		wx, wy, wcscode, keystr, strval);
	}

	/* Send it to the client program and terminate cursor mode. */
	write (dataout, curval, sizeof(curval));
	xim_cursorMode (xim, frame, 0);
	xim->cursor_chan = NULL;
}


