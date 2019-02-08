#ifndef SLALIBHDEF
#define SLALIBHDEF
#include <math.h>
 
/*
**  - - - - - - - -
**   s l a l i b . h
**  - - - - - - - -
**
**  Prototype function declarations for slalib library.
**
**  P.T.Wallace   Starlink   22 December 1993
*/
 
#ifdef __STDC__
#define PROTOTYPE(X)     X
#else
#define PROTOTYPE(X)    ( )
#endif


void slaAddet PROTOTYPE((double rm, double dm, double eq, double *rc, double *dc ));
 
void slaAfin PROTOTYPE((char *string, int *iptr, float *a, int *j ));
 
double slaAirmas PROTOTYPE((double zd ));
 
void slaAmp PROTOTYPE((double ra, double da, double date, double eq,
              double *rm, double *dm ));
 
void slaAmpqk PROTOTYPE((double ra, double da, double amprms[21],
                double *rm, double *dm ));
 
void slaAop PROTOTYPE((double rap, double dap, double date, double dut,
              double elongm, double phim, double hm, double xp,
              double yp, double tdk, double pmb, double rh,
              double wl, double tlr,
              double *aob, double *zob, double *hob,
              double *dob, double *rob ));
 
void slaAoppa PROTOTYPE((double date, double dut, double elongm, double phim,
                double hm, double xp, double yp, double tdk, double pmb,
                double rh, double wl, double tlr, double aoprms[14] ));
 
void slaAoppat PROTOTYPE((double date, double aoprms[14] ));
 
void slaAopqk PROTOTYPE((double rap, double dap, double aoprms[14],
                double *aob, double *zob, double *hob,
                double *dob, double *rob ));
 
void slaAv2m PROTOTYPE((float axvec[3], float rmat[3][3] ));
 
float slaBear PROTOTYPE((float a1, float b1, float a2, float b2 ));
 
void slaCaf2r PROTOTYPE((int ideg, int iamin, float asec, float *rad, int *j ));
 
void slaCaldj PROTOTYPE((int iy, int im, int id, double *djm, int *j ));
 
void slaCalyd PROTOTYPE((int iy, int im, int id, int *ny, int *nd, int *j ));
 
void slaCc2s PROTOTYPE((float v[3], float *a, float *b ));
 
void slaCc62s PROTOTYPE((float v[6], float *a, float *b, float *r,
                float *ad, float *bd, float *rd ));
 
void slaCd2tf PROTOTYPE((int ndp, float days, char *sign, int ihmsf[4] ));
 
void slaCldj PROTOTYPE((int iy, int im, int id, double *djm, int *j ));
 
void slaCr2af PROTOTYPE((int ndp, float angle, char *sign, int idmsf[4] ));
 
void slaCr2tf PROTOTYPE((int ndp, float angle, char *sign, int ihmsf[4] ));
 
void slaCs2c PROTOTYPE((float a, float b, float v[3] ));
 
void slaCs2c6 PROTOTYPE((float a, float b, float r, float ad,
                float bd, float rd, float v[6] ));
 
void slaCtf2d PROTOTYPE((int ihour, int imin, float sec, float *days, int *j ));
 
void slaCtf2r PROTOTYPE((int ihour, int imin, float sec, float *rad, int *j ));
 
void slaDaf2r PROTOTYPE((int ideg, int iamin, double asec, double *rad, int *j ));
 
void slaDafin PROTOTYPE((char *string, int *iptr, double *a, int *j ));
 
double slaDat PROTOTYPE((double dju ));
 
void slaDav2m PROTOTYPE((double axvec[3], double rmat[3][3] ));
 
double slaDbear PROTOTYPE((double a1, double b1, double a2, double b2 ));
 
void slaDbjin PROTOTYPE((char *string, int *nstrt,
                double *dreslt, int *jf1, int *jf2 ));
 
void slaDc62s PROTOTYPE((double v[6], double *a, double *b, double *r,
                double *ad, double *bd, double *rd ));
 
void slaDcc2s PROTOTYPE((double v[3], double *a, double *b ));
 
void slaDcmpf PROTOTYPE((double coeffs[6], double *xz, double *yz, double *xs,
                double *ys, double *perp, double *orient ));
 
void slaDcs2c PROTOTYPE((double a, double b, double v[3] ));
 
void slaDd2tf PROTOTYPE((int ndp, double days, char *sign, int ihmsf[4] ));
 
void slaDeuler PROTOTYPE((char *order, double phi, double theta, double psi,
                 double rmat[3][3] ));
 
void slaDfltin PROTOTYPE((char *string, int *nstrt, double *dreslt, int *jflag ));
 
void slaDimxv PROTOTYPE((double dm[3][3], double va[3], double vb[3] ));
 
void slaDjcal PROTOTYPE((int ndp, double djm, int iymdf[4], int *j ));
 
void slaDjcl PROTOTYPE((double djm, int *iy, int *im, int *id, double *fd, int *j ));
 
void slaDm2av PROTOTYPE((double rmat[3][3], double axvec[3] ));
 
void slaDmat PROTOTYPE((int n, double *a, double y[], double *d, int *jf, int iw[] ));
 
void slaDmxm PROTOTYPE((double a[3][3], double b[3][3], double c[3][3] ));
 
void slaDmxv PROTOTYPE((double dm[3][3], double va[3], double vb[3] ));
 
void slaDr2af PROTOTYPE((int ndp, double angle, char *sign, int idmsf[4] ));
 
void slaDr2tf PROTOTYPE((int ndp, double angle, char *sign, int ihmsf[4] ));
 
double slaDrange PROTOTYPE((double angle ));
 
double slaDranrm PROTOTYPE((double angle ));
 
void slaDs2c6 PROTOTYPE((double a, double b, double r, double ad, double bd,
                double rd, double v[6] ));
 
void slaDs2tp PROTOTYPE((double ra, double dec, double raz, double decz,
                double *xi, double *eta, int *j ));
 
double slaDsep PROTOTYPE((double a1, double b1, double a2, double b2 ));
 
void slaDtf2d PROTOTYPE((int ihour, int imin, double sec, double *days, int *j ));
 
void slaDtf2r PROTOTYPE((int ihour, int imin, double sec, double *rad, int *j ));
 
void slaDtp2s PROTOTYPE((double xi, double eta, double raz, double decz,
                double *ra, double *dec ));
 
double slaDtt PROTOTYPE((double dju ));
 
double slaDvdv PROTOTYPE((double va[3], double vb[3] ));
 
void slaDvn PROTOTYPE((double v[3], double uv[3], double *vm ));
 
void slaDvxv PROTOTYPE((double va[3], double vb[3], double vc[3] ));
 
void slaEarth PROTOTYPE((int iy, int id, float fd, float posvel[6] ));
 
void slaEcleq PROTOTYPE((double dl, double db, double date, double *dr, double *dd ));
 
void slaEcmat PROTOTYPE((double date, double rmat[3][3] ));
 
void slaEcor PROTOTYPE((float rm, float dm, int iy, int id, float fd,
               float *rv, float *tl ));
 
void slaEg50 PROTOTYPE((double dr, double dd, double *dl, double *db ));
 
double slaEpb PROTOTYPE((double date ));
 
double slaEpb2d PROTOTYPE((double epb ));
 
double slaEpco PROTOTYPE((char k0, char k, double e ));
 
double slaEpj PROTOTYPE((double date ));
 
double slaEpj2d PROTOTYPE((double epj ));
 
void slaEqecl PROTOTYPE((double dr, double dd, double date, double *dl, double *db ));
 
double slaEqeqx PROTOTYPE((double date ));
 
void slaEqgal PROTOTYPE((double dr, double dd, double *dl, double *db ));
 
void slaEtrms PROTOTYPE((double ep, double ev[3] ));
 
void slaEuler PROTOTYPE((char *order, float phi, float theta, float psi,
                float rmat[3][3] ));
 
void slaEvp PROTOTYPE((double date, double deqx,
              double dvb[3], double dpb[3],
              double dvh[3], double dph[3] ));
 
void slaFitxy PROTOTYPE((int itype, int np, double xye[][2], double xym[][2],
                double coeffs[6], int *j ));
 
void slaFk425 PROTOTYPE((double r1950, double d1950, double dr1950,
                double dd1950, double p1950, double v1950,
                double *r2000, double *d2000, double *dr2000,
                double *dd2000, double *p2000, double *v2000 ));
 
void slaFk45z PROTOTYPE((double r1950, double d1950, double bepoch,
                double *r2000, double *d2000 ));
 
void slaFk524 PROTOTYPE((double r2000, double d2000, double dr2000,
                double dd2000, double p2000, double v2000,
                double *r1950, double *d1950, double *dr1950,
                double *dd1950, double *p1950, double *v1950 ));
 
void slaFk54z PROTOTYPE((double r2000, double d2000, double bepoch,
                double *r1950, double *d1950,
                double *dr1950, double *dd1950 ));
 
void slaFlotin PROTOTYPE((char *string, int *nstrt, float *reslt, int *jflag ));
 
void slaGaleq PROTOTYPE((double dl, double db, double *dr, double *dd ));
 
void slaGalsup PROTOTYPE((double dl, double db, double *dsl, double *dsb ));
 
void slaGe50 PROTOTYPE((double dl, double db, double *dr, double *dd ));
 
void slaGeoc PROTOTYPE((double p, double h, double *r, double *z ));
 
double slaGmst PROTOTYPE((double ut1 ));
 
double slaGresid PROTOTYPE((double s ));
 
void slaImxv PROTOTYPE((float rm[3][3], float va[3], float vb[3] ));
 
void slaIntin PROTOTYPE((char *string, int *nstrt, long *ireslt, int *jflag ));
 
void slaInvf PROTOTYPE((double fwds[6], double bkwds[6], int *j ));
 
void slaKbj PROTOTYPE((int jb, double e, char *k, int *j ));
 
void slaM2av PROTOTYPE((float rmat[3][3], float axvec[3] ));
 
void slaMap PROTOTYPE((double rm, double dm, double pr, double pd,
              double px, double rv, double eq, double date,
              double *ra, double *da ));
 
void slaMappa PROTOTYPE((double eq, double date, double amprms[21] ));
 
void slaMapqk PROTOTYPE((double rm, double dm, double pr, double pd,
                double px, double rv, double amprms[21],
                double *ra, double *da ));
 
void slaMapqkz PROTOTYPE((double rm, double dm, double amprms[21],
                 double *ra, double *da ));
 
void slaMoon PROTOTYPE((int iy, int id, float fd, float posvel[6] ));
 
void slaMxm PROTOTYPE((float a[3][3], float b[3][3], float c[3][3] ));
 
void slaMxv PROTOTYPE((float rm[3][3], float va[3], float vb[3] ));
 
void slaNut PROTOTYPE((double date, double rmatn[3][3] ));
 
void slaNutc PROTOTYPE((double date, double *dpsi, double *deps, double *eps0 ));
 
void slaNutm PROTOTYPE((double dpsi, double deps, double eps0,
               double rmatn[3][3] ));
 
void slaOap PROTOTYPE((char *type, double ob1, double ob2, double date,
              double dut, double elongm, double phim, double hm,
              double xp, double yp, double tdk, double pmb,
              double rh, double wl, double tlr,
              double *rap, double *dap ));
 
void slaOapqk PROTOTYPE((char *type, double ob1, double ob2, double aoprms[14],
                double *rap, double *dap ));
 
void slaObs PROTOTYPE((int n, char *c, char *name, double *w, double *p, double *h ));
 
double slaPa PROTOTYPE((double ha, double dec, double phi ));
 
void slaPcd PROTOTYPE((double disco, double *x, double *y ));
 
void slaPm PROTOTYPE((double r0, double d0, double pr, double pd,
             double px, double rv, double ep0, double ep1,
             double *r1, double *d1 ));
 
void slaPrebn PROTOTYPE((double bep0, double bep1, double rmatp[3][3] ));
 
void slaPrec PROTOTYPE((double ep0, double ep1, double rmatp[3][3] ));
 
void slaPreces PROTOTYPE((char sys[3], double ep0, double ep1,
                 double *ra, double *dc ));
 
void slaPrenut PROTOTYPE((double epoch, double date, double rmatpn[3][3] ));
 
void slaPvobs PROTOTYPE((double p, double h, double stl, double pv[6] ));
 
void slaPxy PROTOTYPE((int np, double xye[][2], double xym[][2],
              double coeffs[6],
              double xyp[][2], double *xrms, double *yrms, double *rrms ));
 
double slaRandom PROTOTYPE((double seed ));
 
double slaRcc PROTOTYPE((double tdb, double ut1, double wl, double u, double v ));
 
void slaRdplan ( double date, int np, double elong, double phi,
                 double *ra, double *dec, double *diam );

void slaRefco PROTOTYPE((double hm, double tdk, double pmb, double rh,
                double wl, double phi, double tlr, double eps,
                double *refa, double *refb ));
 
void slaRefro PROTOTYPE((double zobs, double hm, double tdk, double pmb,
                double rh, double wl, double phi, double tlr, double eps,
                double *ref ));
 
void slaRefv PROTOTYPE((double vu[3], double refa, double refb, double vr[3] ));
 
void slaRefz PROTOTYPE((double zu, double refa, double refb, double *zr ));
 
float slaRverot PROTOTYPE((float phi, float ra, float da, float st ));
 
float slaRvgalc PROTOTYPE((float r2000, float d2000 ));
 
float slaRvlg PROTOTYPE((float r2000, float d2000 ));
 
float slaRvlsr PROTOTYPE((float r2000, float d2000 ));
 
void slaS2tp PROTOTYPE((float ra, float dec, float raz, float decz,
               float *xi, float *eta, int *j ));
 
float slaSep PROTOTYPE((float a1, float b1, float a2, float b2 ));
 
void slaSmat PROTOTYPE((int n, float *a, float y[], float *d, int *jf, int iw[] ));
 
void slaSubet PROTOTYPE((double rc, double dc, double eq,
                double *rm, double *dm ));
 
void slaSupgal PROTOTYPE((double dsl, double dsb, double *dl, double *db ));
 
void slaSvd PROTOTYPE((int m, int n, int mp, int np,
              double *a, double w[], double *v, double work[],
              int *jstat ));
 
void slaSvdcov PROTOTYPE((int n, int np, int nc,
                 double w[], double *v, double work[], double *cvm ));
 
void slaSvdsol PROTOTYPE((int m, int n, int mp, int np,
                 double b[], double *u, double w[], double *v,
                 double work[], double x[] ));
 
void slaTp2s PROTOTYPE((float xi, float eta, float raz, float decz,
               float *ra, float *dec ));
 
void slaUnpcd PROTOTYPE((double disco, double *x, double *y ));
 
float slaVdv PROTOTYPE((float va[3], float vb[3] ));
 
void slaVn PROTOTYPE((float v[3], float uv[3], float *vm ));
 
void slaVxv PROTOTYPE((float va[3], float vb[3], float vc[3] ));
 
void slaWait PROTOTYPE((float delay ));
 
void slaXy2xy PROTOTYPE((double x1, double y1, double coeffs[6],
                double *x2, double *y2 ));
 
double slaZd PROTOTYPE((double ha, double dec, double phi ));
 
#endif
