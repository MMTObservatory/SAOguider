/* saotng.c
 */
/* copyright 1991, 1993, 1995, 1999 John B. Roll jr.
 */

#include <xos.h>
#include <xfile.h>

saotng(xpa, name, x, y, bitpix, data, frame)
	char	*xpa;
	int	 x, y;
	int	 bitpix;
	void	*data;
	int	 frame;
{

	typedef char	Card[80];
	Card		head[36];

	char		command[80];

	File		fp;

	memset(head, ' ', 2880);

/*			  123456789012345678901234567890123		*/
	sprintf(head[0], "SIMPLE  =                    T");		head[0][30] = ' ';
	sprintf(head[1], "BITPIX  =                  %3d", bitpix);	head[1][30] = ' ';
	sprintf(head[2], "NAXIS   =                    2");		head[2][30] = ' ';
	sprintf(head[3], "NAXIS1  =                  %3d", x);		head[3][30] = ' ';
	sprintf(head[4], "NAXIS2  =                  %3d", y);		head[4][30] = ' ';
	sprintf(head[5], "END                           ");		head[5][30] = ' ';

	sprintf(command, "echo \"\" | xpaset %s frame %d", xpa, frame);
	system(command);

	sprintf(command, "xpaset %s fits %s", xpa, name);

	fp = popen(command, "w");

	Write(fp, head, 2880, 1);
	Write(fp, data, x * y * Abs(bitpix)/8, 1);

	pclose(fp);
}
