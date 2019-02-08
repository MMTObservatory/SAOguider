

#include <math.h>

#include <tcl.h>
/* #include <tk.h> */

#include "tclobj.h"
#include "ast.h"


#define MJD1970               40587
#define UNIXtoMJD(unx)      ((unx/SECSPERDAY + MJD1970) )
#define MJDtoUNIX(mjd)      ((int) (((mjd - MJD1970) * SECSPERDAY)))


double slaPa();
double slaGmst();
double slaEqeqx();
double slaDranrm();



char * format(val, buf)
	double   val;
	char	*buf;
{
	sprintf(buf, "%12f", val);

	return buf;
}

Ast_ObjSla(dummy, interp, objc, objv)
    ClientData dummy;    	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
		double  WaveLen = 5000;
		double  MAParams[21];
		double  AOParams[14];
        	double  w, p, h;
		char    name[132];

		char 	*place;
		char	*px;

		double	SRA, SDec, FRA, FDec, UTC;

		double	TDT, TDB;
		double	DUT    = 0.0;
		double	XPolar = 0.0;
		double	YPolar = 0.0;

		double	TeleTmp =  273.155;
		double	TelePmB = 1013.25;
		double	TeleRhy =    0.33;
		double	LapsRat =    0.0065;


		double	ARA, ADec;
		double	AOB, ZOB, EOB, HOB;
		double	PDec, PRA;
		double  pa, lst;

		char	*var;
		char	 buf[44];

        if ( objc != 6 ) {
            TclObjCmdRetStr(interp, "strtod takes three args: obs ra dec mjd var", TCL_ERROR);
        }

	place =           Tcl_GetStringFromObj(objv[1], NULL);
	SRA   = SAOstrtod(Tcl_GetStringFromObj(objv[2], NULL), NULL);
	SDec  = SAOstrtod(Tcl_GetStringFromObj(objv[3], NULL), NULL);
	Tcl_GetDoubleFromObj(interp, objv[4], &UTC);
        UTC   = UNIXtoMJD(UTC);
        var   =           Tcl_GetStringFromObj(objv[5], NULL);

	for ( px = place; *px; px++ )
		if ( islower(*px) ) *px = toupper(*px);

	slaObs(0, place, name, &w, &p, &h);

        /* Other stuff,  I didn't think this up,        - just if you were wondering */
        TDT     = UTC + slaDtt(UTC) / SECSPERDAY;
        TDB     = TDT + slaRcc(TDT, UTC
                        , -w
                        , cos(p) * ((EARTH_RAD + h)/ 1000)
                        , sin(p) * ((EARTH_RAD + h)/ 1000));

        FRA  = h2r(SRA);
        FDec = d2r(SDec);

        WaveLen /= 10000;       /* Angstroms to um */

        /* Precompute some things for slalib
         */
        slaMappa( 2000.0, UTC, MAParams);
        slaAoppa( UTC, DUT
                , -w, p, h
                , XPolar, YPolar
                , TeleTmp, TelePmB, TeleRhy
                , WaveLen, LapsRat, AOParams);

        /* Compute the Alt Az for the center of the field.
         */
        slaMapqkz(FRA, FDec, MAParams, &ARA, &ADec);
        slaAopqk (ARA, ADec, AOParams, &AOB, &ZOB, &HOB, &PDec, &PRA);
	EOB = M_PI/2 - ZOB;

        pa = slaPa(HOB, PDec, p);

        lst = slaGmst(UTC) + -w + slaEqeqx(UTC);
        lst = r2h(slaDranrm(lst));

	Tcl_SetVar2(interp, var, "sra",  format(SRA,      buf), 0);
	Tcl_SetVar2(interp, var, "sdec", format(SDec,     buf), 0);
	Tcl_SetVar2(interp, var, "mjd",  format(UTC,      buf), 0);
	Tcl_SetVar2(interp, var, "ara",  format(r2h(ARA), buf), 0);
	Tcl_SetVar2(interp, var, "adec", format(r2d(ADec),buf), 0);
	Tcl_SetVar2(interp, var, "az",   format(r2d(AOB), buf), 0);
	Tcl_SetVar2(interp, var, "el",   format(r2d(EOB), buf), 0);
	Tcl_SetVar2(interp, var, "zt",   format(r2d(ZOB), buf), 0);
	Tcl_SetVar2(interp, var, "hr",   format(r2h(HOB), buf), 0);
	Tcl_SetVar2(interp, var, "pra",  format(r2h(PRA), buf), 0);
	Tcl_SetVar2(interp, var, "pdec", format(r2d(PDec),buf), 0);
	Tcl_SetVar2(interp, var, "pa",   format(r2d(pa),  buf), 0);
	Tcl_SetVar2(interp, var, "lst",  format(lst,      buf), 0);
	Tcl_SetVar2(interp, var, "lon",  format(r2d(-w),  buf), 0);
	Tcl_SetVar2(interp, var, "lat",  format(r2d(p),   buf), 0);
	Tcl_SetVar2(interp, var, "hei",  format(h,        buf), 0);

	return TCL_OK;
}

