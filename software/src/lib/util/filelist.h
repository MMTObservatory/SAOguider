/* filelist.h
**/

typedef struct	_filelist {
		xList	*next;
		char	*name;
		char	*path;
		int	 mark;
} *filelist;

filelist FileList();
