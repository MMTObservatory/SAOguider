/*
 * UERGET -- Get an error message
 */

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

#include <stdio.h>

void strc2f();

struct f77errstruct{
  int code;
  char *ermes;
};

#define ENDLIST -1001

static struct f77errstruct f77errs[] = {
/* iraf77errs 	   -- Error definitions -- */
{0, ""},
/* */
/*	-- CL access error messages -- 1-9 */
{1, "CL parameter not found"},
{2, "CL parameter has wrong type"},
{3, "CL parameter is undefined"},
{5, "Error putting parameter into CL"},
{6, "First element is bad (must be > 0)"},
{7, "Number of elements is bad (must be > 0)"},
{-1, "End of file on CL string or end of list"},
{-2, "End of file encountered"},
/* */
/*	-- Image access error message -- 10-29 */
{10, "Error opening existing file"},
{11, "Error opening new file"},
{12, "Error opening new copy file"},
{13, "Naxis is wrong"},
{14, "Dimen[naxis] is wrong"},
{15, "Datatype is wrong"},
{16, "Error returning offset to image data"},
{17, "Access mode is wrong"},
{18, "Error closing existing file"},
{19, "Not an image"},
{20, "Bad section specification for image"},
{21, "Bad extension for image"},
{22, "Section is illegal for new image/new copy"},
{23, "Error reading pixel file"},
{24, "Error writing to pixel file"},
{25, "Status of unkown image descriptor parameter"},
{26, "Set unkown image descriptor parameter"},
/* */
/* */
/*	-- Put message Errors -- 30-39 */
{30, "Error writting to STDOUT"},
{31, "Error writting to the log file"},
{32, "Destination for output is bad"},
{33, "Priority for output is bad"},
/*	Header i/o information status  40-59 */
{40, "Header parameter not found"},
{41, "Illegal datatype for header parameter "},
{42, "Out of space in image header"},
{43, "Attempt to redefine parameter"},
{45, "Bad input template"},
{46, "Bad input first element"},
{47, "Bad input number of elements"},
{48, "Cannot delete standard keyword"},
{49, "Cannot delete nonexistant keyword"},
{50, "Image does not exist"},
{51, "Error deleting image"},
{52, "Error renaming image"},
{53, "Illegal image header"},
/*	-- Template processing Errors -- 60-69 */
{60, "Error opening template"},
{61, "Error getting next image from template"},
{62, "Too many concatenable sublist in template filename"},
{63, "Illegal filename template"},
{64, "Error closing template"},
/*	-- Graphics processing Errors -- 70-99 */
{70, "Bad graphic device"},
{71, "Bad graphic mod"},
{72, "Error opening graphic device"},
{73, "Error closing graphic device"},
{74, "Error clearing  graphic device"},
{75, "Error setting window"},
{76, "Error getting window"},
{77, "Error plotting a polyline"},
{78, "Error plotting a sequence of markers"},
{79, "Wrong number of points"},
{80, "Bad marker (size is incorrect)"},
{81, "Bad axes specification"},
{82, "Bad justification for test"},
{83, "Bad character size"},
{84, "Bad orientation (for text)"},
{85, "Error writting label"},
{86, "Error writting text"},
{87, "Error setting viewport"},
{88, "Error getting viewport"},
{89, "Error setting GIO parameter"},
{90, "Error getting GIO parameter"},
{91, "Bad reset code (in reset function)"},
{92, "Error writting cursor"},
{93, "Error in high level plot routine"},
{94, "Error in high level error bar routine"},
{95, "Error in high level histogram plot"},
{96, "Bad cursor call"},
{97, "Error in scale procedure"},
/*         -- Dynamic Memory Errors -- 100-109 */
{100, "Invalid datatype for dynamic memory allocation"},
{101, "Error allocating dynamic memory with malloc"},
{102, "Error freeing dynamic memory"},
/*	  -- Character to number and viceversa error codes (110-119) */
{110, "Not a number"},
{111, "Overflow occurred"},
{112, "Field width not wide enough"},
{ENDLIST, "Unknown error"},
};

#ifdef vms

#include descrip.h

typedef struct dsc$descriptor descriptor;

#define DataLength(x)	x->dsc$w_length
#define DataAddrss(x)	x->dsc$a_pointer

uerget(errcode, text_d)
     int 	*errcode;
     descriptor *text_d;
{
		char	*text = DataAddrss(text_d);
		int	ntext = DataLength(text_d);

		int i;
		char tbuf[160];

	for(i=0; *errcode!=f77errs[i].code && i!=ENDLIST; i++)
		;

	sprintf(tbuf, "Error %d: %s", i, f77errs[i].ermes);

	strc2f(tbuf, text, ntext);
}

#else

uerget_(errcode, text, ntext)
			int 	*errcode;
			char	*text;
			int	ntext;
{
		int i;
		char tbuf[160];

	for(i=0; *errcode!=f77errs[i].code && i!=ENDLIST; i++)
		;

	sprintf(tbuf, "Error %d: %s", i, f77errs[i].ermes);

	strc2f(tbuf, text, ntext);
}
#endif
