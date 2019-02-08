/* Termcap
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */


#include <stdio.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include <stdio.h>

char *getenv();

static	char	*ROWS = NULL;
static	char	*COLS = NULL;
static	char	*MOVE = NULL;
static	char	*CLER = NULL;
static	char	*POPC = NULL;
static	char	*PSHC = NULL;
static	char	*SO   = NULL;
static	char	*SE   = NULL;

static	FILE *tty;

void Termcap()
{
	tty = fopen("/dev/tty", "w");

	if ( ROWS == NULL ) if ( (ROWS = getenv("ROWS")) == NULL ) ROWS = "24";
	if ( COLS == NULL ) if ( (COLS = getenv("COLS")) == NULL ) COLS = "80";

	if ( MOVE == NULL ) MOVE="\033[%02d;%02dH";
	if ( CLER == NULL ) CLER="\033[H\033[2J";
	if ( PSHC == NULL ) PSHC="\0337";
	if ( POPC == NULL ) POPC="\0338";
	if ( SO   == NULL ) SO="\033[7";
	if ( SE   == NULL ) SE="\033[7";

}

void TC_done() 
{
	if ( tty ) fclose(tty);
}

void TC_flush()
{
	if ( !tty ) return;

	fflush(tty);
}

void TC_clear()
{
	if ( !tty ) return;

	fprintf(tty, CLER);
}

void TC_move(col, row)
	int	col, row;
{
	if ( !tty ) return;

	fprintf(tty, MOVE, row, col);
}

void TC_pshcur()
{
	if ( !tty ) return;

	fprintf(tty, PSHC);
}

void TC_popcur()
{
	if ( !tty ) return;

	fprintf(tty, POPC);
}

void TC_standout()
{
	if ( !tty ) return;

	fprintf(tty, SO);
}

void TC_normal()
{
	if ( !tty ) return;

	fprintf(tty, SE);
}

#ifdef __STDC__
void TC_print(char *format, ...)
{
    va_list         args;
    va_start(args, format);
#else
void TC_print(va_alist) va_dcl
{
    va_list         args;

    int		 x;
    int		 y;
    char	*format;
    va_start(args);
    format = va_arg(args, char *);
#endif

    if ( !tty ) return;

    vfprintf(tty, format, args);
}

#ifdef __STDC__
void TC_at(int x, int y, char *format, ...)
{
    va_list         args;
    va_start(args, format);
#else
void TC_at(va_alist) va_dcl
{
    va_list         args;

    int		 x;
    int		 y;
    char	*format;
    va_start(args);
    x	   = va_arg(args, int);
    y	   = va_arg(args, int);
    format = va_arg(args, char *);
#endif

    if ( !tty ) return;

    TC_pshcur();
    TC_move(x, y);
    vfprintf(tty, format, args);
    TC_popcur();
}

