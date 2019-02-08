/* parambind.h
**
**
** Bind routines to c_ routines.
** This keeps C from colliding with fortran in the library on a VMS/VAX.
**/

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

#ifndef _parambind_h_
#define _parambind_h_

#define paramopen	c_paramopen
#define paramstropen	c_paramstropen
#define paramfind	c_paramfind
#define paramunlock	c_paramunlock
#define paramclose	c_paramclose
#define paramerr	c_paramerr
#define paramerrstr	c_paramerrstr
#define paramwrite	c_paramwrite
#define paramfind	c_paramfind
#define paramlist	c_paramlist
#define paccess		c_paccess

#define pgetb		c_pgetb
#define pgets		c_pgets
#define pgeti		c_pgeti
#define pgetf		c_pgetf
#define pgetd		c_pgetd
#define pgetstr		c_pgetstr

#define pputb		c_pputb
#define pputs		c_pputs
#define pputi		c_pputi
#define pputf		c_pputf
#define pputd		c_pputd
#define pputstr		c_pputstr

#endif
