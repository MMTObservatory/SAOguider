
#include "xos.h"
#include "xfile.h"

char *readfile(file)
			char	*file;
{
		char	*text = NULL;
		off_t	 size;
		File	 fd;


	if ( ( fd = Open(file, "r")) == NULL ) {
	    	return NULL;
	}
	if ( size = filesize(fd) ) {
		Malloc(text, size + 1);
		if ( Read(fd, text, 1, size) != size ) {
	    		return NULL;
		}

		text[size] = '\0';
	}

	Close(fd);

	return text;
}

off_t filesize(file)
			File	file;
{
    off_t here = Seek(file, 0, SEEK_CUR);
    off_t size;

    Seek(file, 0,    SEEK_END);
    size = Tell(file);
    Seek(file, here, SEEK_SET);

    return size;
}	
