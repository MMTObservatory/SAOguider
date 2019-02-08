
double juldate( int iy, int im, int id)
{
   double mjd;
   long iyL, imL;

/* Month lengths in days */
   static int mtab[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


/* Validate year */
   if ( iy < -4699 ) { return -1; }

/* Validate month */
   if ( ( im < 1 ) || ( im > 12 ) ) { return -2; }

/* Allow for leap year */
   mtab[1] = ( ( ( iy % 4 ) == 0 ) &&
             ( ( ( iy % 100 ) != 0 ) || ( ( iy % 400 ) == 0 ) ) ) ?
             29 : 28;

/* Validate day */
   if ( id < 1 || id > mtab[im-1] ) { return -3; }

/* Lengthen year and month numbers to avoid overflow */
   iyL = (long) iy;
   imL = (long) im;

   return (double)
        ( ( 1461L * ( iyL - ( 12L - imL ) / 10L + 4712L ) ) / 4L
        + ( 306L * ( ( imL + 9L ) % 12L ) + 5L ) / 10L
        - ( 3L * ( ( iyL - ( 12L - imL ) / 10L + 4900L ) / 100L ) ) / 4L
        + (long) id - 2399904L );
}

void caldate (double mjd, int *year, int *month, int *day, double *hour)
{
   long int jd, k, m, n;

   *hour = (mjd - (int) mjd) * 24.0;

   jd = (long int) mjd + 2400001L;

   k     = jd + 68569L;
   n     = 4L * k / 146097L;

   k     = k - (146097L * n + 3L) / 4L;
   m     = 4000L * (k + 1L) / 1461001L;
   k     = k - 1461L * m / 4L + 31L;

   *month = (short int) (80L * k / 2447L);
   *day   = (short int) (k - 2447L * (long int) *month / 80L);
   k      = (long int) *month / 11L;

   *month = (short int) ((long int) *month + 2L - 12L * k);
   *year  = (short int) (100L * (n - 49L) + m + k);

   return;
}

