/* xfile pipe class
 */

#include "xos.h"
#include "xfile.h"

xfile_pclose(f)
	File	f;
{
	xFileDone(f);
	return pclose(f);
}

struct _xFileClass xFilePipe = {
	/* ivector write;	*/	  NULL
	/* ivector read;	*/	, NULL
	/* ivector seek;	*/	, NULL
	/* ivector tell;	*/	, NULL
	/* ivector flush;	*/	, NULL
	/* ivector close;	*/	, xfile_pclose
	/* ivector getc;	*/	, NULL
	/* ivector putc;	*/	, NULL
	/* ivector ungetc;	*/	, NULL
	/* ivector print;	*/	, NULL
	/* ivector setbuffer;	*/	, NULL
	/* ivector left;	*/	, NULL
	/* void	 *data;		*/	, NULL
};


