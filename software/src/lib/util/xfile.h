/* xfile.h
 */

#ifndef XFILE_H
#define XFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

typedef FILE	*File;

char *readfile();
off_t filesize();

#if _FILE_OFFSET_BITS == 64
#define OffFormat	"%lld"
#else
#define OffFormat	"%ld"
#endif

typedef struct _xFileClass {
	ifunct	write;
	ifunct	read;
	ofunct	seek;
	ofunct	tell;
	ifunct flush;
	ifunct close;
	ifunct	getc;
	ifunct	putc;
	ifunct	ungetc;
	ifunct print;
	ifunct	setbuffer;
	ifunct left;
	void	*data;
} *xFileClass;

typedef struct _xFileType {
	File		 fp;
	xFileClass	 fclass;
	void		*data;
} *xFileType;

#define NXFiles	256

xFileType xFiles[NXFiles];

#define xFile(f)		( xFiles[FileFd(f)] )
#define xFunc(f, func)		( xFile(f)->fclass->func )
#define xChek(f, func)		( xFile(f) && xFile(f)->fclass && xFunc(f, func) )
#define xFileInit(f, c)	{						\
			    Malloc(xFile(f), sizeof(struct _xFileType));\
			    xFile(f)->fp     = f;			\
			    xFile(f)->fclass = c;			\
			    xFile(f)->data   = NULL;			\
			}
#define xFileDone(f)		Free(xFile(f))

#define Open(f, m)		xOpen(f, m)
#define FileFd(f)		fileno(f)
#define OpenFd(fd, mode)	fdopen(fd, mode)

#define Write(f, b, s, n)	( xChek(f, write) 			\
					? xFunc(f, write)(f, b, s, n)	\
					: fwrite(b, s, n, f) )
#define Read(f, b, s, n)	( xChek(f, read)			\
					? xFunc(f, read)(f, b, s, n)	\
					: fread(b, s, n, f) )
#define Flush(f)		( xChek(f, flush)			\
					? xFunc(f, flush)(f)		\
					: fflush(f) )
#define Seek(f, o, n)		( xChek(f, seek)			\
					? xFunc(f, seek)(f, o, n)	\
					: (fseeko(f, o, n), ftello(f)) )
#define Tell(f)			( xChek(f, tell)			\
					? xFunc(f, tell)(f)		\
					: ftello(f) )
#define Close(f)		( xChek(f, close)			\
					? xFunc(f, close)(f)		\
					: fclose(f) )
#define GetC(f)			( xChek(f, getc)			\
					? xFunc(f, getc)(f)		\
					: getc(f) )
#define PutC(f, c)		( xChek(f, putc)			\
					? xFunc(f, putc)(f, c)		\
					: putc(c, f) )
#define UnGetC(f, c)		( xChek(f, ungetc)			\
					? xFunc(f, ungetc)(f, c)	\
					: ungetc(c, f) )
#define SetBuffer(f, t, b, s)	( xChek(f, setbuffer)			\
					? xFunc(f, setbuffer)(f, t, b, s)\
					: setvbuf(f, b, t, s) )

#define ReadLine(f, buffer, n)	( (fgets(buffer, n, f) == NULL)	\
					? ( feof(f) ? 0 : -1 )	\
					: strlen(buffer) )

#define XFREAD	 1
#define XFWRITE	 2
#define XFCREAT	 4
#define XFMULTI	32

#define FileLineBuffer	_IOFBF
#define FileFullBuffer	_IOLBF
#define FileNullBuffer	_IONBF

#define Stdin	stdin
#define Stdout	stdout
#define Stderr	stderr

#define  Print	 printf
#define FPrint	fprintf
#define FPuts(file, string)	fputs(string, file)

typedef struct timeval Timeval;

/* Multiplex IO query structure
**/

typedef struct _Polls {
    File            f;
    int             qmode;
    int             rmode;
}               Polls;

typedef struct stat FileInfo;

#define Stat(file, info)		 stat(file, info)
#define FileStat(f, info)		fstat(FileFd(f), info)
#define FileInfoSize(info)	info.st_size
#define FileInfoMode(info)	info.st_mode
#define FileInfoUser(info)	info.st_uid
#define FileInfoGrop(info)	info.st_gid

#define PERM_READ	4
#define PERM_WRIT	2
#define PERM_EXEC	1

File XFile();

#ifdef __STDC__
File Accept(File file, char *mode, char **name);
File SokOpen(char *name, int port, int mode);
char *FindFile(char *, char *, char *, char *, struct stat *);
File xOpen(char *name, char *mode);
#else
File Accept();
File SokOpen();
char *FindFile();
File xOpen();
#endif

#endif
