/* filelist.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */



#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#if HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#if HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#include "xos.h"
#include "xlist.h"
#include "filelist.h"

filelist FileList(path, patt)
			char	*path;
			char	*patt;
{
		char	pathbuff[1024];

		char 	*here, *getenv();

		filelist	list = NULL;
		filelist	l;
		filelist	FileList1Dir();

	if ( path == NULL ) return NULL;

	strcpy(pathbuff, path);
	path = pathbuff;

	if ( (here = strpbrk(pathbuff, " :")) ) *here++ = '\0';

	while ( path ) {
	    if ( path[0] == '$' ) {
		path = getenv(&path[1]);
		if ( l = FileList(path, patt) ) {
		    list = ListAdd(list, l);
		}
	    } else {
		if ( l = FileList1Dir(path, patt) ) {
		    list = ListAdd(list, l);
		}

	    }

	    path = here;
	    if ( here && (here = strpbrk(here, " :")) ) *here++ = '\0';
	}

	return (filelist) list;
}


filelist FileList1Dir(path, patt)
			char	*path;
			char	*patt;
{
	DIR *dirp, *opendir();
	struct dirent *direntp, *readdir();

	filelist list = NULL;

	if ( path == NULL ) return NULL;

	if ( (dirp = opendir(path)) == NULL ) return NULL ;

	while( (direntp = readdir(dirp)) != NULL ) {
		if ( tmatch(direntp->d_name, patt) ) {
				struct _filelist *entry;

			Malloc(entry, sizeof(struct _filelist));
			entry->next = NULL;
			entry->mark = 0;

			NewString(entry->path, path);
			NewString(entry->name, direntp->d_name);

			list = ListAdd(list, entry);
		}
	}

	closedir(dirp);

	return list;
}


void FileListFree(list)
			filelist list;
{
		void	*prev = NULL;

	for ( ; list; list = ListNext(list) ) {
		if ( prev ) Free(prev);

		Free(list->path);
		Free(list->name);

		prev = list;
	}

	if ( prev ) Free(prev);
}


