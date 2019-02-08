/* strtod.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 
   This source is distributed without warranty under the terms of
   the GNU General Public License, version 2, 1991.
 */

#include <stdio.h>
#include <ctype.h>

double strtod();
double pow();
int	SAOdtype;


double	SAOstrtod(str, ptr)
	char	 *str;
	char	**ptr;
{
	double	 d = 0.0, dx;
	double	 m = 0.0;
	double	 s = 0.0;
	char	*p, *px;
        char     c;

        SAOdtype = 0;
 
        if ( ptr == NULL ) ptr = &p;
        while ( *str == ' ' ) str++;
 
 
        /* No base implied (yet).
         */
        d = strtod(str, ptr);
	px = *ptr;

        if ( ( c = **ptr )
          && ( c == 'h' || c == 'd' || c == ':' || c == ' ' || c == 'm' )
          && ( (*ptr - str) <= 4 )
          && ( ( isdigit(*((*ptr)+1)) )
	    || ( (*((*ptr)+1)) == ' ' && isdigit(*((*ptr)+2)) ) ) ) {
                double   sign = 1.0;

	    SAOdtype = c;
            (*ptr)++;
 
            if ( *str == '-' ) {
                sign = -1.0;
                d = -d;
            }

	    m = strtod(*ptr, ptr);

	    if ( c == 'm' ) {
		s = m;
		m = d;
		d = 0.0;
	    } else
		if ( ( c = **ptr )
		  && ( c == ':' || c == ' ' || c == 'm' )
		  && ( (*ptr - px) <= 3 )
		  && ( ( isdigit(*((*ptr)+1)) ) 
		    || ( (*((*ptr)+1)) == ' ' && isdigit(*((*ptr)+2)) ) ) ) {
     
		    (*ptr)++;

		    s = strtod(*ptr, ptr);
		}
 
            return sign * (d + m / 60 + s / 3600);
        }
 
        /* I guess that there wern't really any units.
	 */
	return d;
}


int SAOparsefmt(buff, type, width, prec, flag)
	char	*buff;
	int	*type;
	int	*width;
	int	*prec;
	int	*flag;
{
	char	*q = buff+1;
	int	t_flag = 0;

    if ( *buff != '%' ) return 0;

    *type  = 0;
    *width = 0;
    *prec  = 0;
    *flag  = 0;

    /* eat the flags */
    while (*q == '-' || *q == '+' || *q == ' ' ||
             *q == '#' || *q == '0') {
        switch ( *q ) {
            case '-': *flag |=  2;      break;
            case '+': *flag |=  4;      break;
            case ' ': *flag |=  8;      break;
            case '#': *flag |= 16;      break;
            case '0': *flag |= 32;      break;
        }

         q++ ;
    }

    if ( isdigit(*q) ) { *width = strtol(  q, &q, 10); }
    if ( *q == '.' ) { *prec  = strtol(++q, &q, 10); }

    if ( *q == 'h' || *q == 'l' ) {
	t_flag = *q++;
    }

    switch ( *q ) {
     case 'd':  *type = 0;	break;
     case 'b':	
     case 'o':
     case 'x':
     case 'f':
     case 'g':
     case '@':	*type = *q;	break;
     default:	return 0;
    }
 
    if ( *type == '@' ) {
	*type = ':';

	if ( t_flag == 'h' ) *type = 'h';
	if (  *flag  &  8  ) *type = ' ';
    }

    return 1;
}

	

char *SAOconvert(buff, val, type, width, prec, flags)
	char	*buff;
	double	 val;
	int	 type;
	int	 width;
	int	 prec;
	int	 flags;		/* minus, plus, space, pound, zero */
{
                char    fmt[32];
                char   *sign = "";
                char   *zero = "";
		char    dfmt[64];
 
                float   degrees = val;
                float   minutes;
                float   seconds; 
 
                char    ch1, ch2;
 
        if ( flags & 2  ) sign = "-";
        if ( flags & 4  ) sign = "+";
        if ( flags & 8  ) sign = " ";
	if ( flags & 16 ) type = ' ';
	if ( flags & 32 ) zero = "0";

        switch ( type ) {
         case 'b': {
		int		v = val;
		unsigned int	i;
		int		c = 2;

		buff[0] = '0';
		buff[1] = 'b';

		for ( i = 0x8000; i; i /= 2 ) {
			if ( v & i || c > 2 ) {
				buff[c] = v & i ? '1' : '0';
				c++;
			}
		}

		buff[c] = '\0';
                return buff;
	 }
         case 'o':
                sprintf(buff, "0o%o", (int) val);
                return buff;
 
         case 'x':
                sprintf(buff, "0x%x", (int) val);
                return buff;
 
         case ':':      ch1 = ':';      ch2 = ':';      break;
         case ' ':      ch1 = ' ';      ch2 = ' ';      break;
         case 'h':      ch1 = 'h';      ch2 = 'm';      break;
         case 'd':      ch1 = 'd';      ch2 = 'm';      break;
         case 'm':      ch1 = 'm';      ch2 = 'm';      break;
         default: 	return 0;
        }
 
        if ( degrees < 0.0 ) {
                sign = "-";
                degrees = - degrees;
        }

	minutes = (degrees - ((int) degrees)) * 60;
	if ( minutes < 0 ) minutes = 0.0;
	seconds = (minutes - ((int) minutes)) * 60;
	if ( seconds < 0 ) seconds = 0.0;
 
	if ( prec == -1 )
	    if ( type == 'h' ) prec = 4;
	    else	       prec = 3;

	if ( prec == -2 ) {
		if ( type  == 'm' )
		     if ( seconds < 10.0 )
			sprintf(buff, "%s%d%c0%g"
			    , sign, (int) minutes + degrees * 60, ch2, seconds);
		     else
			sprintf(buff, "%s%d%c%g"
			    , sign, (int) minutes + degrees * 60, ch2, seconds);
		else if ( seconds < 10.0 )
			sprintf(buff, "%s%d%c0%2d%c0%g"
			    , sign, (int) (degrees)
			    , ch1 , (int) (minutes)
			    , ch2, seconds);
		      else
			sprintf(buff, "%s%d%c%2d%c%g"
			    , sign, (int) (degrees)
			    , ch1 , (int) (minutes)
			    , ch2, seconds);
	} else {
		double 	p = pow(10.0, (double) prec);
		int 	m = minutes;
		int 	d = degrees;

                if ( (((int)(seconds * p + .5))/ p) >= 60 ) {
                        seconds  = 0.0;
                        m++;
                        if ( m >= 60 ) {
                                m = 0;
                                d++;
                        }
                }

		if ( width ) {
		    sprintf(dfmt, "%%%s%dd", zero, width);
		} else
		    strcpy(dfmt, "%d");

                if ( (((int)(seconds * p + .5))/ p) < 10.0 )
		    sprintf(fmt, "%%s%s%c%%2.2d%c0%%.%df", dfmt, ch1, ch2, prec);
		else
		    sprintf(fmt, "%%s%s%c%%2.2d%c%%.%df" , dfmt, ch1, ch2, prec);

                sprintf(buff, fmt, sign, d, m, seconds);
	}

	return buff;
}

