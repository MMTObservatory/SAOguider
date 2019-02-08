/* uclxfile.c
**/

/*
 *      Copyrights:
 *
 *    	      Copyright (c) 1996 Smithsonian Astrophysical Observatory
 *
 *        Permission to use, copy, modify, distribute, and  sell  this
 *        software  and  its  documentation  for any purpose is hereby
 *        granted without  fee,  provided  that  the  above  copyright
 *        notice  appear  in  all  copies and that both that copyright
 *        notice and this permission notice appear in supporting docu-
 *        mentation,  and  that  the  name  of the  Smithsonian Astro-
 *	  physical Observatory not be used in advertising or publicity
 *  	  pertaining to distribution of the software without specific,
 *	  written  prior  permission.   The Smithsonian  Astrophysical 
 *        Observatory makes no representations about  the  suitability
 *	  of  this  software for any purpose.  It is provided  "as is"
 *	  without express or implied warranty.
 *        THE  SMITHSONIAN  ASTROPHYSICAL  OBSERVATORY  DISCLAIMS  ALL
 *	  WARRANTIES  WITH  REGARD  TO  THIS  SOFTWARE,  INCLUDING ALL
 *	  IMPLIED  WARRANTIES  OF  MERCHANTABILITY AND FITNESS, IN  NO
 *	  EVENT SHALL THE  SMITHSONIAN  ASTROPHYSICAL  OBSERVATORY  BE 
 *	  LIABLE FOR  ANY SPECIAL, INDIRECT  OR  CONSEQUENTIAL DAMAGES
 *	  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS  OF USE,  DATA
 *	  OR  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 *	  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 *	  THE  USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdlib.h>

#include "pfile.h"
#include "ptemplat.h"
#include "parameter.h"

extern char *strf2c();

OpenDefaultPF(argc, argv)
			int	 argc;
			char	*argv[];
{
		paramfile	pfile;

	if ( !(pfile = c_paramopen(NULL, argv, argc, "rw")) )
		paramerr(1, argv[0], "");

#ifdef vms
	uclxfile(&pfile);
#else
	uclxfile_(&pfile);
#endif
}


CloseDefaultPF()
{
#ifdef vms
	uclxdone();
#else
	uclxdone_();
#endif
}


#define	ER_EOF                 -2  /* End-of-file indicator; same as IRAF */
#define	ER_OK                   0  /* Successful return */

/* Terminal I/O status messages : codes 30-39 */

#define	ER_UPTOUT              	30  /* Error writing to STDOUT */
#define	ER_UPTLOG              	31  /* Error writing to the log file */
#define	ER_UPTBADDEST          	32  /* Invalid message destination */
#define	ER_UPTBADPRIO          	33  /* Invalid message priority */

#define	MSG_NOP			 0		/* No output (for UMSPUT) */
#define MSG_MAXPRIO		10		/* Max priority for output */

#ifdef vms

#include descrip.h

typedef struct dsc$descriptor descriptor;

#define DataLength(x)	x->dsc$w_length
#define DataAddrss(x)	x->dsc$a_pointer

umsput(message_d, dest, priority, istat)
			descriptor	*message_d;
			int  		*dest;
			int  		*priority;
			int		*istat;
{
		char	*message  = DataAddrss(message_d);
		int	 nmessage = DataLength(message_d);

  int msg_stdout, msg_stderr;
  char *text;

  if (*dest < 0  || *dest > 15) {
    *istat = ER_UPTBADDEST;
    return;
  }
  if ( *priority < 0  || *priority > MSG_MAXPRIO )  {
    *istat = ER_UPTBADDEST;
    return;
  }

  istat = ER_OK;
  if (*dest == MSG_NOP) 
    return;

  /*
    # Check where output has to go: 1 -> STDOUT, 2 -> STDERR,
    # 4 -> user logfile (if keeplog = yes), 8 -> system logfile
    # Get each bit in dest
  */
  msg_stdout = *dest & 1;
  msg_stderr = *dest & 2;

  /* convert string to string to null-terminated */
	text = (char *)malloc(nmessage+1);
	strf2c(nmessage, message, text);

	if ( msg_stdout )
		fprintf(stdout, "%s\n", text);

	if ( msg_stderr )
		fprintf(stderr, "%s\n", text);

	free(text);
}

#else

umsput_(message, dest, priority, istat, nmessage)
			char	*message;
			int 	*dest;
			int 	*priority;
			int	*istat;
			int	 nmessage;
{

		int	 msg_stdout;
		int	 msg_stderr;
		char	*text;

  if (*dest < 0  || *dest > 15) {
    *istat = ER_UPTBADDEST;
    return;
  }
  if ( *priority < 0  || *priority > MSG_MAXPRIO )  {
    *istat = ER_UPTBADDEST;
    return;
  }

  istat = ER_OK;
  if (*dest == MSG_NOP) 
    return;

  /*
    # Check where output has to go: 1 -> STDOUT, 2 -> STDERR,
    # 4 -> user logfile (if keeplog = yes), 8 -> system logfile
    # Get each bit in dest
  */
  msg_stdout = *dest & 1;
  msg_stderr = *dest & 2;

  /* convert string to string to null-terminated */
	text = (char *)malloc(nmessage+1);
	strf2c(nmessage, message, text);

	if ( msg_stdout )
		fprintf(stdout, "%s\n", text);

	if ( msg_stderr )
		fprintf(stderr, "%s\n", text);

	free(text);
}

#endif
