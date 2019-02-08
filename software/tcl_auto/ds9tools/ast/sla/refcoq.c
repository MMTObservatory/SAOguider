#include "slalib.h"
#include "slamac.h"
void slaRefcoq ( double tdk, double pmb, double rh, double wl,
                 double *refa, double *refb )
/*
**  - - - - - - - - - -
**   s l a R e f c o q
**  - - - - - - - - - -
**
**  Determine the constants A and B in the atmospheric refraction
**  model dZ = A tan Z + B tan^3 Z.  This is a fast alternative
**  to the slaRefco routine - see notes.
**
**  Z is the "observed" zenith distance (i.e. affected by refraction)
**  and dZ is what to add to Z to give the "topocentric" (i.e. in vacuo)
**  zenith distance.
**
**  Given:
**    tdk    double    ambient temperature at the observer (deg K)
**    pmb    double    pressure at the observer (millibar)
**    rh     double    relative humidity at the observer (range 0-1)
**    wl     double    effective wavelength of the source (micrometre)
**
**  Returned:
**    refa   double*   tan Z coefficient (radian)
**    refb   double*   tan^3 Z coefficient (radian)
**
**  The radio refraction is chosen by specifying WL > 100 micrometres.
**
**  Notes:
**
**  1  The model is an approximation, for moderate zenith distances,
**     to the predictions of the slaRefro routine.  The approximation
**     is maintained across a range of conditions, and applies to
**     both optical/IR and radio.
**
**  2  The algorithm is a fast alternative to the slaRefco routine.
**     The latter calls the slaRefro routine itself:  this involves
**     integrations through a model atmosphere, and is costly in
**     processor time.  However, the model which is produced is precisely
**     correct for two zenith distance (45 degrees and about 76 degrees)
**     and at other zenith distances is limited in accuracy only by the
**     A tan Z + B tan^3 Z formulation itself.  The present routine
**     is not as accurate, though it satisfies most practical
**     requirements.
**
**  3  The model omits the effects of (i) height above sea level (apart
**     from the reduced pressure itself), (ii) latitude (i.e. the
**     flattening of the Earth) and (iii) variations in tropospheric
**     lapse rate.
**
**     The model was tested using the following range of conditions:
**
**       lapse rates 0.0055, 0.0065, 0.0075 deg/metre
**       latitudes 0, 25, 50, 75 degrees
**       heights 0, 2500, 5000 metres ASL
**       pressures mean for height -10% to +5% in steps of 5%
**       temperatures -10 deg to +20 deg with respect to 280 deg at SL
**       relative humidity 0, 0.5, 1
**       wavelengths 0.4, 0.6, ... 2 micron, + radio
**       zenith distances 15, 45, 75 degrees
**
**     The accuracy with respect to direct use of the slaRefro routine
**     was as follows:
**
**                            worst         RMS
**
**       optical/IR           62 mas       8 mas
**       radio               319 mas      49 mas
**
**     For this particular set of conditions:
**
**       lapse rate 0.0065 degK/metre
**       latitude 50 degrees
**       sea level
**       pressure 1005 mB
**       temperature 280.15 degK
**       humidity 80%
**       wavelength 5740 Angstroms
**
**     the results were as follows:
**
**       ZD        slaRefro    slaRefcoq   Saastamoinen
**
**       10         10.27        10.27        10.27
**       20         21.19        21.20        21.19
**       30         33.61        33.61        33.60
**       40         48.82        48.83        48.81
**       45         58.16        58.18        58.16
**       50         69.28        69.30        69.27
**       55         82.97        82.99        82.95
**       60        100.51       100.54       100.50
**       65        124.23       124.26       124.20
**       70        158.63       158.68       158.61
**       72        177.32       177.37       177.31
**       74        200.35       200.38       200.32
**       76        229.45       229.43       229.42
**       78        267.44       267.29       267.41
**       80        319.13       318.55       319.10
**
**      deg        arcsec       arcsec       arcsec
**
**     The values for Saastamoinen's formula (which includes terms
**     up to tan^5) are taken from Hohenkerk and Sinclair (1985).
**
**     The results from the much slower but more accurate slaRefco
**     routine have not been included in the tabulation as they are
**     identical to those in the slaRefro column to the 0.01 arcsec
**     resolution used.
**
**  4  Outlandish input parameters are silently limited to mathematically
**     safe values.  Zero pressure is permissible, and causes zeroes to
**     be returned.
**
**  5  The algorithm draws on several sources, as follows:
**
**     a) The formula for the saturation vapour pressure of water as
**        a function of temperature and temperature is taken from
**        expressions A4.5-A4.7 of Gill (1982).
**
**     b) The formula for the water vapour pressure, Given the
**        saturation pressure and the relative humidity, is from
**        Crane (1976), expression 2.5.5.
**
**     c) The refractivity of air is a function of temperature,
**        total pressure, water-vapour pressure and, in the case
**        of optical/IR but not radio, wavelength.  The formulae
**        for the two cases are developed from the Essen and Froome
**        expressions adopted in Resolution 1 of the 12th International
**        Geodesy Association General Assembly (1963).
**
**     The above three items are as used in the slaRefro routine.
**
**     d) The formula for beta, the ratio of the scale height of the
**        atmosphere to the geocentric distance of the observer, is
**        an adaption of expression 9 from Stone (1996).  The
**        adaptations, arrived at empirically, consist of (i) a
**        small adjustment to the coefficient and (ii) a humidity
**        term for the radio case only.
**
**     e) The formulae for the refraction constants as a function of
**        n-1 and beta are from Green (1987), expression 4.31.
**
**  References:
**
**     Crane, R.K., Meeks, M.L. (ed), "Refraction Effects in the Neutral
**     Atmosphere", Methods of Experimental Physics: Astrophysics 12B,
**     Academic Press, 1976.
**
**     Gill, Adrian E., "Atmosphere-Ocean Dynamics", Academic Press, 1982.
**
**     Hohenkerk, C.Y., & Sinclair, A.T., NAO Technical Note No. 63, 1985.
**
**     International Geodesy Association General Assembly, Bulletin
**     Geodesique 70 p390, 1963.
**
**     Stone, Ronald C., P.A.S.P. 108 1051-1058, 1996.
**
**     Green, R.M., "Spherical Astronomy", Cambridge University Press, 1987.
**
**  Last revision:   17 March 1999
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
   int optic;
   double t, p, r,w, tdc, ps, pw, wlsq, gamma, beta;


/* Decide whether optical/IR or radio case:  switch at 100 microns. */
   optic = ( wl <= 100.0 );

/* Restrict parameters to safe values. */
   t = gmax ( tdk, 100.0 );
   t = gmin ( t, 500.0 );
   p = gmax ( pmb, 0.0 );
   p = gmin ( p, 10000.0 );
   r = gmax ( rh, 0.0 );
   r = gmin ( r, 1.0 );
   w = gmax ( wl, 0.1 );
   w = gmin ( w, 1e6 );

/* Water vapour pressure at the observer. */
   if ( p > 0.0 ) {
      tdc = t - 273.15;
      ps = pow ( 10.0, ( 0.7859 + 0.03477 * tdc ) /
                          ( 1.0 + 0.00412 * tdc ) ) *
                 ( 1.0 + p * ( 4.5e-6 + 6e-10 * tdc * tdc )  );
      pw = r * ps / ( 1.0 - ( 1.0 - r ) * ps / p );
   } else {
      pw = 0.0;
   }

/* Refractive index minus 1 at the observer. */
   if ( optic ) {
      wlsq = wl * wl;
      gamma = ( ( 77.532e-6 + ( 4.391e-7 + 3.57e-9 / wlsq ) / wlsq ) * p
                - 11.2684e-6 * pw ) / t;
   } else {
      gamma = ( 77.624e-6 * p - ( 12.92e-6 - 0.371897 / t ) * pw ) / t;
   }

/* Formula for beta from Stone, with empirical adjustments. */
   beta = 4.4474e-6 * t;
   if ( !optic ) beta -= 0.0074 * pw * beta;

/* Refraction constants from Green. */
   *refa = gamma * ( 1.0 - beta );
   *refb = - gamma * ( beta - gamma / 2.0 );
}
