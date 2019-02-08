
/* NANO-EMACS line editing facility */
/* printable characters print as themselves (insert not overwrite) */
/* ^A moves to the beginning of the line */
/* ^B moves back a single character */
/* ^E moves to the end of the line */
/* ^F moves forward a single character */
/* ^K kills from current position to the end of line */
/* ^P moves back through history */
/* ^N moves forward through history */
/* ^H and DEL delete the previous character */
/* ^D deletes the current character, or EOF if line is empty */
/* ^L/^R redraw line in case it gets trashed */
/* ^U kills the entire line */
/* ^W kills last word */
/* LF and CR return the entire line regardless of the cursor postition */
/* EOF with an empty line returns (char *)NULL */

/* all other characters are ignored */

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#ifdef NeXT
#define READLINE 0
#endif

#ifdef vax
#define READLINE 0
#define MAX_LINE 4096

#include descrip.h
#include ctype.h

int readline(prompt, line, max)
			char	*prompt;
			char	*line;
			int	 max;
{
		char	buffer[MAX_LINE];
		int	length = max;
		int	one = 1;
		int	i;

		struct dsc$descriptor_s line_d = {
        	        MAX_LINE, DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer
	        };
		struct dsc$descriptor_s prompt_d = {
        	        MAX_LINE, DSC$K_DTYPE_T, DSC$K_CLASS_S, buffer
	        };

	prompt_d.dsc$w_length  = strlen(prompt);
	prompt_d.dsc$a_pointer = prompt;
	
	lib$get_foreign(&line_d, &prompt_d, &length, &one);
	buffer[length] = '\0';

	for ( i = 0; i < length; i++ )
		if ( isupper(buffer[i]) ) buffer[i] = tolower(buffer[i]);

	strncpy(line, buffer, max);
	line[length] = '\0';
	
	return length;
}
#endif

#ifndef READLINE
#define READLINE 1
#endif

#if  READLINE

#define EOF (-1)

#include <ctype.h>
#include <signal.h>

#ifndef MSDOS

/* SIGTSTP defines job control */
/* if there is job control then we need termios.h instead of termio.h */
#ifdef SIGTSTP
#define TERMIOS
#endif

/* hacks to make BSDI work */
#ifdef __386BSD__
#define _POSIX_SOURCE 1
#define TERMIOS 1
#endif
#ifdef __FreeBSD__
#define _POSIX_SOURCE 1
#define TERMIOS 1
#endif    

#ifdef TERMIOS
#include <termios.h>
static struct termios orig_termio, rl_termio;
#else
#include <termio.h>
static struct termio orig_termio, rl_termio;
#endif /* TERMIOS */
static int term_set = 0;	/* =1 if rl_termio set */

#else
/* MSDOS specific stuff */
#define getc(stdin) msdos_getch()
static char msdos_getch();

#endif /* MSDOS */

#include <stdlib.h>
#include <string.h>

#define MAXBUF	1024
#define BACKSPACE 0x08	/* ^H */
#define SPACE	' '

#include "history.h"

static int cur_pos = 0;	/* current position of the cursor */
static int max_pos = 0;	/* maximum character position */

void *kil_history();
void *add_history();

static void fix_line();
static void redraw_line();
static void clear_line();
static void clear_eoline();
static void copy_line();

extern void set_termio();
extern void reset_termio();

int
readline(prompt, cur_line, max, history)
	char 	*prompt;
	char 	*cur_line;
	int	 max;
	struct hist *history;
{

		struct hist *cur_entry = NULL;
		char cur_char;

		FILE *f;

	if ( (f = fopen("/dev/tty", "r")) == NULL )
		return -1;
	
	/* set the termio so we can do our own input processing */
	set_termio(fileno(f));

	/* print the prompt */
	fputs(prompt, stderr);
	cur_line[0] = '\0';
	cur_pos = 0;
	max_pos = 0;

	/* get characters */
	for(;;) {
		fread(&cur_char, 1, 1, f);

		if(isprint(cur_char)) {
			int i;
			for(i=max_pos; i>cur_pos; i--) {
				cur_line[i] = cur_line[i-1];
			}
			putc(cur_char, stderr);
			cur_line[cur_pos] = cur_char;
			cur_pos += 1;
			max_pos += 1;
			if (cur_pos < max_pos)
			    fix_line(cur_line);
			cur_line[max_pos] = '\0';

		/* else interpret unix terminal driver characters */
#ifdef VERASE
		} else if(cur_char == orig_termio.c_cc[VERASE] ){  /* DEL? */
			if(cur_pos > 0) {
				int i;
				cur_pos -= 1;
				putc(BACKSPACE, stderr);
				for(i=cur_pos; i<max_pos; i++)
					cur_line[i] = cur_line[i+1];
				max_pos -= 1;
				fix_line(cur_line);
			}
#endif /* VERASE */
#ifdef VEOF
		} else if(cur_char == orig_termio.c_cc[VEOF] ){   /* ^D? */
			if(max_pos == 0) {
				reset_termio(fileno(f));
				fclose(f);
				return 0;
			}
			if((cur_pos < max_pos)&&(cur_char == 004)) { /* ^D */
				int i;
				for(i=cur_pos; i<max_pos; i++)
					cur_line[i] = cur_line[i+1];
				max_pos -= 1;
				fix_line(cur_line);
			}
#endif /* VEOF */
#ifdef VKILL
		} else if(cur_char == orig_termio.c_cc[VKILL] ){  /* ^U? */
			clear_line(prompt, cur_line);
#endif /* VKILL */
#ifdef VWERASE
		} else if(cur_char == orig_termio.c_cc[VWERASE] ){  /* ^W? */
			while((cur_pos > 0) &&
			      (cur_line[cur_pos-1] == SPACE)) {
				cur_pos -= 1;
				putc(BACKSPACE, stderr);
			}
			while((cur_pos > 0) &&
			      (cur_line[cur_pos-1] != SPACE)) {
				cur_pos -= 1;
				putc(BACKSPACE, stderr);
			}
			clear_eoline(prompt, cur_line);
			max_pos = cur_pos;
#endif /* VWERASE */
#ifdef VREPRINT
		} else if(cur_char == orig_termio.c_cc[VREPRINT] ){  /* ^R? */
			putc('\n',stderr); /* go to a fresh line */
			redraw_line(prompt, cur_line);
#else
#ifdef VRPRNT   /* on Ultrix VREPRINT is VRPRNT */
		} else if(cur_char == orig_termio.c_cc[VRPRNT] ){  /* ^R? */
			putc('\n',stderr); /* go to a fresh line */
			redraw_line(prompt, cur_line);
#endif /* VRPRNT */
#endif /* VREPRINT */

#ifdef VINTR
		} else if(cur_char == orig_termio.c_cc[VINTR]) {
			reset_termio(fileno(f));
			kill(0, SIGINT);

			exit(1);
#endif /* VINTR */
		/* added by EGM and JR to fix Solaris */	
		} else if(cur_char == '\003') {
			reset_termio(fileno(f));
			kill(0, SIGINT);

			exit(1);
#ifdef TERMIOS
		} else if(cur_char == orig_termio.c_cc[VSUSP]) {
			reset_termio(fileno(f));
			kill(0, SIGTSTP);

			/* process stops here */

			set_termio(fileno(f));
			/* print the prompt */
			redraw_line(prompt, cur_line);
#endif /* VSUSP */
		} else {
			/* do normal editing commands */
			/* some of these are also done above */
			int i;
			switch(cur_char) {
			    case EOF:
				reset_termio(fileno(f));
				fclose(f);
				return 0;
			    case 001: /* ^A */
				while(cur_pos > 0) {
					cur_pos -= 1;
					putc(BACKSPACE, stderr);
				}
				break;
			    case 002: /* ^B */
				if(cur_pos > 0) {
					cur_pos -= 1;
					putc(BACKSPACE, stderr);
				}
				break;
			    case 005: /* ^E */
				while(cur_pos < max_pos) {
					putc(cur_line[cur_pos], stderr);
					cur_pos += 1;
				}
				break;
			    case 006: /* ^F */
				if(cur_pos < max_pos) {
					putc(cur_line[cur_pos], stderr);
					cur_pos += 1;
				}
				break;
			    case 013: /* ^K */
				clear_eoline(prompt, cur_line);
				max_pos = cur_pos;
				break;
			    case 020: /* ^P */
				if(history != NULL) {
					if(cur_entry == NULL) {
						cur_entry = history;
						clear_line(prompt, cur_line);
						copy_line(cur_entry->line, cur_line);
					} else if(cur_entry->prev != NULL) {
						cur_entry = cur_entry->prev;
						clear_line(prompt, cur_line);
						copy_line(cur_entry->line, cur_line);
					}
				}
				break;
			    case 016: /* ^N */
				if(cur_entry != NULL) {
					cur_entry = cur_entry->next;
					clear_line(prompt, cur_line);
					if(cur_entry != NULL) 
						copy_line(cur_entry->line, cur_line);
					else
						cur_pos = max_pos = 0;
				}
				break;
			    case 014: /* ^L */
			    case 022: /* ^R */
				putc('\n',stderr); /* go to a fresh line */
				redraw_line(prompt, cur_line);
				break;
			    case 0177: /* DEL */
			    case 010: /* ^H */
				if(cur_pos > 0) {
					cur_pos -= 1;
					putc(BACKSPACE, stderr);
					for(i=cur_pos; i<max_pos; i++)
						cur_line[i] = cur_line[i+1];
					max_pos -= 1;
					fix_line(cur_line);
				}
				break;
			    case 004: /* ^D */
				if(max_pos == 0) {
					reset_termio(fileno(f));
					fclose(f);
					return 0;
				}
				if(cur_pos < max_pos) {
					for(i=cur_pos; i<max_pos; i++)
						cur_line[i] = cur_line[i+1];
					max_pos -= 1;
					fix_line(cur_line);
				}
				break;
			    case 025:  /* ^U */
				clear_line(prompt, cur_line);
				break;
			    case 027:  /* ^W */
				while((cur_pos > 0) &&
				      (cur_line[cur_pos-1] == SPACE)) {
					cur_pos -= 1;
					putc(BACKSPACE, stderr);
				}
				while((cur_pos > 0) &&
				      (cur_line[cur_pos-1] != SPACE)) {
					cur_pos -= 1;
					putc(BACKSPACE, stderr);
				}
				clear_eoline(prompt, cur_line);
				max_pos = cur_pos;
				break;
			    case '\n': /* ^J */
			    case '\r': /* ^M */
				cur_line[max_pos+1] = '\0';
				putc('\n', stderr);
				reset_termio(fileno(f));
				fclose(f);
				return strlen(cur_line);
			    default:
				break;
			}
		}
	}
}

/* fix up the line from cur_pos to max_pos */
/* do not need any terminal capabilities except backspace,	*/
/* and space overwrites a character */
static void
fix_line(cur_line)
		char	*cur_line;
{
	int i;

	/* write tail of string */
	for(i=cur_pos; i<max_pos; i++)
		putc(cur_line[i], stderr);

	/* write a space at the end of the line in case we deleted one */
	putc(SPACE, stderr);

	/* backup to original position */
	for(i=max_pos+1; i>cur_pos; i--)
		putc(BACKSPACE, stderr);

}

/* redraw the entire line, putting the cursor where it belongs */
static void
redraw_line(prompt, cur_line)
     char *prompt;
     char *cur_line;
{
	int i;

	fputs(prompt, stderr);
	fputs(cur_line, stderr);

	/* put the cursor where it belongs */
	for(i=max_pos; i>cur_pos; i--)
		putc(BACKSPACE, stderr);
}

/* clear cur_line and the screen line */
static void
clear_line(prompt, cur_line)
     char *prompt;
     char *cur_line;
{
	int i;
	for(i=0; i<max_pos; i++)
		cur_line[i] = '\0';

	for(i=cur_pos; i>0; i--)
		putc(BACKSPACE, stderr);

	for(i=0; i<max_pos; i++)
		putc(SPACE, stderr);


	putc('\r', stderr);
	fputs(prompt, stderr);

	cur_pos = 0;
	max_pos = 0;
}

/* clear to end of line and the screen end of line */
static void
clear_eoline(prompt, cur_line)
char *prompt;
char *cur_line;
{
	int i;
	for(i=cur_pos; i<max_pos; i++)
		cur_line[i] = '\0';

	for(i=cur_pos; i<max_pos; i++)
		putc(SPACE, stderr);
	for(i=cur_pos; i<max_pos; i++)
		putc(BACKSPACE, stderr);
}

/* copy line to cur_line, draw it and set cur_pos and max_pos */
static void copy_line(line, cur_line)
     char *line;
     char *cur_line;
{
	strcpy(cur_line, line);
	fputs(cur_line, stderr);
	cur_pos = max_pos = strlen(cur_line);
}

/* add line to the history */
void *
add_history(history, line)
			struct hist *history;
			char *line;
{
	struct hist *entry;
	entry = (struct hist *)malloc(sizeof(struct hist));
	entry->line = (char *)malloc(strlen(line)+1);
	strcpy(entry->line, line);

	entry->prev = history;
	entry->next = NULL;
	if(history != NULL) {
		history->next = entry;
	}
	return entry;
}

void *
kil_history(history)
     struct hist *history;
{
  while ( history ) {
    struct hist *h = history->prev;

    free(history->line);
    free(history);

    history = h;
  }

  return NULL;
}


#ifdef MSDOS

/* Convert Arrow keystrokes to Control characters: */
static  char
msdos_getch()
{
    char c = getch();

    if (c == 0) {
	c = getch(); /* Get the extended code. */
	switch (c) {
	    case 75: /* Left Arrow. */
		c = 002;
		break;
	    case 77: /* Right Arrow. */
		c = 006;
		break;
	    case 72: /* Up Arrow. */
		c = 020;
		break;
	    case 80: /* Down Arrow. */
		c = 016;
		break;
	    case 115: /* Ctl Left Arrow. */
	    case 71: /* Home */
		c = 001;
		break;
	    case 116: /* Ctl Right Arrow. */
	    case 79: /* End */
		c = 005;
		break;
	    case 83: /* Delete */
		c = 004;
		break;
	    default:
		c = 0;
		break;
	}
    }
    else if (c == 033) { /* ESC */
	c = 025;
    }


    return c;
}

#endif /* MSDOS */

/*
 * setterm.c 
 */

/* UNIX specific stuff */

#ifdef SunOS_4
extern int ioctl(/* int fd, int request, void *arg */);
#endif


/* set termio so we can do our own input processing */
void  set_termio (fd)
     int fd;
{
#ifndef MSDOS
	if(term_set == 0) {
#ifdef TERMIOS
#ifdef TCGETS
		ioctl(fd, TCGETS, &orig_termio);
#else
		tcgetattr(fd, &orig_termio);
#endif /* TCGETS */
#else
		ioctl(fd, TCGETA, &orig_termio);
#endif /* TERMIOS */
		rl_termio = orig_termio;

#ifdef _POSIX_SOURCE
		rl_termio.c_iflag &= ~(BRKINT|PARMRK|INPCK|IXON|IXOFF);
#else
		rl_termio.c_iflag &= ~(BRKINT|PARMRK|INPCK|IUCLC|IXON|IXOFF);
#endif
		rl_termio.c_iflag |=  (IGNBRK|IGNPAR);

#ifndef _POSIX_SOURCE
		rl_termio.c_oflag &= ~(ONOCR);
#endif

		rl_termio.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|NOFLSH);
		rl_termio.c_lflag |=  (ISIG);

		rl_termio.c_cc[VMIN] = 1;
		rl_termio.c_cc[VTIME] = 0;

#ifdef TERMIOS
		/* disable suspending process on ^Z */
		rl_termio.c_cc[VSUSP] = 0;
#endif

#ifdef VINTR
		/* disable suspending process on ^C */
		rl_termio.c_cc[VINTR] = 0;
#endif /* VINTR */

#ifdef TERMIOS
#ifdef TCSETSW
		ioctl(fd, TCSETSW, &rl_termio);
#else
		tcsetattr(fd, TCSADRAIN, &rl_termio);
#endif /* TCSETSW */
#else
		ioctl(fd, TCSETAW, &rl_termio);
#endif /* TERMIOS */
		term_set = 1;
	}
#endif /* MSDOS */
}

void reset_termio(fd)
     int fd;
{
#ifndef MSDOS
	if(term_set == 1) {
#ifdef TERMIOS
#ifdef TCSETSW
		ioctl(fd, TCSETSW, &orig_termio);
#else
		tcsetattr(fd, TCSADRAIN, &orig_termio);
#endif /* TCSETSW */
#else
		ioctl(fd, TCSETAW, &orig_termio);
#endif /* TERMIOS */
		term_set = 0;
	}
#endif /* MSDOS */
}
#endif /* READLINE */
