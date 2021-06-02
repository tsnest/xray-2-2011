#ifndef SISL_INCLUDED
#define SISL_INCLUDED
/* SISL - SINTEF Spline Library version 4.4.                              */

//#  define MAXDOUBLE   1.79769313486231570e+308
//#  define MAXFLOAT    ((float)3.40282346638528860e+38)
//#  define MINDOUBLE   2.22507385850720140e-308
//#  define MINFLOAT     ((float)1.17549435082228750e-38)
//#  define MIN_VALUE		MINFLOAT 

#  define MIN_VALUE		1.0e-300
typedef       double sisl_double;
//typedef       float sisl_double;

#define SISLNEEDPROTOTYPES
typedef struct SISLdir
{
  int igtpi;			/* 0 - The direction of the surface or curve
			               is not greater than pi in any
			               parameter direction.
			           1 - The direction of the surface or curve
			               is greater than pi in the first
			               parameter direction.
			           2 - The direction of the surface is greater
			               than pi in the second parameter
			               direction. 			     */
  sisl_double *ecoef;		/* The coordinates to the center of the cone.*/
  sisl_double aang;			/* The angle from the center whice describe the
			           cone.				     */
  sisl_double *esmooth;		/* Coordinates of object after smoothing.    */
} SISLdir;
 /* The following structure contains 3 different boxes. The
    first box is the plain box given by the coefficients of the
    object. The second box is expanded with the half of a given
    tolerance. The third box is expanded with half the tolerance
    in the inner and for the vertices at the edges/endpoints
    a distance of half the tolerance is removed. The minimum and
    maximum values of the boxes are given by the arrays
    e2min[0] - e2min[2] and e2max[0] - e2max[2]. The tolerances used
    when making the boxes are stored in etol[0] - etol[2]. etol[0]
    will always be equal to zero. If a box is made, the pointers
    belonging to this box points to arrays, otherwise they point
    to SISL_NULL.                                                       */

typedef struct SISLbox
{
  sisl_double *emax;			/* The minimum values to the boxes.	     */
  sisl_double *emin;			/* The maximum values to the boxes.	     */
  int imin;			/* The index of the min coeff (one-dim case) */
  int imax;			/* The index of the max coeff (one-dim case) */

  sisl_double *e2max[3];		/* The minimum values dependant on tolerance.*/
  sisl_double *e2min[3];		/* The maximum values dependant on tolerance.*/
  sisl_double etol[3];		/* Tolerances of the boxes.                  */
} SISLbox;

/* This file will contain the definition of the structure SISLCurve which
   will contain the description of a B-spline curve.                        */

typedef struct SISLCurve
{
  int ik;			/* Order of curve.                           */
  int in;			/* Number of vertices.                       */
  sisl_double *et;			/* Pointer to the knotvector.                */
  sisl_double *ecoef;		/* Pointer to the array containing vertices. */
  sisl_double *rcoef;		/*Pointer to the array of scaled vertices if
				  rational.  */
  int ikind;			/* Kind of curve
	                           = 1 : Polynomial B-spline curve.
	                           = 2 : Rational B-spline curve.
	                           = 3 : Polynomial Bezier curve.
	                           = 4 : Rational Bezier curve.             */
  int idim;			/* Dimension of the space in which the curve
				   lies.      */
  int icopy;			/* Indicates whether the arrays of the curve
				   are copied or referenced by creation of the
				   curve.
	                           = 0 : Pointer set to input arrays.
			           = 1 : Copied.
	                           = 2 : Pointer set to input arrays,
				         but are to be treated as copied.   */
  SISLdir *pdir;		/* Pointer to a structur to store curve
				   direction.      */
  SISLbox *pbox;		/* Pointer to a structur to store the
				   surrounded boxes. */
  int cuopen;			/* Open/closed flag.                         */
} SISLCurve;

/* This file will contain the definition of the structure SISLSurf which
   contains the description of a tensor-product surface.                    */

typedef struct SISLSurf
{
  int ik1;			/* Order of surface in first parameter
				   direction.       */
  int ik2;			/* Order of surface in second parameter
				   direction.      */
  int in1;			/* Number of vertices in first parameter
				   direction.     */
  int in2;			/* Number of vertices in second parameter
				   direction.    */
  sisl_double *et1;			/* Pointer to knotvector in first parameter
				   direction.  */
  sisl_double *et2;			/* Pointer to knotvector in second parameter
				   direction. */
  sisl_double *ecoef;		/* Pointer to array of vertices of surface. */
  sisl_double *rcoef;		/* Pointer to the array of scaled vertices
				   if surface is rational. */
  int ikind;			/* Kind of surface
	                           = 1 : Polynomial B-spline tensor-product
				         surface.
	                           = 2 : Rational B-spline tensor-product
				         surface.
	                           = 3 : Polynomial Bezier tensor-product
				         surface.
	                           = 4 : Rational Bezier tensor-product
				         surface.                           */
  int idim;			/* Dimension of the space in which the surface
				   lies.    */
  int icopy;			/* Indicates whether the arrays of the surface
				   are copied or referenced by creation of
				   the surface.
	                           = 0 : Pointer set to input arrays.
			           = 1 : Copied.
	                           = 2 : Pointer set to input arrays,
				         but are to be treated as copied.               */
  SISLdir *pdir;		/* Pointer to a structur to store surface
				   direction.    */
  SISLbox *pbox;		/* Pointer to a structur to store the
				   surrounded boxes. */
  int use_count;                /* use count so that several tracks can share
				   surfaces, no internal use */
 int cuopen_1;                  /* Open/closed flag, 1. par directiion */
 int cuopen_2;                  /* Open/closed flag. 2. par direction  */
} SISLSurf;

/* This file contains the description of an intersection curve.
   The curve is given by a number of points represented by the
   parameter-values of the point in the objects involved in the
   intersection. Pointers to the curve in the geometry space and
   in the parameter planes of the objects might exist.            */

typedef struct SISLIntcurve
{
  int ipoint;			/* Number of points defining the curve.      */
  int ipar1;			/* Number of parameter directions of first
				   object.                                   */
  int ipar2;			/* Number of parameter directions of second
				 * object.                                   */
  sisl_double *epar1;		/* Pointer to the parameter-values of the
				   points
	                           in the first object.                      */
  sisl_double *epar2;		/* Pointer to the parameter-values of the
				   points
	                           in the second object. If one of the objects
	                           is an analytic curve or surface epar2 points
	                           to nothing.                               */
  SISLCurve *pgeom;		/* Pointer to the intersection curve in the
				   geometry space. If the curve is not
				   computed, pgeom points to nothing.       */
  SISLCurve *ppar1;		/* Pointer to the intersection curve in the
				   parameter plane of the first object. If
				   the curve is not computed, ppar1 points
				   to nothing.                              */
  SISLCurve *ppar2;		/* Pointer to the intersection curve in the
				   parameter plane of the second object. If
				   the curve is not computed, ppar2 points
				   to nothing.                              */
  int itype;			/* Kind of curve.
	                           = 1 : Straight line.
	                           = 2 : Closed loop. No singularities.
	                           = 3 : Closed loop. One singularity.
				         Not used.
	                           = 4 : Open curve. No singularity.
	                           = 5 : Open curve. Singularity at the
	                                 beginning of the curve.
	                           = 6 : Open curve. Singularity at the end
	                                 of the curve.
	                           = 7 : Open curve. Singularity at the
				         beginning  and end of the curve.
	                           = 8 : An isolated singularity. Not used.
				   = 9 : The curve is exact, pgeom and either
				   	 ppar1 or ppar2 is set.      */

  int pretop[4];		/* Pretopology */
} SISLIntcurve;

/* Pretopology information. */

enum
{
  SI_RIGHT=1, SI_LEFT=2
};
enum
{
  SI_UNDEF, SI_IN, SI_OUT, SI_ON, SI_AT
};

#define SISL_CRV_PERIODIC -1
#define SISL_CRV_OPEN 1
#define SISL_CRV_CLOSED 0

#define SISL_SURF_PERIODIC -1
#define SISL_SURF_OPEN 1
#define SISL_SURF_CLOSED 0

 /*
 * Required for C++ 2.0 and later version
 * --------------------------------------
 */

#if defined(SISLNEEDPROTOTYPES)

#ifndef CONSTRUCT
extern
#endif
SISLbox      *newbox(int);
#ifndef CONSTRUCT
extern
#endif
SISLCurve    *newCurve(int,int,sisl_double *,sisl_double *,int,int,int);
#ifndef CONSTRUCT
extern
#endif
SISLCurve    *copyCurve(SISLCurve *);
#ifndef CONSTRUCT
extern
#endif
SISLdir      *newdir(int);
#ifndef CONSTRUCT
extern
#endif
SISLIntcurve *newIntcurve(int,int,int,sisl_double *,sisl_double *,int);
#ifndef CONSTRUCT
extern
#endif
SISLSurf     *newSurf(int,int,int,int,sisl_double *,sisl_double *,sisl_double *,int,int,int);
#ifndef CONSTRUCT
extern
#endif
SISLSurf     *copySurface(SISLSurf *);

#ifndef DESTRUCT
extern
#endif
void freeCurve(SISLCurve *);
#ifndef DESTRUCT
extern
#endif
void freeIntcrvlist(SISLIntcurve **,int);
#ifndef DESTRUCT
extern
#endif
void freeIntcurve(SISLIntcurve *);
#ifndef DESTRUCT
extern
#endif
void freeSurf(SISLSurf *);
#ifndef  S1001
extern
#endif
void s1001(SISLSurf *,sisl_double,sisl_double,sisl_double,sisl_double,SISLSurf **,int *);
#ifndef  S1011
extern
#endif
void s1011(sisl_double [],sisl_double [],sisl_double [],sisl_double,int,SISLCurve **,int *);
#ifndef  S1012
extern
#endif
void s1012(sisl_double [],sisl_double [],sisl_double [],sisl_double,int,int,
		SISLCurve **,int *);
#ifndef  S1013
extern
#endif
void s1013(SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double *,int *);
#ifndef  S1014
extern
#endif
void s1014(SISLCurve *,sisl_double[],sisl_double,sisl_double,sisl_double[],sisl_double[],sisl_double,
                  sisl_double *,sisl_double *,sisl_double[],int *);
#ifndef  S1015
extern
#endif
void s1015(SISLCurve *,SISLCurve *,sisl_double,sisl_double[],sisl_double[],sisl_double,
		  sisl_double *,sisl_double *,sisl_double[],int *);
#ifndef  S1016
extern
#endif
void s1016(SISLCurve *,sisl_double[],sisl_double[],sisl_double,sisl_double[],sisl_double[],
		  sisl_double,sisl_double *,sisl_double *,sisl_double[],int *);
#ifndef  S1017
extern
#endif
void s1017(SISLCurve *,SISLCurve **,sisl_double,int *);
#ifndef  S1018
extern
#endif
void s1018(SISLCurve *,sisl_double [],int,SISLCurve **,int *);
#ifndef  S1021
extern
#endif
void s1021(sisl_double [],sisl_double [],sisl_double,sisl_double [],sisl_double,SISLSurf **,int *);
#ifndef  S1022
extern
#endif
void s1022(sisl_double [],sisl_double[],sisl_double,sisl_double [],sisl_double,sisl_double,
	       SISLSurf **,int *);
#ifndef  S1023
extern
#endif
void s1023(sisl_double [],sisl_double [],sisl_double [],int,int,SISLSurf **,int *);
#ifndef  S1024
extern
#endif
void s1024(sisl_double [],sisl_double [],sisl_double [],sisl_double,int,int,int,
	      SISLSurf **,int *);
#ifndef  S1025
extern
#endif
void s1025(SISLSurf *,sisl_double [],int,sisl_double [],int,SISLSurf **,int *);
#ifndef  S1221
extern
#endif
void s1221(SISLCurve *,int,sisl_double,int *,sisl_double [],int *);
#ifndef  S1225
extern
#endif
void s1225(SISLCurve *,int,sisl_double,int *,sisl_double [],sisl_double [],sisl_double *,int *);
#ifndef  S1226
extern
#endif
void s1226(SISLCurve *,int,sisl_double,int *,sisl_double [],sisl_double [],sisl_double *,int *);
#ifndef  S1227
extern
#endif
void s1227(SISLCurve *,int,sisl_double,int *,sisl_double [],int *);
#ifndef  S1233
extern
#endif
void s1233(SISLCurve *,sisl_double,sisl_double,SISLCurve **,int *);
#ifndef  S1237
extern
#endif
void s1237(SISLSurf *,int,int,sisl_double,int *);
#ifndef  S1238
extern
#endif
void s1238(SISLSurf *,SISLCurve *,int,int,sisl_double,sisl_double,int *);
#ifndef  S1240
extern
#endif
void s1240(SISLCurve *,sisl_double,sisl_double *,int *);
#ifndef  S1241
extern
#endif
void s1241(SISLCurve *,sisl_double [],int,sisl_double,sisl_double *,int *);
#ifndef  S1243
extern
#endif
void s1243(SISLCurve *,sisl_double [],int,sisl_double,sisl_double[],sisl_double *,
            sisl_double *,int *);
#ifndef  S1302
extern
#endif
void s1302(SISLCurve *,sisl_double,sisl_double,sisl_double [],sisl_double [],SISLSurf **,int *);
#ifndef  S1303
extern
#endif
void s1303(sisl_double [],sisl_double,sisl_double,sisl_double [],sisl_double [],int,SISLCurve **,int *);
#ifndef  S1310
extern
#endif
void s1310(SISLSurf *,SISLSurf *,SISLIntcurve *,sisl_double,sisl_double,int,int,int *);
#ifndef  S1314
extern
#endif
void s1314(SISLSurf *,sisl_double *,sisl_double *,int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef  S1315
extern
#endif
void s1315(SISLSurf *,sisl_double *,sisl_double,int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef  S1316
extern
#endif
void s1316(SISLSurf *,sisl_double *,sisl_double *,sisl_double,int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef  S1317
extern
#endif
void s1317(SISLSurf *,sisl_double *,sisl_double *,sisl_double *,int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef  S1318
extern
#endif
void s1318(SISLSurf *,sisl_double *,sisl_double *,sisl_double,sisl_double,int,sisl_double,sisl_double,
	   sisl_double,SISLIntcurve *,int,int,int *);
#ifndef  S1319
extern
#endif
void s1319(SISLSurf *,sisl_double *,int,sisl_double,sisl_double,sisl_double,SISLIntcurve *,
	   int,int,int *);
#ifndef  S1327
extern
#endif
void s1327(SISLCurve *,sisl_double [],sisl_double [],sisl_double [],int,SISLCurve **,int *);
#ifndef  S1328
extern
#endif
void s1328(SISLSurf *,sisl_double [],sisl_double [],sisl_double [],int,SISLSurf **,int *);
#ifndef  S1332
extern
#endif
void s1332(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],SISLSurf **,int *);
#ifndef  S1356
extern
#endif
void s1356(sisl_double [],int,int,int [],int,int,int,int,sisl_double,
	   sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef  S1357
extern
#endif
void s1357(sisl_double [],int,int,int [],sisl_double [],int,int,int,int,sisl_double,
	   sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef  S1360
extern
#endif
void s1360(SISLCurve *,sisl_double,sisl_double,sisl_double [],sisl_double,int,SISLCurve **,int *);
#ifndef  S1363
extern
#endif
void s1363(SISLCurve *,sisl_double *,sisl_double *,int *);
#ifndef  S1364
extern
#endif
void s1364(SISLCurve *,sisl_double,int *);
#ifndef S1365
extern
#endif
void s1365(SISLSurf *,sisl_double,sisl_double, sisl_double,int,SISLSurf **,int *);
#ifndef  S1369
extern
#endif
void s1369(SISLSurf *,sisl_double [],sisl_double [],sisl_double,sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1371
extern
#endif
void s1371(SISLCurve *,sisl_double [],sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1372
extern
#endif
void s1372(SISLCurve *,sisl_double [],sisl_double [],sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1373
extern
#endif
void s1373(SISLCurve *,sisl_double [],sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1374
extern
#endif
void s1374(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1375
extern
#endif
void s1375(SISLCurve *,sisl_double [],sisl_double [],sisl_double,sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1379
extern
#endif
void s1379(sisl_double [],sisl_double [],sisl_double [],int,int,SISLCurve **,int *);
#ifndef  S1380
extern
#endif
void s1380(sisl_double [],sisl_double [],int,int,int,SISLCurve **,int *);
#ifndef  S1383
extern
#endif
void s1383(SISLSurf *,SISLCurve *,sisl_double,sisl_double,int,SISLCurve **,
	   SISLCurve **,SISLCurve **,int *);
#ifndef  S1386
extern
#endif
void s1386(SISLSurf *,int,int,SISLSurf **,int *);
#ifndef  S1387
extern
#endif
void s1387(SISLSurf *,int,int,SISLSurf **,int *);
#ifndef  S1388
extern
#endif
void s1388(SISLSurf *,sisl_double *[],int *,int *,int *,int *);
#ifndef  S1389
extern
#endif
void s1389(SISLCurve *,sisl_double *[],int *,int *,int *);
#ifndef  S1390
extern
#endif
void s1390(SISLCurve *[],SISLSurf **,int [],int *);
#ifndef  S1391
extern
#endif
void s1391(SISLCurve **,SISLSurf ***,int,int [],int *);
#ifndef  S1401
extern
#endif
void s1401(SISLCurve *[],sisl_double [],SISLSurf **,int *);
#ifndef  S1421
extern
#endif
void s1421(SISLSurf *,int,sisl_double [],int *,int *,sisl_double [],sisl_double [],int *);
#ifndef  S1422
extern
#endif
void s1422(SISLSurf *,int,int,int,sisl_double [],int *,int *,
		 sisl_double [],sisl_double [],int *);
#ifndef  S1424
extern
#endif
void s1424(SISLSurf *,int,int,sisl_double [],int *,int *,sisl_double [],int *);
#ifndef  S1425
extern
#endif
void s1425(SISLSurf *,int,int,int,int,sisl_double [],int *,int *,
		 sisl_double [],int *);
#ifndef  S1439
extern
#endif
void s1439(SISLSurf *,sisl_double,int,SISLCurve **,int *);
#ifndef  S1440
extern
#endif
void s1440(SISLSurf *,SISLSurf **,int *);
#ifndef  S1450
extern
#endif
void s1450(SISLSurf *,sisl_double,int *,int *,int *,int *,int *,int *,int *);
#ifndef  S1451
extern
#endif
void s1451(SISLCurve *,sisl_double,int *,int *);
#ifndef S1501
extern
#endif
void s1501(SISLSurf *,sisl_double *,sisl_double *,sisl_double *,sisl_double,sisl_double,int,
	   sisl_double,sisl_double,sisl_double,SISLIntcurve *,int,int,int *);
#ifndef S1502
extern
#endif
void s1502(SISLCurve *,sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double,int,
	   sisl_double,sisl_double,int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1503
extern
#endif
void s1503(SISLSurf *,sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double,int,
	   sisl_double,sisl_double,int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1506
extern
#endif
void s1506(SISLSurf *,int,int,sisl_double *,int,sisl_double *,sisl_double [],
           sisl_double [],int *);
#ifndef S1508
extern
#endif
void s1508(int,SISLCurve **,sisl_double[],SISLSurf **,int *);
#ifndef S1510
extern
#endif
void s1510(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1511
extern
#endif
void s1511(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1514
extern
#endif
void s1514(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,sisl_double,SISLIntcurve *,
	   int,int,int *);
#ifndef S1515
extern
#endif
void s1515(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef S1518
extern
#endif
void s1518(SISLSurf *surf, sisl_double point[], sisl_double dir[], sisl_double epsge,
	   sisl_double start[], sisl_double end[], sisl_double parin[], sisl_double parout[],
	   int *stat);
#ifndef  S1522
extern
#endif
void s1522(sisl_double [],sisl_double [],sisl_double [],sisl_double,int,SISLCurve **,int *);
#ifndef S1529
extern
#endif
void s1529(sisl_double [],sisl_double [],sisl_double [],sisl_double [],
           int,int,int,int,SISLSurf **,int *);
#ifndef S1530
extern
#endif
void s1530(sisl_double [],sisl_double [],sisl_double [],sisl_double [], sisl_double [],sisl_double [],
           int ,int ,int , SISLSurf **,int *);
#ifndef S1534
extern
#endif
void s1534(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int,int,
	   int,int,int,int,int,int,int,int,int,SISLSurf **,int *);
#ifndef S1535
extern
#endif
void s1535(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int,int,sisl_double [],
	   sisl_double [],int,int,int,int,int,int,int,int,SISLSurf **,int *);
#ifndef S1536
extern
#endif
void s1536(sisl_double [],int,int,int,int,int,int,int,int,int,int,
	   int,int,SISLSurf **,int *);
#ifndef S1537
extern
#endif
void s1537(sisl_double [],int,int,int,sisl_double [],sisl_double [],int,int,
	   int,int,int,int,int,int,SISLSurf **,int *);
#ifndef  S1538
extern
#endif
void s1538(int,SISLCurve *[],int [],sisl_double,int,int,int,SISLSurf **,
	   sisl_double **,int *);
#ifndef  S1539
extern
#endif
void s1539(int,SISLCurve *[],int [],sisl_double [], sisl_double,int,int,int,SISLSurf **,
	   sisl_double **,int *);
#ifndef  S1542
extern
#endif
void s1542(SISLCurve *, int, sisl_double *, sisl_double [], int *);
#ifndef  S1600
extern
#endif
void s1600(SISLCurve *,sisl_double [],sisl_double [],int,SISLCurve **,int *);
#ifndef  S1601
extern
#endif
void s1601(SISLSurf *,sisl_double [],sisl_double [],int,SISLSurf **,int *);
#ifndef  S1602
extern
#endif
void s1602(sisl_double [],sisl_double [],int,int,sisl_double,sisl_double *,SISLCurve **,int *);
#ifndef  S1603
extern
#endif
void s1603(SISLSurf *,sisl_double *,sisl_double *,sisl_double *,sisl_double *,int *);
#ifndef  S1606
extern
#endif
void s1606(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],sisl_double [],
	   int,int,int,SISLCurve **,int *);
#ifndef  S1607
extern
#endif
void s1607(SISLCurve *,SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double,sisl_double,
	   int,int,int,SISLCurve **,int *);
#ifndef  S1608
extern
#endif
void s1608(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],int,int,int,SISLCurve **,sisl_double *,sisl_double *,
	   sisl_double *,sisl_double *,int *);
#ifndef  S1609
extern
#endif
void s1609(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],sisl_double [],sisl_double [],
	   sisl_double,sisl_double [],int,int,int,SISLCurve **,sisl_double *,sisl_double *,
	   sisl_double *,sisl_double *,int *);
#ifndef  S1611
extern
#endif
void s1611(sisl_double [],int,int,sisl_double [],int,int,sisl_double,sisl_double,
	   sisl_double *,SISLCurve **,int *);
#ifndef  S1613
extern
#endif
void s1613(SISLCurve *,sisl_double,sisl_double **,int *,int *);
#ifndef S1620
extern
#endif
void s1620(sisl_double epoint[],int inbpnt1, int inbpnt2, int ipar,
           int iopen1, int iopen2, int ik1, int ik2, int idim,
           SISLSurf **rs,int *jstat);
#ifndef  S1630
extern
#endif
void s1630(sisl_double [],int,sisl_double,int,int,int,SISLCurve **,int *);
#ifndef  S1706
extern
#endif
void s1706(SISLCurve *);
#ifndef  S1710
extern
#endif
void s1710(SISLCurve *,sisl_double,SISLCurve **,SISLCurve **,int *);
#ifndef  S1711
extern
#endif
void s1711(SISLSurf *,int,sisl_double,SISLSurf **,SISLSurf **,int *);
#ifndef  S1712
extern
#endif
void s1712(SISLCurve *,sisl_double,sisl_double,SISLCurve **,int *);
#ifndef  S1713
extern
#endif
void s1713(SISLCurve *,sisl_double,sisl_double,SISLCurve **,int *);
#ifndef  S1714
extern
#endif
void s1714(SISLCurve *,sisl_double,sisl_double,SISLCurve **,SISLCurve **,int *);
#ifndef  S1715
extern
#endif
void s1715(SISLCurve *,SISLCurve *,int,int,SISLCurve **,int *);
#ifndef  S1716
extern
#endif
void s1716(SISLCurve *,SISLCurve *,sisl_double,SISLCurve **,int *);
#ifndef  S1720
extern
#endif
void s1720(SISLCurve *,int,SISLCurve **,int *);
#ifndef  S1730
extern
#endif
void s1730(SISLCurve *,SISLCurve **,int *);
#ifndef  S1731
extern
#endif
void s1731(SISLSurf *,SISLSurf **,int *);
#ifndef  S1732
extern
#endif
void s1732(SISLCurve *,int,sisl_double *,sisl_double *,sisl_double *,int *);
#ifndef  S1733
extern
#endif
void s1733(SISLSurf *,int,int,sisl_double *,sisl_double *,sisl_double *,sisl_double *,
	   sisl_double *,int *);
#ifndef  S1750
extern
#endif
void s1750(SISLCurve *,int,SISLCurve **,int *);
#ifndef  S1774
extern
#endif
void s1774(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,sisl_double,sisl_double,sisl_double *,
	   int *);
#ifndef  S1775
extern
#endif
void s1775(SISLSurf *,sisl_double [],int,sisl_double,sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],int *);
#ifndef  S1850
extern
#endif
void s1850(SISLCurve *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1851
extern
#endif
void s1851(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1852
extern
#endif
void s1852(SISLSurf *,sisl_double [],sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **, int *,SISLIntcurve ***,int *);
#ifndef  S1853
extern
#endif
void s1853(SISLSurf *,sisl_double [],sisl_double [],sisl_double,int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1854
extern
#endif
void s1854(SISLSurf *,sisl_double [],sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1855
extern
#endif
void s1855(SISLSurf *,sisl_double [],sisl_double,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1856
extern
#endif
void s1856(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1857
extern
#endif
void s1857(SISLCurve *,SISLCurve *,sisl_double,sisl_double,
	   int *,sisl_double **,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1858
extern
#endif
void s1858(SISLSurf *,SISLCurve *,sisl_double,sisl_double,
	   int *,sisl_double **,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1859
extern
#endif
void s1859(SISLSurf *,SISLSurf *,sisl_double,sisl_double,
	   int *,sisl_double **,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1860
extern
#endif
void s1860(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1870
extern
#endif
void
   s1870(SISLSurf *ps1, sisl_double *pt1, int idim, sisl_double aepsge,
	 int *jpt,sisl_double **gpar1,int *jcrv,SISLIntcurve ***wcurve,int *jstat);
#ifndef S1871
extern
#endif
void
   s1871(SISLCurve *pc1, sisl_double *pt1, int idim, sisl_double aepsge,
	 int *jpt,sisl_double **gpar1,int *jcrv,SISLIntcurve ***wcurve,int *jstat);
#ifndef  S1920
extern
#endif
void s1920(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1921
extern
#endif
void s1921(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1940
extern
#endif
void s1940(SISLCurve *oldcurve, sisl_double eps[], int startfix, int endfix,
	   int iopen, int itmax, SISLCurve **newcurve, sisl_double maxerr[],
	   int *stat);
#ifndef  S1953
extern
#endif
void s1953(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1954
extern
#endif
void s1954(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,
	   int *,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1955
extern
#endif
void s1955(SISLCurve *,SISLCurve *,sisl_double,sisl_double,
	   int *,sisl_double **,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef  S1957
extern
#endif
void s1957(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,sisl_double *,sisl_double *,int *);
#ifndef  S1958
extern
#endif
void s1958(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,sisl_double [],sisl_double *,int *);
#ifndef S1961
extern
#endif
void s1961(sisl_double ep[],int im,int idim,int ipar,sisl_double epar[],
	   sisl_double eeps[],int ilend,int irend,int iopen,sisl_double afctol,
	   int itmax,int ik,SISLCurve **rc,sisl_double emxerr[],
	   int *jstat);
#ifndef S1962
extern
#endif
void s1962(sisl_double ep[],sisl_double ev[],int im,int idim,int ipar,sisl_double epar[],
           sisl_double eeps[],int ilend,int irend,int iopen,int itmax,
           SISLCurve **rc,sisl_double emxerr[],int *jstat);
#ifndef S1963
extern
#endif
void s1963(SISLCurve *pc,sisl_double eeps[],int ilend,int irend,int iopen,
           int itmax, SISLCurve **rc,int *jstat);
#ifndef S1965
extern
#endif
void s1965(SISLSurf *oldsurf,sisl_double eps[],int edgefix[4],int iopen1,
           int iopen2,sisl_double edgeps[],int opt,int itmax,
           SISLSurf **newsurf,sisl_double maxerr[],int *stat);
#ifndef S1966
extern
#endif
void s1966(sisl_double ep[],int im1,int im2,int idim,int ipar,sisl_double epar1[],
           sisl_double epar2[],sisl_double eeps[],int nend[],int iopen1,int iopen2,
           sisl_double edgeps[],sisl_double afctol,int iopt,int itmax,
           int ik1,int ik2,SISLSurf **rs,sisl_double emxerr[],int *jstat);
#ifndef S1967
extern
#endif
void s1967(sisl_double ep[],sisl_double etang1[],sisl_double etang2[],sisl_double eder11[],
           int im1,int im2,int idim,int ipar,sisl_double epar1[],sisl_double epar2[],
           sisl_double eeps[],int nend[],int iopen1,int iopen2,sisl_double edgeps[],
           int iopt,int itmax,SISLSurf **rs,sisl_double emxerr[],
           int *jstat);
#ifndef S1968
extern
#endif
void s1968(SISLSurf *ps,sisl_double eeps[],int nend[],int iopen1,int iopen2,
           sisl_double edgeps[],int iopt,int itmax,SISLSurf **rs,
           int *jstat);
#ifndef S1986
extern
#endif
void s1986(SISLCurve *pc, sisl_double aepsge, int *jgtpi, sisl_double **gaxis,
	   sisl_double *cang,int *jstat);
#ifndef S1987
extern
#endif
void s1987(SISLSurf *ps, sisl_double aepsge, int *jgtpi, sisl_double **gaxis,
	   sisl_double *cang,int *jstat);
#ifndef S1988
extern
#endif
void s1988(SISLCurve *,sisl_double **,sisl_double **,int *);
#ifndef S1989
extern
#endif
void s1989(SISLSurf *,sisl_double **,sisl_double **,int *);
#ifndef  S2500
extern
#endif
void s2500(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2502
extern
#endif
void s2502(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2504
extern
#endif
void s2504(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2506
extern
#endif
void s2506(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2508
extern
#endif
void s2508(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2510
extern
#endif
void s2510(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2532
extern
#endif
void s2532(SISLSurf *, int, int, int *, int *, SISLSurf ***, int *);
#ifndef  S2536
extern
#endif
void s2536(SISLSurf *, int, int, int *, int *, SISLSurf ***, int *);
#ifndef  S2540
extern
#endif
void s2540(SISLSurf *, int, int, int, sisl_double [], int, int, sisl_double **, int *);
#ifndef  S2542
extern
#endif
void s2542(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *,
	   sisl_double *, sisl_double [], sisl_double [],int *);
#ifndef  S2544
extern
#endif
void s2544(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double *, int *);
#ifndef  S2545
extern
#endif
void s2545(SISLSurf *, int, int, int, sisl_double [], int, int, sisl_double, sisl_double **,
	   int *);
#ifndef  S2550
extern
#endif
void s2550(SISLCurve *, sisl_double [], int, sisl_double [], int *);
#ifndef  S2553
extern
#endif
void s2553(SISLCurve *, sisl_double [], int, sisl_double [], int *);
#ifndef  S2556
extern
#endif
void s2556(SISLCurve *, sisl_double [], int, sisl_double [], int *);
#ifndef  S2559
extern
#endif
void s2559(SISLCurve *, sisl_double [], int, sisl_double [], sisl_double [], sisl_double [],
           sisl_double [], int *);
#ifndef  S2562
extern
#endif
void s2562(SISLCurve *, sisl_double [], int, int, sisl_double [], sisl_double [], sisl_double [],
           sisl_double [], sisl_double [], int *);
#ifndef  S6DRAWSEQ
extern
#endif
void s6drawseq(sisl_double [],int);

#else /* not SISLNEEDPROTOTYPES */

#ifndef CONSTRUCT
extern
#endif
SISLbox       *newbox();
#ifndef CONSTRUCT
extern
#endif
SISLCurve     *newCurve();
#ifndef CONSTRUCT
extern
#endif
SISLCurve     *copyCurve();
#ifndef CONSTRUCT
extern
#endif
SISLdir       *newdir();
#ifndef CONSTRUCT
extern
#endif
SISLIntcurve  *newIntcurve();
#ifndef CONSTRUCT
extern
#endif
SISLSurf      *newSurf();
#ifndef CONSTRUCT
extern
#endif
SISLSurf      *copySurface();
#ifndef DESTRUCT
extern
#endif
void freeCurve();
#ifndef DESTRUCT
extern
#endif
void freeIntcrvlist();
#ifndef DESTRUCT
extern
#endif
void freeIntcurve();
#ifndef DESTRUCT
extern
#endif
void freeSurf();

#ifndef  S1001
extern
#endif
void s1001();
#ifndef  S1011
extern
#endif
void s1011();
#ifndef  S1012
extern
#endif
void s1012();
#ifndef  S1013
extern
#endif
void s1013();
#ifndef  S1014
extern
#endif
void s1014();
#ifndef  S1015
extern
#endif
void s1015();
#ifndef  S1016
extern
#endif
void s1016();
#ifndef  S1017
extern
#endif
void s1017();
#ifndef  S1018
extern
#endif
void s1018();
#ifndef  S1021
extern
#endif
void s1021();
#ifndef  S1022
extern
#endif
void s1022();
#ifndef  S1023
extern
#endif
void s1023();
#ifndef  S1024
extern
#endif
void s1024();
#ifndef  S1025
extern
#endif
void s1025();
#ifndef  S1221
extern
#endif
void s1221();
#ifndef  S1225
extern
#endif
void s1225();
#ifndef  S1226
extern
#endif
void s1226();
#ifndef  S1227
extern
#endif
void s1227();
#ifndef  S1233
extern
#endif
void s1233();
#ifndef  S1237
extern
#endif
void s1237();
#ifndef  S1238
extern
#endif
void s1238();
#ifndef  S1240
extern
#endif
void s1240();
#ifndef  S1241
extern
#endif
void s1241();
#ifndef  S1243
extern
#endif
void s1243();
#ifndef  S1302
extern
#endif
void s1302();
#ifndef  S1303
extern
#endif
void s1303();
#ifndef  S1310
extern
#endif
void s1310();
#ifndef  S1314
extern
#endif
void s1314();
#ifndef  S1315
extern
#endif
void s1315();
#ifndef  S1316
extern
#endif
void s1316();
#ifndef  S1317
extern
#endif
void s1317();
#ifndef  S1318
extern
#endif
void s1318();
#ifndef  S1319
extern
#endif
void s1319();
#ifndef  S1327
extern
#endif
void s1327();
#ifndef  S1328
extern
#endif
void s1328();
#ifndef  S1332
extern
#endif
void s1332();
#ifndef  S1356
extern
#endif
void s1356();
#ifndef  S1357
extern
#endif
void s1357();
#ifndef  S1360
extern
#endif
void s1360();
#ifndef  S1363
extern
#endif
void s1363();
#ifndef  S1364
extern
#endif
void s1364();
#ifndef  S1365
extern
#endif
void s1365();
#ifndef  S1369
extern
#endif
void s1369();
#ifndef  S1371
extern
#endif
void s1371();
#ifndef  S1372
extern
#endif
void s1372();
#ifndef  S1373
extern
#endif
void s1373();
#ifndef  S1374
extern
#endif
void s1374();
#ifndef  S1375
extern
#endif
void s1375();
#ifndef  S1379
extern
#endif
void s1379();
#ifndef  S1380
extern
#endif
void s1380();
#ifndef  S1383
extern
#endif
void s1383();
#ifndef  S1386
extern
#endif
void s1386();
#ifndef  S1387
extern
#endif
void s1387();
#ifndef  S1388
extern
#endif
void s1388();
#ifndef  S1389
extern
#endif
void s1389();
#ifndef  S1390
extern
#endif
void s1390();
#ifndef  S1391
extern
#endif
void s1391();
#ifndef  S1401
extern
#endif
void s1401();
#ifndef  S1421
extern
#endif
void s1421();
#ifndef  S1422
extern
#endif
void s1422();
#ifndef  S1424
extern
#endif
void s1424();
#ifndef  S1425
extern
#endif
void s1425();
#ifndef  S1439
extern
#endif
void s1439();
#ifndef  S1440
extern
#endif
void s1440();
#ifndef  S1450
extern
#endif
void s1450();
#ifndef  S1451
extern
#endif
void s1451();
#ifndef S1501
extern
#endif
void s1501();
#ifndef S1502
extern
#endif
void s1502();
#ifndef S1503
extern
#endif
void s1503();
#ifndef S1506
extern
#endif
void s1506();
#ifndef S1508
extern
#endif
void s1508();
#ifndef S1510
extern
#endif
void s1510();
#ifndef S1511
extern
#endif
void s1511();
#ifndef S1514
extern
#endif
void s1514();
#ifndef S1515
extern
#endif
void s1515();
#ifndef S1522
extern
#endif
void s1522();
#ifndef S1529
extern
#endif
void s1529();
#ifndef S1530
extern
#endif
void s1530();
#ifndef S1534
extern
#endif
void s1534();
#ifndef S1535
extern
#endif
void s1535();
#ifndef S1536
extern
#endif
void s1536();
#ifndef S1537
extern
#endif
void s1537();
#ifndef S1538
extern
#endif
void s1538();
#ifndef S1539
extern
#endif
void s1539();
#ifndef S1542
extern
#endif
void s1542();
#ifndef  S1600
extern
#endif
void s1600();
#ifndef  S1601
extern
#endif
void s1601();
#ifndef  S1602
extern
#endif
void s1602();
#ifndef  S1603
extern
#endif
void s1603();
#ifndef  S1606
extern
#endif
void s1606();
#ifndef  S1607
extern
#endif
void s1607();
#ifndef  S1608
extern
#endif
void s1608();
#ifndef  S1609
extern
#endif
void s1609();
#ifndef  S1611
extern
#endif
void s1611();
#ifndef  S1613
extern
#endif
void s1613();
#ifndef  S1620
extern
#endif
void s1620();
#ifndef  S1630
extern
#endif
void s1630();
#ifndef  S1706
extern
#endif
void s1706();
#ifndef  S1710
extern
#endif
void s1710();
#ifndef  S1711
extern
#endif
void s1711();
#ifndef  S1712
extern
#endif
void s1712();
#ifndef  S1713
extern
#endif
void s1713();
#ifndef  S1714
extern
#endif
void s1714();
#ifndef  S1715
extern
#endif
void s1715();
#ifndef  S1716
extern
#endif
void s1716();
#ifndef  S1720
extern
#endif
void s1720();
#ifndef  S1730
extern
#endif
void s1730();
#ifndef  S1731
extern
#endif
void s1731();
#ifndef  S1732
extern
#endif
void s1732();
#ifndef  S1733
extern
#endif
void s1733();
#ifndef  S1750
extern
#endif
void s1750();
#ifndef  S1774
extern
#endif
void s1774();
#ifndef  S1775
extern
#endif
void s1775();
#ifndef  S1850
extern
#endif
void s1850();
#ifndef  S1851
extern
#endif
void s1851();
#ifndef  S1852
extern
#endif
void s1852();
#ifndef  S1853
extern
#endif
void s1853();
#ifndef  S1854
extern
#endif
void s1854();
#ifndef  S1855
extern
#endif
void s1855();
#ifndef  S1856
extern
#endif
void s1856();
#ifndef  S1857
extern
#endif
void s1857();
#ifndef  S1858
extern
#endif
void s1858();
#ifndef  S1859
extern
#endif
void s1859();
#ifndef  S1860
extern
#endif
void s1860();
#ifndef  S1870
extern
#endif
void s1870();
#ifndef  S1871
extern
#endif
void s1871();
#ifndef  S1920
extern
#endif
void s1920();
#ifndef  S1921
extern
#endif
void s1921();
#ifndef  S1940
extern
#endif
void s1940();
#ifndef  S1953
extern
#endif
void s1953();
#ifndef  S1954
extern
#endif
void s1954();
#ifndef  S1955
extern
#endif
void s1955();
#ifndef  S1957
extern
#endif
void s1957();
#ifndef  S1958
extern
#endif
void s1958();
#ifndef  S1961
extern
#endif
void s1961();
#ifndef  S1962
extern
#endif
void s1962();
#ifndef  S1963
extern
#endif
void s1963();
#ifndef  S1965
extern
#endif
void s1965();
#ifndef  S1966
extern
#endif
void s1966();
#ifndef  S1967
extern
#endif
void s1967();
#ifndef  S1968
extern
#endif
void s1968();
#ifndef S1986
extern
#endif
void s1986();
#ifndef S1987
extern
#endif
void s1987();
#ifndef S1988
extern
#endif
void s1988();
#ifndef S1989
extern
#endif
void s1989();
#ifndef  S2500
extern
#endif
void s2500();
#ifndef  S2502
extern
#endif
void s2502();
#ifndef  S2504
extern
#endif
void s2504();
#ifndef  S2506
extern
#endif
void s2506();
#ifndef  S2508
extern
#endif
void s2508();
#ifndef  S2510
extern
#endif
void s2510();
#ifndef  S2532
extern
#endif
void s2532();
#ifndef  S2536
extern
#endif
void s2536();
#ifndef  S2540
extern
#endif
void s2540();
#ifndef  S2542
extern
#endif
void s2542();
#ifndef  S2544
extern
#endif
void s2544();
#ifndef  S2545
extern
#endif
void s2545();
#ifndef  S2550
extern
#endif
void s2550();
#ifndef  S2553
extern
#endif
void s2553();
#ifndef  S2556
extern
#endif
void s2556();
#ifndef  S2559
extern
#endif
void s2559();
#ifndef  S2562
extern
#endif
void s2562();
#ifndef  S6DRAWSEQ
extern
#endif
void s6drawseq();
#endif /* End of forward declarations of sisl top level routines */

#endif /* SISL_INCLUDED */
/* DO NOT ADD ANYTHING AFTER THIS LINE */
