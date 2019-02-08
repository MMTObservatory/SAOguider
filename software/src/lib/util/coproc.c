/* coprocess.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int coprocess(command, argv, in, out, err)
	char	*command;
	char	*argv[];
	int	*in, *out, *err;
{
	int	child;
	int	p1[2];
	int	p2[2];
	int	p3[2];

	if ( in  != NULL ) pipe(p1);
	if ( out != NULL ) pipe(p2);
	if ( err != NULL ) pipe(p3);

	if ( (child = vfork()) == 0 ) {
		if ( in  != NULL ) close(p1[1]);
		if ( out != NULL ) close(p2[0]);
		if ( err != NULL ) close(p3[0]);

		if ( in  != NULL ) dup2(p1[0], 0);
		if ( out != NULL ) dup2(p2[1], 1);
		if ( err != NULL ) dup2(p3[1], 2);

		if ( in  != NULL ) close(p1[0]);
		if ( out != NULL ) close(p2[1]);
		if ( err != NULL ) close(p3[1]);
		execvp(command, argv);

		perror("coprocess returns from exec");
		_exit(1);
	}

	if ( in  != NULL ) close(p1[0]);
	if ( out != NULL ) close(p2[1]);
	if ( err != NULL ) close(p3[1]);

	if ( in  != NULL ) *in = p1[1];
	if ( out != NULL ) *out= p2[0];
        if ( err != NULL ) *err= p3[0];

	return child;
}

int coroutine(routine, data)
	void	(*routine)();
	void	 *data;
{
	int	pid;

    if ( (pid = fork()) == 0 ) {
	    (*routine)(data);
	    exit(0);
    }

    return pid;
}

int shellproc(command, in, out, err)
	char    *command;
	int     *in, *out, *err;
{
	char	*argv[10];

	argv[0] = "sh";
	argv[1] = "-c";
	argv[2] = command;
	argv[3] = (char *) 0;

	return coprocess("sh", argv, in, out, err);
}

