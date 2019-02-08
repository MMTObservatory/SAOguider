#include "slalib.h"
#include "slamac.h"

#include <stdlib.h>
#include <limits.h>
#undef RAND_MAX /* To fix problem with SunOS */
#define RAND_MAX LONG_MAX

double slaRandom ( seed )
	double seed;
/*
**  - - - - - - - - - -
**   s l a R a n d o m
**  - - - - - - - - - -
**
**  Generate pseudo-random real number in the range 0 <= x < 1.
**
**  (single precision)
**
**  Given:
**     seed     float     initializes random number generator: used
**                        first time through only.
**
**  P.T.Wallace   Starlink   13 December 1993
*/
{
   static int ftf = 1;
 
   if ( ftf )
   {
      ftf = 0;
      srandom ( gmin ( UINT_MAX, (unsigned int) gmax ( 1.0,
                                 dnint ( fabs ( (double) seed ) ) ) ) );
   }

   return (double) random() / ((double) RAND_MAX);
}
