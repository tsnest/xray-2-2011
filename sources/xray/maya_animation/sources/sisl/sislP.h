#ifndef SISLP_INCLUDED
#define SISLP_INCLUDED
/* SISL - SINTEF Spline Library version 4.4.                              */
/* Definition and interrogation of NURBS curves and surface.              */
/* Copyright (C) 1978-2005, SINTEF ICT, Applied Mathematics, Norway.      */

/* This program is free software; you can redistribute it and/or          */
/* modify it under the terms of the GNU General Public License            */
/* as published by the Free Software Foundation version 2 of the License. */

/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/* GNU General Public License for more details.                           */

/* You should have received a copy of the GNU General Public License      */
/* along with this program; if not, write to the Free Software            */
/* Foundation, Inc.,                                                      */
/* 59 Temple Place - Suite 330,                                           */
/* Boston, MA  02111-1307, USA.                                           */

/* Contact information: e-mail: tor.dokken@sintef.no                      */
/* SINTEF ICT, Department of Applied Mathematics,                         */
/* P.O. Box 124 Blindern,                                                 */
/* 0314 Oslo, Norway.                                                     */

/* SISL commercial licenses are available for:                            */
/* - Building commercial software.                                        */
/* - Building software whose source code you wish to keep private.        */

//#if defined(_SISL_COPYRIGHT_H)
#define SISL_INTERNAL 1
//#endif
//
//#include "copyright.h"

#define	SISLNEEDPROTOTYPES

#include "sisl.h"
/*
 *
 * $Id: sislP.h,v 1.55 2004/01/21 13:50:14 vsk Exp $
 *
 */






/* This file contains the definition of the structure SISLPoint
   The structure is created 18.04.89 by UJK.                           */


typedef struct SISLPoint
{
  sisl_double ec[3];
  int idim;			/* The dimension the point lies in           */
  sisl_double *ecoef;		/* Pointer to the array containing the
				   coordinates */
  int icopy;			/* Indicates whether the arrays of the point
				   are copied or referenced by creation of
				   the point.
				   = 0 : Pointer set to input arrays.
				   = 1 : Copied.
				   = 2 : Pointer set to input arrays,
				         but are to be treated as copied.    */

  SISLbox *pbox;		/*Pointer to a structur to store the boxes.  */
} SISLPoint;



/* This file contains the definition of the structure SISLObject that contains
   the description of a geometric object that take part in an intersection
   problem or in a related problem. SISLObject can represent any geometric
   structure, but only on at a time.                                   */

typedef struct SISLObject
{
  int iobj;			/* Integer indicates which kind of geometric
				   object is contained in a particular
				   instance of the structure.
		                   = 1 (SISLCurve) - curve.
		                   = 2 (SURFACE) - tensor-product surface.   */
  SISLPoint *p1;		/* Pointer to a point (instance of Point).   */
  SISLCurve *c1;		/* Pointer to a curve
				 * (instance of SISLCurve).                  */
  SISLSurf *s1;			/* Pointer to a surface
				 * (instance of SISLSurf).                   */
  struct SISLObject *o1;	/* Pointer to parent object
				 * (instance of Object).                     */
  struct SISLObject *edg[4];	/* Pointer to objects edges
				 * (instance of Object).                     */
  struct SISLObject *psimple;	/* Indicates if object/object intersection
				 * is simple case. */
} SISLObject;

typedef struct SISLIntsurf
{
  int ipoint;			/* Number of points defining the curve.    */
  int ipar;			/* Number of parameter directions of       */
  sisl_double *epar;		        /* Pointer to the parameter-values of the
				   points, dimension: ipoint*ipar          */
  int *const_par;               /* Constant parameter direction between
				   two points in epar.                     */
} SISLIntsurf;


typedef struct SISLTrimpar
{
    int ptindex;  /* Index in pnext of pointer to an adjacent point
		      in the trim curve. */
    int parindex;  /* The parameter which is constant along
		      the trim curve, between
                      this point and the ptindex point. */
} SISLTrimpar;

/* This file contains the description of an intersection point (or
   closest point or extremal point) found in the recursive part of
   an intersection routine.                                        */


typedef struct SISLIntpt
{
  int ipar;			/* Number of parameter directions in
				 * intersection problem.                     */
  sisl_double *epar;			/* Parametervalues of point, possibly in two
				 * objects.                                  */
  sisl_double adist;			/* Distance between the objects in this point.
				 * tdist is used in closest point problems.  */
  struct SISLIntpt *pcurve;	/* Not used, kept for compatibility with old
			           version on the structure.*/
  int iinter;			/* = 1 ORDINARY MAIN POINT
				   = 2 SINGULAR MAIN POINT
				   = 3 TRIM MAIN POINT
		                   = -1 ORDINARY HELP POINT
				   = -2 SINGULAR HELP POINT
				   = -3 TRIM HELP POINT */
  struct SISLIntpt **pnext;	/* Pointers to next points in each curve
				 * chain.                                   */
  int *curve_dir;		/* An array of curve directions + from - to
				 * this point.                              */
  int no_of_curves;		/* Number of curves containing this point.  */
  int no_of_curves_alloc;	/* The size of the arrays allocated         */
  int *left_obj_1;		/* Pretopology information, one for each
				 * curve.                                   */
  int *left_obj_2;		/* Pretopology information, one for each
				 * curve.                                   */
  int *right_obj_1;		/* Pretopology information, one for each
				   curve.                                   */
  int *right_obj_2;		/* Pretopology information, one for each
				   curve.                                   */
  int size_1;			/* Size of geo_data_1                       */
  int size_2;			/* Size of geo_data_2                       */
  sisl_double *geo_data_1;		/* Containing geometric info first object   */
  sisl_double *geo_data_2;		/* Containing geometric info second object  */
  /*  sisl_double  geo_aux[3]; Containing auxiliary geo info, see sh6idput*/
  sisl_double geo_track_3d[10];	/* To store intersection curve info */
  sisl_double geo_track_2d_1[7];
  sisl_double geo_track_2d_2[7];
  int edge_1;                   /* Edge flag for topology           */
  int edge_2;
  int marker;                   /* Help attribute when creating lists  */
  int evaluated;                /* Help attribute when creating tracks */
  struct SISLTrimpar *trim[2];          /* Used if pt is in trim curve. */
  int iside_1;			/* Left/right evaluator flag.  -1,0+ */
  int iside_2;			/* Left/right evaluator flag.  -1,0+*/
} SISLIntpt;

/* This file contains the definition of the structure SISLTrack
   The structure is created 10.06.91 by UJK.                           */

typedef struct SISLTrack
{
  SISLSurf *psurf_1;		/* Pointer to first surface in intersection */
  SISLSurf *psurf_2;		/* Pointer to second surface in intersection */
  SISLCurve *pcurve_3d;		/* Pointer to 3D support curve. */
  SISLCurve *pcurve_2d_1;	/* Pointer to 2D support curve in first
				   parameter space. */
  SISLCurve *pcurve_2d_2;	/* Pointer to 2D support curve in second
				   parameter space. */
  int ideg;			/* Type of track.
				    = 0, Bspline vs Bspline
				= 1, Bspline vs Plane
				= 2, Bspline vs Quadric surface
				= 1001 Bspline vs Torus surface
				= 1003 Bspline silhouette line, parallel
				  projection
				= 1004 Bspline silhouette line, perspective
				  projection
				= 1005 Bspline silhouette line, circular
			          projection */

  sisl_double eimpli[16];		/* Description of the implicit surface */
  int turned;			/* Connection between the direction of the
				   support curve and the cross product
				   between the two surface normals.
				= 0, same direction
				= 1, oposite direction */
  int exact;                    /* Flag if curve is exact */
  int pretop[4];		/* Pretopology */
  int sing_start;               /* Singular start end point markers */
  int sing_end;
} SISLTrack;

/* This file contains the description of a list connecting points that
   belong to an intersection curve.                                    */

typedef struct SISLIntlist
{
  SISLIntpt *pfirst;		/* Pointer to first point in list. */
  SISLIntpt *plast;		/* Pointer to last point in list.  */
  int ind_first;		/* Index pointer in pfirst         */
  int ind_last;			/* Index pointer in plast          */
  int itype;			/* Status of curve-segment.
                               = 0 : open curve, no singularities.
                               = 1 : closed curve, no singularities.
                               = 2 : more than two curves meet at start point.
                               = 3 : more than two curves meet at end point.
                               = 4 : more than two curves meet at start
                                     and end point.
                               = 5 : isolated singularity.
                               = 6 : touching area of surface.            */
  int inumb;			/* Number of points in the list.          */
  int pretop[4];		/* Pretopology */
} SISLIntlist;

/* This file contains the definition of an element in a list where
   each element points to an intersection point. The structure is
   used internally in intersection routines and related routines.   */

typedef struct SISLPtedge
{
  SISLIntpt *ppt;		/* Pointer to intersection points.      */
  struct SISLPtedge *pnext;	/* Pointer to next element in the list. */
} SISLPtedge;

/* This file contains the definition of the structure keeping
   track of intersections on edges/endpoints of objects. This
   structure is used internally in the recursive part of intersection
   routines and related routines.                                     */

typedef struct SISLEdge
{
  int iedge;			/* Number of edges/endpoints of object.      */
  int ipoint;			/* Number of intersection points found on
				 * the edges.                                */
  SISLPtedge **prpt;		/* Array containing lists of pointers to the
			         * intersections at the edges.               */
} SISLEdge;

/* This file contains the definition of the structure SISLIntdat used
   intarnally in intersection routines in order to store information
   in intersection points and curves.                                        */

typedef struct SISLIntdat
{
  SISLIntpt **vpoint;
  int ipoint;
  int ipmax;
  SISLIntlist **vlist;
  int ilist;
  int ilmax;
} SISLIntdat;

typedef struct rank_info
    {
    int *prio;
    int *groups;
    int antgr;
    int antrem;
  }rank_info;

/* UJK: NEW INTERSECTION STUFF START*/

enum
{
  SI_ORD = 1, SI_SING, SI_TRIM, SI_TOUCH
};

/* UJK: NEW INTERSECTION STUFF END*/


//#ifndef HUGE
//#define HUGE          MAXDOUBLE
//#endif

/* Storage classes. Used in space allocation.  */
typedef       int sisl_int;



/* Name of geometry objects. Used in branching. */
#define SISLPOINT    0
#define SISLCURVE    1
#define SISLSURFACE  2

/* The resolution of sisl_double precision numbers */
#define AEPSGE (sisl_double)1.e-6
#define AEPSCO (sisl_double)0.0

#define REL_COMP_RES                   (sisl_double)0.000000000000001
#define REL_PAR_RES                    (sisl_double)0.000000000001
#define ANGULAR_TOLERANCE              (sisl_double)0.01  /* IN RADIANS */
#define MAXIMAL_RADIUS_OF_CURVATURE    (sisl_double)10000.0

/*
 * Name of the nullobject, what a pointer points to when
 * it points to  nothing.
 */

#ifndef SISL_NULL
#define SISL_NULL 0
#endif

/* Name of sisl_double 0.0 */

#ifndef DZERO
#define DZERO (sisl_double)0.0
#endif


#define ONE_THIRD  (sisl_double)0.333333333333333333333333
#define ONE_SIXTH  (sisl_double)0.166666666666666666666667
#define ONE_FOURTH (sisl_double)0.25

/* Functions taking max/min of two arguments. */

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX max
#endif
#ifndef MIN
#define MIN min
#endif

/* Macros checking for equality/non-equality  between two sisl_double numbers */

#define DEQUAL(a,b) \
  ( (fabs((a) - (b)) <= (REL_PAR_RES * MAX(fabs(a),fabs(b)))) ? (1) : (0) )

#define DNEQUAL(a,b) \
  ( (fabs((a) - (b)) >  (REL_PAR_RES * MAX(fabs(a),fabs(b)))) ? (1) : (0) )

#define ZEROLEN(a) \
  ( (fabs(a) <= AEPSGE) ? (1) : (0) )

/* Space allocation.  */

#if defined(SISL_INTERNAL) || defined(USING_SISL_NEWARRAY)

#define newarray(a,b) \
  ((a)>(SISL_NULL)?((b*)MALLOC((size_t)((a)*sizeof(b)),"s curves")):(SISL_NULL))


inline void *curves_calloc( u32 number, u32 size )
{
	void * ret = MALLOC( number * size , "s curves" );
	memset(ret, 0, number * size);
	return ret;
}

#define new0array(a,b) \
  ((a)>(SISL_NULL)?((b*)curves_calloc((size_t)(a),(size_t)(sizeof(b)))):(SISL_NULL))



#define increasearray(a,b,c) (c*)REALLOC((void*)(a),(size_t)((b)*sizeof(c)),"s curves")

#define freearray(a) { (void)FREE(a); a = SISL_NULL; }

#define free0array(a) { (void)FREE(a); a = SISL_NULL; }


/* Copying of arrays   */

#define memcopy(a,b,c,d) \
  (void*) memcpy((void*) (a),(const void *)(b),(size_t)((c)*sizeof(d)))

/* Zero out an array   */

#define memzero(a,b,c) \
  (void*) memset((void*) (a),0,(size_t)((b)*sizeof(c)))

#endif /* SISL_INTERNAL || USING_SISL_NEWARRAY */

/* Set value of PI/2, PI, 3PI/2 and 2PI */

#ifndef PIHALF
#define PIHALF       (sisl_double)M_PI_2
#endif
#ifndef PI
#define PI           (sisl_double)M_PI
#endif
/* #ifndef PIHALF
   #define PIHALF       (sisl_double)1.57079632679489661923
   #endif
   #ifndef PI
   #define PI           (sisl_double)3.14159265358979323846
   #endif                            */
#ifndef THREEPIHALF
#define THREEPIHALF  (sisl_double)4.71238898038468985769
#endif
#ifndef TWOPI
#define TWOPI        (sisl_double)6.28318530717958647692
#endif
#ifndef ROTM
/* #define ROTM         (sisl_double)0.70710678118654752440 */
#define ROTM         (sisl_double)M_SQRT1_2
#endif
#ifndef SIMPLECASE
#define SIMPLECASE   (sisl_double)0.75
#endif

/* Values of logicals */





/*
 * --------------------------------------------------------------
 * Forward declarations of all sisl low-level routines, all in C.
 * --------------------------------------------------------------
 */


#if defined(SISLNEEDPROTOTYPES)

#ifndef CONSTRUCT
extern
#endif
SISLIntpt    *copyIntpt(SISLIntpt *);
#ifndef CONSTRUCT
extern
#endif
SISLEdge     *newEdge(int);
#ifndef CONSTRUCT
extern
#endif
SISLIntdat   *newIntdat(void);
#ifndef CONSTRUCT
extern
#endif
SISLIntlist  *newIntlist(SISLIntpt *,SISLIntpt *,int);
#ifndef CONSTRUCT
extern
#endif
SISLIntpt    *newIntpt(int,sisl_double *,sisl_double);
#ifndef CONSTRUCT
extern
#endif
SISLObject   *newObject(int);
#ifndef CONSTRUCT
extern
#endif
SISLPoint    *newPoint(sisl_double *,int,int);
#ifndef CONSTRUCT
extern
#endif
SISLPtedge   *newPtedge(SISLIntpt *);
#ifndef CONSTRUCT
extern
#endif
SISLIntsurf  *newIntsurf(SISLIntlist *);
#ifndef CONSTRUCT
extern
#endif
SISLTrack    *newTrack(SISLSurf *,SISLSurf *,SISLCurve *,SISLCurve *,
		       SISLCurve *,int,sisl_double[],int,int,int);
#ifndef CONSTRUCT
extern
#endif
SISLTrimpar  *newTrimpar(int,int);
#ifndef CONSTRUCT
extern
#endif
SISLIntpt    *hp_newIntpt(int,sisl_double *,sisl_double,int,int,int,int,int,
			  int,int,sisl_double [],sisl_double []);
#ifndef CONSTRUCT
extern
#endif
SISLIntpt    *hp_copyIntpt(SISLIntpt *);
#ifndef DESTRUCT
extern
#endif
void freeEdge(SISLEdge *);
#ifndef DESTRUCT
extern
#endif
void freeIntdat(SISLIntdat *);
#ifndef DESTRUCT
extern
#endif
void freeIntlist(SISLIntlist *);
#ifndef DESTRUCT
extern
#endif
void freeIntpt(SISLIntpt *);
#ifndef DESTRUCT
extern
#endif
void freeObject(SISLObject *);
#ifndef DESTRUCT
extern
#endif
void freePoint(SISLPoint *);
#ifndef DESTRUCT
extern
#endif
void freePtedge(SISLPtedge *);
#ifndef DESTRUCT
extern
#endif
void freeIntsurf(SISLIntsurf *);
#ifndef DESTRUCT
extern
#endif
void freeTrimpar(SISLTrimpar *);
#ifndef DESTRUCT
extern
#endif
void freeTrack(SISLTrack * ptrack);
#ifndef MAKE3D
extern
#endif
void make3D(SISLSurf *, SISLSurf **, int *);
#ifndef S1119
extern
#endif
void s1119(sisl_double *,sisl_double *,sisl_double *,int,int,int,int,int *,int *,int *,int *);
#ifndef S1161
extern
#endif
void s1161(SISLObject *,sisl_double *,sisl_double ,SISLIntdat **,int *);
#ifndef S1162
extern
#endif
void s1162(SISLObject *,sisl_double *,sisl_double,SISLIntdat **,SISLEdge *[2],int,
	   int,int *);
#ifndef S1172
extern
#endif
void s1172(SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double *,int *);
#ifndef S1173
extern
#endif
void s1173(SISLPoint *,SISLSurf *,sisl_double,sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],int *);
#ifndef S1174
extern
#endif
void s1174(SISLSurf *,sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef S1190
extern
#endif
void s1190(SISLObject *, sisl_double *, sisl_double, int *);
#ifndef  S1192
extern
#endif
void s1192(SISLObject *, sisl_double, int *);
#ifndef  S1219
extern
#endif
void s1219(sisl_double *,int,int,int *,sisl_double,int *);
#ifndef  S1220
extern
#endif
void s1220(sisl_double *,int,int,int *,sisl_double,int,sisl_double [],int *);
#ifndef  S1222
extern
#endif
void s1222(sisl_double *,int,int,int,sisl_double,int,sisl_double [],int *);
#ifndef  S1223
extern
#endif
void s1223(sisl_double [],sisl_double [],int,int,int,int,int,int,sisl_double [],
	   int,int,sisl_double [],int *);
#ifndef  S1224
extern
#endif
void s1224(sisl_double [],sisl_double [],int,int,int,int,int, int,sisl_double [],int,
	   sisl_double [],int *);
#ifndef  S1231
extern
#endif
void s1231(SISLCurve *,sisl_double,SISLCurve **,SISLCurve **,int *);
#ifndef  S1232
extern
#endif
void s1232(sisl_double [],int,int,sisl_double,sisl_double,sisl_double [],int *);
#ifndef  S1235
extern
#endif
void s1235(sisl_double [],int,int,int *,sisl_double **,int *);
#ifndef  S1236
extern
#endif
void s1236(sisl_double [],int,int,int,sisl_double [],int *);
#ifndef  S1239
extern
#endif
void s1239(SISLCurve *,int,sisl_double,SISLCurve *,sisl_double,sisl_double,SISLCurve **,
	   int,int *,int *);
#ifndef  S1244
extern
#endif
void s1244(sisl_double [],int,int,int,int,int,int,sisl_double *,int *);
#ifndef  S1245
extern
#endif
void s1245(sisl_double [],int,int,sisl_double [],sisl_double,int,sisl_double [],sisl_double *,
           sisl_double *,int *);
#ifndef  S1251
extern
#endif
void s1251(SISLCurve *,sisl_double,sisl_double *,int *);
#ifndef  S1252
extern
#endif
void s1252(SISLCurve *,sisl_double,sisl_double,sisl_double *,int *);
#ifndef  S1301
extern
#endif
void s1301(sisl_double,sisl_double,int,SISLCurve **,int *);
#ifndef  S1304
extern
#endif
void s1304(sisl_double [],sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S1305
extern
#endif
void s1305(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *,sisl_double [],int *);
#ifndef  S1306
extern
#endif
void s1306(sisl_double [],sisl_double [],sisl_double [],int,sisl_double [],sisl_double [],int *);
#ifndef  S1307
extern
#endif
void s1307(sisl_double [],int,sisl_double [],int *);
#ifndef  S1308
extern
#endif
void s1308(sisl_double [],int,sisl_double [],int,sisl_double [],int *);
#ifndef  S1309
extern
#endif
sisl_double s1309(sisl_double [],sisl_double [],sisl_double [],int,int *);
#ifndef  S1311
extern
#endif
sisl_double s1311(sisl_double,sisl_double,sisl_double,int *);
#ifndef  S1312
extern
#endif
void s1312(sisl_double [],int,int,int,sisl_double [],SISLCurve **,int *);
#ifndef  S1313
extern
#endif
void s1313(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,sisl_double,
	   SISLIntcurve *,int,int,int *);
#ifndef  S1320
extern
#endif
void s1320(SISLSurf *,sisl_double [],int,int,SISLSurf **,int *);
#ifndef  S1321
extern
#endif
void s1321(sisl_double [],sisl_double,int,int,sisl_double [],int *);
#ifndef  S1322
extern
#endif
void s1322(sisl_double [],sisl_double [],sisl_double,int,int,sisl_double [],int *);
#ifndef  S1323
extern
#endif
void s1323(sisl_double [],sisl_double [],sisl_double [],int,int,sisl_double [],int *);
#ifndef  S1324
extern
#endif
void s1324(sisl_double [],sisl_double,sisl_double [],int,sisl_double [],int *);
#ifndef  S1325
extern
#endif
sisl_double s1325(sisl_double,sisl_double);
#ifndef  S1329
extern
#endif
void s1329(SISLSurf *,sisl_double [],sisl_double [],int,SISLSurf **,int *);
#ifndef  S1330
extern
#endif
void s1330(sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],sisl_double [],int *,sisl_double [],sisl_double [],int *);
#ifndef  S1331
extern
#endif
void s1331(sisl_double [],sisl_double [],int,int,sisl_double [],sisl_double [],int *);
#ifndef  S1333
extern
#endif
void s1333(int,SISLCurve *[],int [],sisl_double,int,int,int,SISLSurf **,
	   sisl_double **,int *);
#ifndef  S1333_COUNT
extern
#endif
void s1333_count(int,SISLCurve *[],int *,int *);
#ifndef  S1333_CYCLIC
extern
#endif
void s1333_cyclic(SISLSurf *,int,int *);
#ifndef  S1334
extern
#endif
void s1334(sisl_double [],int,int,sisl_double [],int,int,int,int,sisl_double,
	   sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef  S1340
extern
#endif
void s1340(SISLCurve *,sisl_double [],int,int,sisl_double,int,SISLCurve **,
	   sisl_double [],int *);
#ifndef  S1341
extern
#endif
void s1341(sisl_double [],int,int,int,sisl_double [],sisl_double [],int,int,
	   sisl_double,sisl_double,int,int,SISLCurve **,sisl_double [],int *);
#ifndef  S1342
extern
#endif
void s1342(sisl_double [],sisl_double [],int,int,int,sisl_double [],
	   sisl_double [],int,int,sisl_double,int,SISLCurve **,sisl_double [],int *);
#ifndef  S1343
extern
#endif
void s1343(SISLCurve *,sisl_double [],int,int,sisl_double,int,SISLCurve **,int *);
#ifndef  S1345
extern
#endif
void s1345(SISLSurf *,sisl_double [],int [],sisl_double [],sisl_double,int,int,
	   SISLSurf **,sisl_double [],int *);
#ifndef  S1346
extern
#endif
void s1346(sisl_double [],int,int,int,int,sisl_double [],sisl_double [],sisl_double [],
	   int [],sisl_double [],sisl_double,sisl_double,int,int,int,int,
	   SISLSurf **,sisl_double [],int *);
#ifndef  S1347
extern
#endif
void s1347(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int,int,int,
	   sisl_double [],sisl_double [],sisl_double [],int [],sisl_double [],sisl_double,int,
	   int,SISLSurf **,sisl_double [],int *);
#ifndef  S1348
extern
#endif
void s1348(SISLSurf *,sisl_double [],int [],sisl_double [],sisl_double,int,int,
	   SISLSurf **,int *);
#ifndef  S1349
extern
#endif
void s1349(int,SISLCurve *[],int *);
#ifndef  S1350
extern
#endif
void s1350(sisl_double [],sisl_double [],int,int,int,SISLCurve **,int *);
#ifndef  S1351
extern
#endif
void s1351(sisl_double [],int,int,int,int,SISLCurve **,int *);
#ifndef  S1352
extern
#endif
void  s1352(sisl_double [],int,int,sisl_double [],sisl_double [],
	    sisl_double [],int,int,int,sisl_double [],int *);
#ifndef  S1353
extern
#endif
void s1353(SISLCurve *,sisl_double [],rank_info *,int *);
#ifndef  S1354
extern
#endif
void s1354(SISLCurve *,SISLCurve *,rank_info *,sisl_double [],
	   sisl_double [],int,int,int,int,SISLCurve **,sisl_double [],int *);
#ifndef  S1355
extern
#endif
void s1355(SISLCurve *,sisl_double [],sisl_double **,int *,int *);
#ifndef  S1358
extern
#endif
void s1358(sisl_double [],int,int,sisl_double [],sisl_double [],int,int,int,int,sisl_double,
	   sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef  S1359
extern
#endif
void s1359(sisl_double [],sisl_double,int,int,int,sisl_double [],SISLCurve **,int *);
#ifndef  S1361
extern
#endif
void s1361(sisl_double [],sisl_double [],int,sisl_double [],sisl_double [],int *);
#ifndef  S1362
extern
#endif
void s1362(SISLCurve *,sisl_double,sisl_double [],int,int,sisl_double,int *,sisl_double [],int *);
#ifndef S1366
extern
#endif
void s1366(SISLSurf *,sisl_double,sisl_double,sisl_double,int,
	   sisl_double *,int,int,sisl_double *,int,int,SISLSurf **,int *);
#ifndef S1367
extern
#endif
void s1367(SISLSurf *,sisl_double,sisl_double,int,sisl_double [],
	   int,int *,int *,sisl_double [],int *);
#ifndef  S1370
extern
#endif
void s1370(SISLCurve *,sisl_double [],int,int,int,SISLCurve **,int *);
#ifndef  S1376
extern
#endif
void s1376(sisl_double [],int,int,sisl_double **,int *,int *,int *);
#ifndef  S1377
extern
#endif
void s1377(SISLCurve *,sisl_double [],int,int,SISLCurve **,int *);
#ifndef  S1378
extern
#endif
void s1378(SISLSurf *,sisl_double [],int,int,SISLSurf **,int *);
#ifndef  S1381
extern
#endif
void s1381(sisl_double [],int,int,sisl_double **,int *,int,int *);
#ifndef  S1382
extern
#endif
void s1382(SISLSurf *,sisl_double [],int,SISLSurf **,int *);
#ifndef  S1384
extern
#endif
void s1384(SISLCurve *,SISLSurf *,int,int,sisl_double,int *,int *,int *,sisl_double [],
	   sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S1385
extern
#endif
void s1385(sisl_double [],sisl_double [],sisl_double [],sisl_double,int,sisl_double,SISLCurve **,int *);
#ifndef  S1393
extern
#endif
void s1393(int,SISLCurve *[],SISLCurve *[],SISLCurve *[],int *);
#ifndef  S1399
extern
#endif
void s1399(SISLCurve *,sisl_double,sisl_double);
#ifndef  S1435
extern
#endif
void s1435(SISLSurf *,int,SISLCurve **,sisl_double *,int *);
#ifndef  S1436
extern
#endif
void s1436(SISLSurf *,sisl_double,SISLCurve **,int *);
#ifndef  S1437
extern
#endif
void s1437(SISLSurf *,sisl_double,SISLCurve **,int *);
#ifndef  S1438
extern
#endif
void s1438(SISLCurve  *,int,SISLPoint **,sisl_double *,int *);
#ifndef  S1452
extern
#endif
void s1452(SISLSurf *,sisl_double,sisl_double,SISLSurf **,int *);
#ifndef S1500
extern
#endif
void s1500(sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double,int,int,
	   sisl_double [],int *);
#ifndef S1504
extern
#endif
void s1504(sisl_double *,int,int,sisl_double *,int,int,sisl_double [],int [],int *);
#ifndef S1505
extern
#endif
void s1505(const SISLSurf *,int,int,int,sisl_double *,sisl_double *, int *,
           int *,sisl_double [],sisl_double [],int *);
#ifndef S1507
extern
#endif
void s1507(SISLCurve **, int, int, SISLCurve ***, int *);
#ifndef S1512
extern
#endif
void s1512(SISLSurf *,sisl_double [],int,SISLSurf **,int *);
#ifndef S1513
extern
#endif
void s1513(SISLSurf *,sisl_double [],sisl_double [],int,SISLSurf **,int *);
#ifndef S1516
extern
#endif
void s1516(sisl_double [],sisl_double [],int,int,sisl_double **,int *);
#ifndef S1517
extern
#endif
void s1517(sisl_double [],sisl_double [],sisl_double [],int,sisl_double,sisl_double **,int *);
#ifndef  S1520
extern
#endif
void s1520(SISLCurve *,sisl_double,sisl_double [],sisl_double [],SISLSurf **,int *);
#ifndef S1521
extern
#endif
SISLCurve* s1521(SISLCurve *,int *);
#ifndef S1528
extern
#endif
void s1528(int,int,int,sisl_double [],int,int,int,sisl_double **,sisl_double **,int *);
#ifndef S1531
extern
#endif
void s1531(sisl_double [],int,int,int,sisl_double **,int *);
#ifndef S1540
extern
#endif
void s1540(sisl_double [], int, int, sisl_double [], int, int, sisl_double [], int [], int *);
#ifndef S1541
extern
#endif
void s1541(SISLCurve *, int, sisl_double [], int [], sisl_double [], int *);
#ifndef  S1604
extern
#endif
void s1604(sisl_double [],int,sisl_double,int,int,int,SISLCurve **,int *);
#ifndef  S1605
extern
#endif
void s1605(SISLCurve *,sisl_double,sisl_double **,int *,int *);
#ifndef  S1612
extern
#endif
void s1612(SISLCurve *,sisl_double,sisl_double **,int *,int *,int *);
#ifndef  S1613BEZ
extern
#endif
void s1613bez(SISLCurve *,int,sisl_double,sisl_double **,int *,int *);
#ifndef S1614
extern
#endif
void s1614(sisl_double [],int,int,int [],sisl_double [],int *,int [],int *);
#ifndef S1615
extern
#endif
void s1615(sisl_double [],int,int,int [],int *);
#ifndef S1616
extern
#endif
void s1616(sisl_double [],int,int,int [],sisl_double [],int *);
#ifndef S1617
extern
#endif
void s1617(sisl_double [],int,int,int [],sisl_double,sisl_double [],
           sisl_double [],sisl_double [],sisl_double [],sisl_double *,int *);
#ifndef S1618
extern
#endif
void s1618(sisl_double [],sisl_double [],sisl_double[],int,sisl_double *);
#ifndef S1619
extern
#endif
void s1619(sisl_double [],int,int,int [],sisl_double [],int,sisl_double[],sisl_double *, int *);
#ifndef  S1700
extern
#endif
void s1700(int,int,int,int,int *,int *,int *,sisl_double *,sisl_double,sisl_double *,int *);
#ifndef  S1701
extern
#endif
void s1701(int,int,int,int,int *,int *,int *,
	   sisl_double *,sisl_double *,sisl_double *,sisl_double *,int *);
#ifndef  S1705
extern
#endif
void s1705(SISLCurve *,int *);
#ifndef  S1707
extern
#endif
void s1707(SISLCurve *,int *);
#ifndef  S1708
extern
#endif
void s1708(SISLSurf *,int *);
#ifndef  S1741
extern
#endif
void s1741(SISLObject *,SISLObject *,sisl_double,int *);
#ifndef S1753
extern
#endif
void
s1753(sisl_double [],sisl_double [],int,int,int,sisl_double [],sisl_double [],int,
      sisl_double [],sisl_double [],int *);
#ifndef S1754
extern
#endif
void
s1754(sisl_double *,int,int,int,sisl_double **,int *,int *);
#ifndef S1755
extern
#endif
void
s1755(sisl_double [],int,int,sisl_double [],int *,int *);
#ifndef  S1770
extern
#endif
void s1770(SISLCurve *,SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double,sisl_double,
	   sisl_double,sisl_double,sisl_double *,sisl_double *,int *);
#ifndef  S1770_2D
extern
#endif
   void s1770_2D(SISLCurve *,SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double,sisl_double,
		 sisl_double,sisl_double,sisl_double *,sisl_double *,int *);
#ifndef  S1771
extern
#endif
void s1771(SISLPoint *,SISLCurve *,sisl_double,sisl_double,sisl_double,sisl_double,sisl_double *,int *);
#ifndef  S1772
extern
#endif
void s1772(SISLCurve *,SISLSurf *,sisl_double,sisl_double,sisl_double [],sisl_double,sisl_double [],
	   sisl_double,sisl_double [],sisl_double *,sisl_double [],int *);
#ifndef  S1773
extern
#endif
void s1773(SISLPoint *,SISLSurf *,sisl_double,sisl_double [],sisl_double [],sisl_double [],
	   sisl_double [],int *);
#ifndef  S1780
extern
#endif
void s1780(SISLCurve *,SISLCurve *,SISLIntpt *[],int *);
#ifndef  S1785
extern
#endif
void s1785(SISLCurve *,SISLSurf *,sisl_double,sisl_double [],sisl_double [],int,int *);
#ifndef  S1786
extern
#endif
void s1786(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],sisl_double [],int *);
#ifndef  S1787
extern
#endif
void s1787(SISLSurf *,sisl_double,sisl_double,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S1788
extern
#endif
void s1788(SISLSurf *,SISLSurf *,sisl_double,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S1789
extern
#endif
void  s1789(SISLPoint *,SISLSurf *,sisl_double ,sisl_double [],sisl_double [],int *);
#ifndef  S1790
extern
#endif
void s1790(SISLObject *,SISLObject *,sisl_double,int *);
#ifndef  S1791
extern
#endif
int s1791(sisl_double [],int,int);
#ifndef  S1792
extern
#endif
sisl_double s1792(sisl_double [],int,int);
#ifndef  S1795
extern
#endif
void s1795(SISLSurf *,SISLSurf *,sisl_double,sisl_double,int *);
#ifndef  S1796
extern
#endif
void s1796(SISLCurve *,SISLCurve *,sisl_double,sisl_double,int *);
#ifndef  S1797
extern
#endif
void s1797(SISLSurf *,SISLCurve *,sisl_double,sisl_double,int *);
#ifndef  S1830
extern
#endif
void s1830(SISLSurf *,SISLCurve *,int *);
#ifndef  S1834
extern
#endif
void s1834(sisl_double [],int,sisl_double [],int,int,sisl_double [],sisl_double [],int *);
#ifndef  S1839
extern
#endif
void s1839(SISLSurf *,sisl_double [],int,int,int *);
#ifndef  S1840
extern
#endif
void s1840(SISLCurve *,sisl_double *,int *);
#ifndef  S1880
extern
#endif
void s1880(int,int,int *,SISLIntpt **,int *,SISLIntlist **,
	   int *,sisl_double **,sisl_double **,int *,SISLIntcurve ***,int *);
#ifndef S1890
extern
#endif
void
s1890(sisl_double [],int,int,sisl_double *[],int *[],int *);
#ifndef S1891
extern
#endif
void
s1891(sisl_double [],sisl_double [],int,int,int,int [],int,sisl_double [],
      sisl_double *[],int *,int,int,int,int *);
#ifndef S1893
extern
#endif
void
s1893(SISLCurve *,sisl_double [],int,int,int,int,SISLCurve **,int *);
#ifndef S1894
extern
#endif
void
s1894(sisl_double [],int,int,int,int,sisl_double [],int,int,sisl_double *[],int *,
      int *,int *);
#ifndef S1896
extern
#endif
void
s1896(SISLSurf *,sisl_double [],int,int,int [],int [],int [],int [],SISLSurf **,int *);
#ifndef S1897
extern
#endif
void
s1897(sisl_double [],int,sisl_double,int,int,sisl_double [],int *);
#ifndef S1900
extern
#endif
void
s1900(sisl_double [],sisl_double [],sisl_double [],int [],int,int,int,int,
      sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef S1901
extern
#endif
void
s1901(void (*)(sisl_double[],int[],int,int,int,sisl_double,sisl_double *,
	       sisl_double *[],sisl_double *[],int *),
      void (*)(sisl_double[],int,int,int,sisl_double *[],int *),
      sisl_double [],int [],int,sisl_double,int,int,int,
      sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef S1902
extern
#endif
void
s1902(sisl_double [],int,int,int,sisl_double *[],int *);
#ifndef S1903
extern
#endif
void
s1903(sisl_double [],int,int,int,sisl_double *[],int *);
#ifndef S1904
extern
#endif
void
s1904(sisl_double [],int,int,int,sisl_double *[],int *);
#ifndef S1905
extern
#endif
void
s1905(sisl_double [],int [],int,int,int,int,sisl_double **,int **,int *,int *);
#ifndef S1906
extern
#endif
void
s1906(sisl_double *,int *,int,int,int,int,sisl_double **,int **,int *,int *);
#ifndef S1907
extern
#endif
void
s1907(sisl_double *,int *,sisl_double *,int,int,int,int,int,
      sisl_double *[],int *[],sisl_double *[],int *,int *);
#ifndef S1908
extern
#endif
void
s1908(sisl_double [],int [],sisl_double [],int,int,int,int,
      sisl_double *[],int *[],sisl_double *[],int *,int *);
#ifndef S1909
extern
#endif
void
s1909(sisl_double [],int [],int,int,int,sisl_double,sisl_double *,sisl_double *[],
      sisl_double *[],int *);
#ifndef S1910
extern
#endif
void
s1910(sisl_double [],int [],int,int,int,sisl_double,sisl_double *,sisl_double *[],
      sisl_double *[],int *);
#ifndef S1911
extern
#endif
void
s1911(sisl_double [],int [],int,int,int,sisl_double,sisl_double *,sisl_double *[],
      sisl_double *[],int *);
#ifndef S1912
extern
#endif
void
s1912(void (*)(sisl_double[],int[],int,int,int,sisl_double,sisl_double *,
	       sisl_double *[],sisl_double *[],int *),
      void (*)(sisl_double[],int,int,int,sisl_double *[],int *),
      sisl_double [],int [],int,sisl_double,int,int,int,
      sisl_double *,SISLCurve **,sisl_double **,int *,int *);
#ifndef S1916
extern
#endif
void
s1916(int,sisl_double [],sisl_double [],int,int,int,int,sisl_double [],int [],int *);
#ifndef S1917
extern
#endif
void
s1917(int,sisl_double [],int,int,int [],sisl_double,int,sisl_double *[],int *[],int *i,int *);
#ifndef S1918
extern
#endif
void
s1918(int,sisl_double [],sisl_double [],int,int,int,sisl_double [],int [],int *);
#ifndef S1919
extern
#endif
void
s1919(sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int,int,int,int,
      sisl_double,sisl_double,sisl_double,int *);
#ifndef S1924
extern
#endif
void
s1924(int,int,int,int,int,int,sisl_double **,int *);
#ifndef S1925
extern
#endif
void
s1925(sisl_double [],sisl_double [],int,int [],sisl_double [],sisl_double [],int,int,int,int,
      sisl_double [],int,int [],sisl_double [],int,sisl_double [],int,int *);
#ifndef S1926
extern
#endif
void
s1926(sisl_double *,int,int,int *,sisl_double *,int,sisl_double *,int,int *);
#ifndef S1927
extern
#endif
void
s1927(sisl_double *,int,int,int *,sisl_double *,int,sisl_double *,int,sisl_double *[],
      sisl_double *,int *);
#ifndef S1931
extern
#endif
void
s1931(int,SISLCurve **,sisl_double **,sisl_double **,int *,int *,int *);
#ifndef S1931UNIT
extern
#endif
void s1931unit (int,SISLCurve **,sisl_double **,sisl_double **,int *,int *,int *);
#ifndef S1932
extern
#endif
void
s1932(int,SISLCurve **,sisl_double,sisl_double,sisl_double *,int,int,sisl_double **,int *);
#ifndef S1933
extern
#endif
void
s1933(int,SISLCurve *[],sisl_double,sisl_double,sisl_double **,int *,int *,int *);
#ifndef S1934
extern
#endif
void
s1934(sisl_double *,int,int,sisl_double,sisl_double,int *);
#ifndef S1935
extern
#endif
void
s1935(sisl_double *,int,sisl_double *,int,sisl_double *[],int *,int,int *);
#ifndef S1936
extern
#endif
void
s1936(SISLCurve *,sisl_double [],int,sisl_double *,int *);
#ifndef S1937
extern
#endif
void
s1937(sisl_double [],int,int,int,sisl_double [],sisl_double []);
#ifndef S1938
extern
#endif
void
s1938(SISLSurf *,sisl_double [],int,sisl_double [],int,sisl_double **,int *);
#ifndef S1941
extern
#endif
void
s1941(SISLCurve *pcurve,int icont,int *jstat);
#ifndef S1942
extern
#endif
void s1942(SISLCurve *pc1,SISLCurve *pc2,int idim,sisl_double ea[],int nstart[],
             int nstop[],sisl_double emxerr[],sisl_double el2err[],int *jstat);
#ifndef S1943
extern
#endif
void
   s1943(SISLCurve *pcurve,sisl_double etau[],int ik,int in,int ileftfix,
             int irightfix,int incont,SISLCurve **rnewcurve,
             sisl_double gmaxerr[],sisl_double gl2err[],int *jstat);
#ifndef S1944
extern
#endif
void
   s1944(sisl_double etau[],int ik,int in,int idim,sisl_double et[],sisl_double ed[],
             int im,int inlc,int inlr,int inorm,sisl_double ea[],sisl_double ew1[],
             int nfirst[],int nlast[],sisl_double eb[],sisl_double ew2[],
             int n2sta[],sisl_double ec[],int *jstat);
#ifndef S1945
extern
#endif
void
   s1945(sisl_double etau[],int ik,int in,int idim,sisl_double et[],
             sisl_double ed[],int im,int ilend,int irend, int inlc,
             int inlr, int inorm, sisl_double ea[], sisl_double ew1[], int inh,
             int nfirst[],int nlast[],sisl_double eb[],
             sisl_double ew2[], sisl_double ec[],int n2sta[],int *jstat);
#ifndef S1946
extern
#endif
void
   s1946(sisl_double ea[],sisl_double ew1[],int nfirst[],int nlast[],sisl_double ed[],
         sisl_double ec[],int ik,int in,int im,int idim,int ilend,int irend,
         int inlr, int inlc,int *jstat);
#ifndef S1947
extern
#endif
void
   s1947(sisl_double ea[], int nfirst[], int nlast[], int ik, int im,
         sisl_double etau[], int in, int incont, sisl_double ew[], int inlr,
         int *jnred, sisl_double efac[], int *jstat);
#ifndef S1948
extern
#endif
void
   s1948(sisl_double *ea,sisl_double *ew,int in,int ik,int inlr,
         int *nstart,int *jstat);
#ifndef S1949
extern
#endif
void
   s1949(sisl_double *ea,sisl_double *ew,sisl_double *eb,int in,int ik,int inlr,
         int idim,int *nstart,int *jstat);
#ifndef S1950
extern
#endif
void s1950(SISLCurve *oldcurve,SISLCurve *rankcurve,rank_info *ranking,
           sisl_double eps[],sisl_double epsco[],int startfix,int endfix,int *jncont,
           int mini,int maxi,SISLCurve **newcurve,sisl_double maxerr[],int *stat);
#ifndef S1951
extern
#endif
void
   s1951(sisl_double etau[], sisl_double ecoef[], int in, int ik, int idim,
         int ilend, int irend, int incont, sisl_double efac[]);
#ifndef  S1956
extern
#endif
void s1956(SISLCurve *,SISLCurve *,SISLSurf **,int *);
#ifndef  S1959
extern
#endif
void s1959(SISLPoint *, SISLCurve *, sisl_double *, int *);
#ifndef  S1960
extern
#endif
void s1960(SISLPoint *, SISLSurf *, sisl_double [], int *);
#ifndef S1990
extern
#endif
void s1990(SISLSurf *, sisl_double, int *);
#ifndef  S1991
extern
#endif
void s1991(SISLCurve *,sisl_double,int *);
#ifndef  S1992
extern
#endif
void s1992(SISLObject *,int *);
#ifndef  S1992
extern
#endif
void s1992cu(SISLCurve *,int *);
#ifndef  S1992
extern
#endif
void s1992su(SISLSurf *,int *);
#ifndef  S1993
extern
#endif
void s1993(SISLCurve *,int *);
#ifndef  S1994
extern
#endif
void s1994(SISLSurf *,int *);
#ifndef  S2501
extern
#endif
void s2501(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2503
extern
#endif
void s2503(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2505
extern
#endif
void s2505(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2507
extern
#endif
void s2507(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2509
extern
#endif
void s2509(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2511
extern
#endif
void s2511(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, int *);
#ifndef  S2512
extern
#endif
void s2512(SISLSurf *, int, int, int, sisl_double [], int *, int *, sisl_double [], int *);
#ifndef  S2513
extern
#endif
void s2513(SISLSurf *, int, int, int, sisl_double [], sisl_double [], sisl_double [], int *);
#ifndef  S2514
extern
#endif
void s2514(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double [], int *);
#ifndef  S2515
extern
#endif
void s2515(SISLSurf *, int, int, int, sisl_double [], int *,int *, sisl_double [], int *);
#ifndef  S2516
extern
#endif
void s2516(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double [], int *);
#ifndef  S2533
extern
#endif
void s2533(sisl_double *, int, int, int, int, int *, sisl_double **, int *);
#ifndef  S2534
extern
#endif
void s2534(SISLSurf *, int, int, int, int,
           void evalp(SISLSurf *, int, int, int,
		     sisl_double [], int *, int *,
		     sisl_double *, int *),
	 int, SISLSurf **, int *);
#ifndef  S2535
extern
#endif
void s2535(SISLSurf *, int, int, int *, int *, SISLSurf ***, int *);
#ifndef  S2541
extern
#endif
void s2541(SISLSurf *surf,
           void evalp(SISLSurf *, int, int, int, sisl_double [], int *, int *,
		      sisl_double *, int *),
	 int, int, int, sisl_double [], int, int, sisl_double **, int *);
#ifndef  S2543
extern
#endif
void s2543(SISLSurf *, int, sisl_double [], sisl_double [], sisl_double *, sisl_double *,
	   sisl_double [], sisl_double [], int *);
#ifndef  S2551
extern
#endif
void s2551(SISLCurve *, sisl_double, int *, sisl_double [], sisl_double *, int *);
#ifndef  S2554
extern
#endif
void s2554(SISLCurve *, sisl_double, int *, sisl_double [], sisl_double *, int *);
#ifndef  S2555
extern
#endif
void s2555(sisl_double [], sisl_double *, int *);
#ifndef  S2557
extern
#endif
void s2557(SISLCurve *, sisl_double, int *, sisl_double [], sisl_double *, int *);
#ifndef  S2558
extern
#endif
void s2558(sisl_double [], int, sisl_double *, int *);
#ifndef  S2560
extern
#endif
void s2560(SISLCurve *, sisl_double, int *, sisl_double [], sisl_double [], sisl_double [],
           sisl_double [], sisl_double [], int *);
#ifndef  S2561
extern
#endif
void s2561(sisl_double [], int, sisl_double [], sisl_double [], sisl_double [], sisl_double [], int *);
#ifndef  S6ADDCURVE
extern
#endif
void s6addcurve(SISLCurve *,SISLCurve *,int,SISLCurve **,int *);
#ifndef  S6AFFDIST
extern
#endif
sisl_double s6affdist(sisl_double [],sisl_double [],sisl_double [],int);
#ifndef  S6ANG
extern
#endif
sisl_double s6ang(sisl_double [],sisl_double [],int);
#ifndef  S6ANGLE
extern
#endif
sisl_double s6angle(sisl_double [],sisl_double [],sisl_double [],int,int *);
#ifndef  S6CHPAR
extern
#endif
void s6chpar(sisl_double [],int,int,int,sisl_double []);
#ifndef  S6CRSS
extern
#endif
void s6crss(sisl_double [],sisl_double [],sisl_double []);
#ifndef  S6CRVCHECK
extern
#endif
void s6crvcheck(SISLCurve *,int *);
#ifndef  S6CURVATURE
extern
#endif
void s6curvature(sisl_double [],int,sisl_double [],int *);
#ifndef  s6CURVRAD
extern
#endif
void s6curvrad(sisl_double [],sisl_double [],sisl_double [],int,sisl_double *,int *);
#ifndef  S6DECASTELJAU
extern
#endif
void     s6deCasteljau(sisl_double [],sisl_double,sisl_double,sisl_double,int,sisl_double [],int *);
#ifndef  S6DECOMP
extern
#endif
void s6decomp(sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S6DEGNORM
extern
#endif
void s6degnorm(SISLSurf *,int,sisl_double[],sisl_double[],sisl_double[],sisl_double[],
	       sisl_double[],int *);
#ifndef  S6DERTOPT
extern
#endif
void s6dertopt(sisl_double [],int [],int,int,sisl_double [],int *);
#ifndef  S6DIFF
extern
#endif
void s6diff(sisl_double [],sisl_double [],int,sisl_double []);
#ifndef  S6DIST
extern
#endif
sisl_double s6dist(sisl_double [],sisl_double [],int);
#ifndef  S6DLINE
extern
#endif
sisl_double s6dline(sisl_double [],sisl_double [],sisl_double [],int,int *);
#ifndef  S6DPLANE
extern
#endif
sisl_double  s6dplane(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int *);
#ifndef  S6EQUAL
extern
#endif
int s6equal(sisl_double,sisl_double,sisl_double);
#ifndef  S6ERR
extern
#endif
void s6err(char *,int,int);
#ifndef  S6EXISTBOX
extern
#endif
int s6existbox(SISLbox *,int,sisl_double);
#ifndef S6FINDFAC
extern
#endif
void s6findfac(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int,int,
	       sisl_double *,sisl_double *,sisl_double *,int *);
#ifndef S6FNDINTVL
extern
#endif
void s6fndintvl(sisl_double *et,int ik,int in,int *ileft,
	      sisl_double ax1,sisl_double ax2,int mu_max,int *jstat);
#ifndef S6HERM
extern
#endif
void s6herm(sisl_double *,sisl_double *,sisl_double *,int,int,int,int,int,sisl_double [],int *);
#ifndef  S6HERMITE_BEZIER
extern
#endif
void     s6hermite_bezier(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double [],int *);
#ifndef  S6IDCON
extern
#endif
void s6idcon(SISLIntdat **,SISLIntpt **,SISLIntpt **,int *);
#ifndef  S6IDCPT
extern
#endif
void s6idcpt(SISLIntdat *,SISLIntpt *,SISLIntpt **);
#ifndef  S6IDEDG
extern
#endif
void s6idedg(SISLObject *,SISLObject *,int,int,sisl_double,SISLIntdat *,
	     SISLPtedge **,int *,int *);
#ifndef  S6IDENTIFY
extern
#endif
void     s6identify(SISLSurf *,sisl_double [],sisl_double [],sisl_double,sisl_double,sisl_double,int *);
#ifndef  S6IDGET
extern
#endif
void s6idget(SISLObject *,SISLObject *,int,sisl_double,SISLIntdat *,
	     SISLIntdat **,int *);
#ifndef  S6IDINT
extern
#endif
void s6idint(SISLObject *,SISLObject *,SISLIntdat *,SISLIntpt **,int);
#ifndef  S6IDKLIST
extern
#endif
void s6idklist(SISLIntdat **,SISLIntlist *,int *);
#ifndef  S6IDKPT
extern
#endif
void s6idkpt(SISLIntdat **,SISLIntpt **,SISLIntpt **,SISLIntpt **,int *);
#ifndef  S6IDLIS
extern
#endif
void s6idlis(SISLObject *,SISLObject *,SISLIntdat **,int *);
#ifndef  S6IDNPT
extern
#endif
void s6idnpt(SISLIntdat **,SISLIntpt **,int,int *);
#ifndef  S6IDPUT
extern
#endif
void s6idput(SISLIntdat **,SISLIntdat *,int,sisl_double,int *);
#ifndef S6INV4
extern
#endif
void s6inv4 (sisl_double [], sisl_double [], int *);
#ifndef  S6INVERT
extern
#endif
void s6invert(sisl_double [],int,sisl_double [],int *);
#ifndef  S6KNOTMULT
extern
#endif
int s6knotmult(sisl_double [],int,int,int *,sisl_double,int *);
#ifndef  S6LENGTH
extern
#endif
sisl_double s6length(sisl_double [],int,int *);
#ifndef  S6LINE
extern
#endif
void s6line(sisl_double []);
#ifndef  S6LPRJ
extern
#endif
sisl_double s6lprj(sisl_double [],sisl_double [],int);
#ifndef  S6LUFACP
extern
#endif
void s6lufacp(sisl_double [],int [],int,int *);
#ifndef  S6LUSOLP
extern
#endif
void s6lusolp(sisl_double [],sisl_double [],int [],int,int *);
#ifndef  S6METRIC
extern
#endif
void s6metric(sisl_double [],int,int,sisl_double [],int *);
#ifndef  S6MOVE
extern
#endif
void s6move(sisl_double []);
#ifndef S6MULVEC
extern
#endif
void s6mulvec (sisl_double [],sisl_double [],sisl_double []);
#ifndef  S6MVEC
extern
#endif
void s6mvec(sisl_double [],sisl_double [],int,sisl_double []);
#ifndef  S6NEWBOX
extern
#endif
void s6newbox(SISLbox *pbox,int inum,int itype,sisl_double aepsge,int *jstat);
#ifndef  S6NORM
extern
#endif
sisl_double s6norm(sisl_double [],int,sisl_double [],int *);
#ifndef  S6RATDER
extern
#endif
void s6ratder(sisl_double [],int,int,sisl_double [],int *);
#ifndef  S6ROTAX
extern
#endif
void s6rotax(sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef S6ROTMAT
extern
#endif
void s6rotmat (sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef S6SCHOEN
extern
#endif
sisl_double s6schoen(sisl_double [],int, int);
#ifndef S6SCPR
extern
#endif
sisl_double s6scpr(sisl_double [],sisl_double [],int);
#ifndef  S6SORTPAR
extern
#endif
void s6sortpar(sisl_double [],sisl_double [],int,int,sisl_double [],sisl_double [],int *);
#ifndef S6SRATDER
extern
#endif
void s6sratder(sisl_double [],int,int,int,sisl_double [],int *);
#ifndef S6STRIDER
extern
#endif
void s6strider(sisl_double [],int,int,sisl_double [],int *);
#ifndef  S6TAKEUNION
extern
#endif
void s6takeunion(sisl_double [],int,sisl_double [],int,sisl_double **,int *,int *);
#ifndef  S6TWONORM
extern
#endif
void s6twonorm(sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef S9ADSIMP
extern
#endif
sisl_double s9adsimp(sisl_double [],sisl_double [],sisl_double [],int,sisl_double [],
		sisl_double [],sisl_double [],sisl_double [],sisl_double,int *);
#ifndef S9ADSTEP
extern
#endif
sisl_double s9adstep(sisl_double [],sisl_double [],sisl_double [],sisl_double [],
		sisl_double [],sisl_double [],sisl_double [],sisl_double [],
		sisl_double [],sisl_double [],sisl_double [],sisl_double,int *);
#ifndef S9BOUNDIMP
extern
#endif
void s9boundimp(sisl_double [],sisl_double [],SISLSurf *,sisl_double [],
		int,sisl_double,int,sisl_double,sisl_double [],sisl_double [],int *);
#ifndef  S9BOUNDIT
extern
#endif
void s9boundit(sisl_double [],sisl_double [],sisl_double [],sisl_double [],
	       SISLSurf *,SISLSurf *,sisl_double,int,sisl_double,
	       sisl_double [],sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S9CLIPIMP
extern
#endif
void s9clipimp(sisl_double [],sisl_double [],SISLSurf *,sisl_double [],
	       int,sisl_double [],sisl_double [],sisl_double,sisl_double [],sisl_double [],int *);
#ifndef  S9CLIPIT
extern
#endif
void s9clipit(sisl_double [],sisl_double [],sisl_double [],sisl_double [],SISLSurf *,SISLSurf *,
	      sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double [],
	      sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  S9CONMARCH
extern
#endif
void s9conmarch(SISLSurf *,sisl_double,sisl_double [],int [],int,
		sisl_double *[],int *[],int *,int *);
#ifndef  S9ITERATE
extern
#endif
void s9iterate(sisl_double [],sisl_double [],sisl_double [],sisl_double [],sisl_double [],
	       SISLSurf *,SISLSurf *,sisl_double,sisl_double,sisl_double [],sisl_double [],
	       sisl_double [],sisl_double [],int *);
#ifndef  S9ITERIMP
extern
#endif
void s9iterimp(sisl_double [],sisl_double [],sisl_double [],SISLSurf *,sisl_double [],int,
	       sisl_double,sisl_double,sisl_double [],sisl_double [],int *);
#ifndef  S9SIMPLE_KNOT
extern
#endif
void 	 s9simple_knot(SISLSurf *,int,sisl_double [],int *,int *);
#ifndef  S9SURMARCH
extern
#endif
void s9surmarch(SISLSurf *,SISLSurf *,sisl_double [],int [],int,
		sisl_double *[],int *[],int *,int *);
#ifndef  SH1260
extern
#endif
void sh1260(sisl_double aconst,SISLCurve *vcurve[],int icurve,int *jstat);
#ifndef  SH1261
extern
#endif
void sh1261(SISLCurve *pcurve1,SISLCurve *pcurve2,sisl_double ecoef1[],
             int ik1,sisl_double ecoef2[],int ik2,SISLCurve **rcrtanc,int *jstat);
#ifndef  SH1262
extern
#endif
void sh1262(SISLCurve *vcurve[],int iedge,int inmbx,sisl_double ecoef[],int *jstat);
#ifndef  SH1263
extern
#endif
void sh1263(SISLCurve *vcurve[],int iedge,SISLCurve *vboundc[],int *jstat);
#ifndef  SH1365
extern
#endif
void sh1365(SISLCurve *,sisl_double [],int,int,int,int,SISLCurve **,
	    sisl_double **,sisl_double **,int *);
#ifndef  SH1369
extern
#endif
void sh1369(SISLSurf *,sisl_double [],sisl_double [],sisl_double,sisl_double,int,sisl_double,
	    sisl_double,int,int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1371
extern
#endif
void sh1371(SISLCurve *,sisl_double [],sisl_double,int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1372
extern
#endif
void sh1372(SISLCurve *,sisl_double [],sisl_double [],sisl_double,int,sisl_double,sisl_double,int,
	    int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *);
#ifndef  SH1373
extern
#endif
void sh1373(SISLCurve *,sisl_double [],sisl_double [],sisl_double [],int,sisl_double,sisl_double,
	    int,int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *);
#ifndef  SH1374
extern
#endif
void sh1374(SISLCurve *,sisl_double [],int,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1375
extern
#endif
void sh1375(SISLCurve *,sisl_double [],sisl_double [],sisl_double,sisl_double,int,sisl_double,
	    sisl_double,int,int *,SISLTrack ***,int *,sisl_double **,int **,
	    int *,SISLIntcurve ***,int *);
#ifndef  SH1460
extern
#endif
void sh1460(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    SISLCurve *[],int,SISLSurf ***,int *);
#ifndef  SH1461
extern
#endif
void sh1461(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    void (*)(void (*)(sisl_double [],sisl_double [],int,int,int *),
		     SISLCurve *[],int,sisl_double [],sisl_double [],sisl_double [],int *),
	    SISLCurve *[],int,SISLSurf *[],int *);
#ifndef  SH1462
extern
#endif
void sh1462(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    SISLCurve *[],int,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SH1463
extern
#endif
void sh1463(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    SISLCurve *[],int,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SH1464
extern
#endif
void sh1464(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    SISLCurve *[],int,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SH1465
extern
#endif
void sh1465(void (*)(sisl_double [],sisl_double [],int,int,int *),
	    SISLCurve *[],int,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SH1466
extern
#endif
void sh1466(SISLCurve *[],sisl_double [],int,sisl_double [],sisl_double [],int *);
#ifndef  SH1467
extern
#endif
void sh1467(SISLCurve *[],sisl_double [],int,sisl_double [],sisl_double [],int *);
#ifndef  SH1502
extern
#endif
void sh1502(SISLCurve *,sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double,
	    int,sisl_double,sisl_double,int,int *,SISLTrack ***,int *,sisl_double **,
	    int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1503
extern
#endif
void sh1503(SISLSurf *,sisl_double [],sisl_double [],sisl_double [],sisl_double,sisl_double,int,
	    sisl_double,sisl_double,int,int *,SISLTrack ***,int *,sisl_double **,int **,
	    int *,SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1510
extern
#endif
void sh1510(SISLSurf *,sisl_double [],int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1511
extern
#endif
void sh1511(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,int,
	    int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1761
extern
#endif
void sh1761(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,int *);
#ifndef  SH1762
extern
#endif
void sh1762(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,SISLEdge *[],int *);
#ifndef  SH1779
extern
#endif
void sh1779(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,SISLIntpt *,
	    int *,int *);
#ifndef  SH1779_AT
extern
#endif
void sh1779_at(SISLObject *,SISLObject *,SISLIntpt *,int *);
#ifndef  SH1780
extern
#endif
void sh1780(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,
	    SISLIntpt *,int *,int *);
#ifndef  SH1780_AT
extern
#endif
void sh1780_at(SISLObject *,SISLObject *,SISLIntpt *,int *);
#ifndef  SH1781
extern
#endif
void sh1781(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,
	    SISLIntpt *,int *,int *);
#ifndef  SH1781_AT
extern
#endif
void sh1781_at(SISLObject *,SISLObject *,SISLIntpt *,int *);
#ifndef  SH1782
extern
#endif
void sh1782(SISLObject *,SISLObject *,sisl_double,SISLIntdat *,int,sisl_double,
	    SISLIntdat **,int *,int *);
#ifndef  SH1783
extern
#endif
void sh1783(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],int,int,sisl_double [],
	    sisl_double [],int *);
#ifndef  SH1784
extern
#endif
void sh1784(SISLCurve *,SISLSurf *,sisl_double,sisl_double [],int,int,sisl_double [],
	    sisl_double [],int *);
#ifndef  SH1786
extern
#endif
void sh1786(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,SISLIntpt *,
	    int *,int *);
#ifndef  SH1787
extern
#endif
void sh1787(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,SISLIntpt *,
	    int *,int *);
#ifndef  SH1790
extern
#endif
void sh1790(SISLObject *,SISLObject *,int,sisl_double,int *);
#ifndef  SH1830
extern
#endif
void sh1830(SISLObject *,SISLObject *,sisl_double,int *);
#ifndef  SH1831
extern
#endif
void sh1831(SISLCurve *pc1, SISLCurve *pc2, int isign, sisl_double epoint[],
            sisl_double enorm[], sisl_double aepsge, int *jstat);
#ifndef  SH1834
extern
#endif
void sh1834(SISLObject *,SISLObject *,sisl_double,int,sisl_double [],sisl_double [],int *);
#ifndef  SH1839
extern
#endif
void sh1839(SISLObject *,SISLObject *,sisl_double,int *);
#ifndef  SH1850
extern
#endif
void sh1850(SISLCurve *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1851
extern
#endif
void sh1851(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,SISLIntcurve ***,
	    int *,SISLIntsurf ***,int *);
#ifndef  SH1852
extern
#endif
void sh1852(SISLSurf *,sisl_double [],sisl_double,int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,SISLIntcurve ***,
	    int *,SISLIntsurf ***,int *);
#ifndef  SH1853
extern
#endif
void sh1853(SISLSurf *,sisl_double [],sisl_double [],sisl_double,int,sisl_double,sisl_double,int,
	    int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1854
extern
#endif
void sh1854(SISLSurf *,sisl_double[],sisl_double[],sisl_double[],int,sisl_double,sisl_double,int,
	    int *, SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *,SISLIntsurf ***,int *);
#ifndef  SH1855
extern
#endif
void sh1855(SISLSurf *,sisl_double [],sisl_double,sisl_double [],int,sisl_double,sisl_double,
	    int,int *,SISLTrack ***,int *,sisl_double **,int **,int *,
	    SISLIntcurve ***,int *);
#ifndef  SH1856
extern
#endif
void sh1856(SISLSurf *,sisl_double [],sisl_double [],int,sisl_double,sisl_double,int,int *,
	    SISLTrack ***,int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1857
extern
#endif
void sh1857(SISLCurve *,SISLCurve *,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1858
extern
#endif
void sh1858(SISLSurf *,SISLCurve *,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1859
extern
#endif
void sh1859(SISLSurf *,SISLSurf *,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,sisl_double **,int **,int *,SISLIntcurve ***,
	    int *,SISLIntsurf ***,int *);
#ifndef  SH1860
extern
#endif
void sh1860(SISLSurf *,sisl_double[],int,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,int **,int *,SISLIntcurve ***,int *,
	    SISLIntsurf ***,int *);

#ifndef  SH1870
extern
#endif
void sh1870(SISLSurf *,sisl_double *,int,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1871
extern
#endif
void sh1871(SISLCurve *,sisl_double *,int,sisl_double,sisl_double,int,int *,SISLTrack ***,
	    int *,sisl_double **,int **,int *,SISLIntcurve ***,int *);
#ifndef  SH1922
extern
#endif
void sh1922(sisl_double [],int,int,sisl_double [],int,sisl_double [],
	    int [],int [],int *);
#ifndef  SH1923
extern
#endif
void sh1923(sisl_double *,int,int,int *,int *);
#ifndef  SH1924
extern
#endif
void sh1924(sisl_double *,sisl_double *,int,int,int,int *,int *);
#ifndef  SH1925
extern
#endif
void sh1925(SISLCurve *,SISLCurve *,int,sisl_double [],int [],
	    int [],sisl_double [],sisl_double [],int,int,int *);
#ifndef  SH1926
extern
#endif
void sh1926(sisl_double [],int,int,int,sisl_double [],sisl_double [],int,sisl_double [],
	    int [],int [],sisl_double [],int [],sisl_double [],int *);
#ifndef  SH1927
extern
#endif
void sh1927(sisl_double [],int,int,int,SISLCurve *,int,int,sisl_double [],int *);
#ifndef  SH1928
extern
#endif
void sh1928(sisl_double [],int,int,int,sisl_double [],sisl_double [],int,int,int,
	    sisl_double [],int,int [],int [],sisl_double [],sisl_double [],int [],int *);
#ifndef  SH1929
extern
#endif
void sh1929(sisl_double [],int,int,int,sisl_double [],int,int,sisl_double [],
	    int *,int *,int *);
#ifndef  SH1930
extern
#endif
void sh1930(sisl_double [],int [],int [],sisl_double [],sisl_double [],
	    int,int,int,int,int,int,int *);
#ifndef  SH1992
extern
#endif
void sh1992(SISLObject *,int,sisl_double,int *);
#ifndef  SH1992CU
extern
#endif
void sh1992cu(SISLCurve *,int,sisl_double,int *);
#ifndef  SH1992SU
extern
#endif
void sh1992su(SISLSurf *,int,sisl_double,int *);
#ifndef  SH1993
extern
#endif
void sh1993(SISLCurve *,sisl_double,int *);
#ifndef  SH1994
extern
#endif
void sh1994(SISLSurf *,sisl_double,int *);
#ifndef SH6CLOSEVERT
extern
#endif
void sh6closevert(SISLCurve *,SISLSurf *,sisl_double *,sisl_double []);
#ifndef SH6CVVERT
extern
#endif
void
sh6cvvert(SISLCurve *pc1, SISLCurve *pc2, sisl_double *cpar1, sisl_double *cpar2);
#ifndef  SH6COMEDG
extern
#endif
void sh6comedg(SISLObject *,SISLObject *,SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6CONNECT
extern
#endif
void sh6connect(SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6COUNT
extern
#endif
int sh6count(SISLIntpt *,int *);
#ifndef  SH6DEGEN
extern
#endif
void sh6degen(SISLObject *,SISLObject *,SISLIntdat **,sisl_double,int *);
#ifndef  SH6DISCONNECT
extern
#endif
void sh6disconnect(SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6EDGPOINT
extern
#endif
void sh6edgpoint(SISLEdge *[],SISLIntpt ***,int *,int *);
#ifndef  SH6EDGRED
extern
#endif
void sh6edgred(SISLObject *,SISLObject *,SISLIntdat *,int *);
#ifndef  SH6EVALINT
extern
#endif
void sh6evalint(SISLObject *,SISLObject *,sisl_double [],int,SISLIntpt *,
		sisl_double,sisl_double *[],sisl_double *[],sisl_double *[],int *);
#ifndef  SH6FINDSPLIT
extern
#endif
void sh6findsplit(SISLSurf *,SISLSurf *,sisl_double,int *);
#ifndef  SH6FLOOP
extern
#endif
void sh6floop(SISLIntpt *[],int,int *,int *);
#ifndef  SH6GETGEOM
extern
#endif
void sh6getgeom(SISLObject *,int,SISLIntpt *,sisl_double **,sisl_double **,sisl_double,int *);
#ifndef  SH6GETLIST
extern
#endif
void sh6getlist(SISLIntpt *,SISLIntpt *,int *,int *,int *);
#ifndef  SH6GETMAIN
extern
#endif
SISLIntpt *sh6getmain(SISLIntpt *);
#ifndef  SH6GETNEXT
extern
#endif
SISLIntpt *sh6getnext(SISLIntpt *,int);
#ifndef  SH6GETNHBRS
extern
#endif
void sh6getnhbrs(SISLIntpt *,SISLIntpt **,SISLIntpt **,int *);
#ifndef  SH6GETOTHER
extern
#endif
void sh6getother(SISLIntpt *,SISLIntpt *,SISLIntpt **,int *);
#ifndef  SH6GETPREV
extern
#endif
int sh6getprev(SISLIntpt *,SISLIntpt *);
#ifndef  SH6GETTOP
extern
#endif
void sh6gettop(SISLIntpt *,int,int *,int *,int *,int *,int *);
#ifndef  SH6GETTOPHLP
extern
#endif
void sh6gettophlp(SISLIntpt *,int [4],int,int *);
#ifndef  SH6IDALLEDG
extern
#endif
void sh6idalledg(SISLObject *,SISLObject *,SISLIntdat *,SISLEdge *[],int *);
#ifndef  SH6IDCON
extern
#endif
void sh6idcon (SISLIntdat **,SISLIntpt **,SISLIntpt **,int *);
#ifndef  SH6IDFCROSS
extern
#endif
void sh6idfcross(SISLIntdat *,SISLIntpt *[],int *,int,int,int *);
#ifndef  SH6IDGET
extern
#endif
void sh6idget(SISLObject *,SISLObject *,int,sisl_double,SISLIntdat *,
	      SISLIntdat **,sisl_double,int *);
#ifndef  SH6IDKPT
extern
#endif
void sh6idkpt(SISLIntdat **,SISLIntpt **,int,int *);
#ifndef  SH6IDLIS
extern
#endif
void sh6idlis(SISLObject *,SISLObject *,SISLIntdat **,sisl_double,int *);
#ifndef  SH6IDNPT
extern
#endif
void sh6idnpt(SISLIntdat **,SISLIntpt **,int,int *);
#ifndef  SH6IDNEWUNITE
extern
#endif
void sh6idnewunite(SISLObject *,SISLObject *,SISLIntdat **,
		   SISLIntpt **,SISLIntpt **,sisl_double,sisl_double,int *);
#ifndef  SH6IDPUT
extern
#endif
void sh6idput(SISLObject *,SISLObject *,SISLIntdat **,SISLIntdat *,
	      int,sisl_double,SISLIntpt ***,int *,int *);
#ifndef  SH6IDRMCROSS
extern
#endif
void sh6idrmcross(SISLObject *,SISLObject *,SISLIntdat **,SISLIntpt *[],
		  int,SISLIntpt *[],int,int *);
#ifndef  SH6IDSPLIT
extern
#endif
void sh6idsplit(SISLIntdat **,SISLIntpt *,int *);
#ifndef  SH6IDUNITE
extern
#endif
void sh6idunite(SISLIntdat **,SISLIntpt **,SISLIntpt **,sisl_double,int *);
#ifndef  SH6INSERT
extern
#endif
void sh6insert(SISLIntdat **,SISLIntpt *,SISLIntpt *,SISLIntpt **,int *);
#ifndef  SH6INSERTPT
extern
#endif
void sh6insertpt(SISLIntpt *,SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6ISCONNECT
extern
#endif
int sh6isconnect(SISLIntpt *,SISLIntpt *,SISLIntpt *);
#ifndef  SH6ISHELP
extern
#endif
int sh6ishelp (SISLIntpt *);
#ifndef  SH6ISINSIDE
extern
#endif
void sh6isinside(SISLObject *,SISLObject *,SISLIntpt *,int *);
#ifndef  SH6ISMAIN
extern
#endif
int sh6ismain(SISLIntpt *);
#ifndef  SH6NMBHELP
extern
#endif
int sh6nmbhelp(SISLIntpt *,int *);
#ifndef  SH6NMBMAIN
extern
#endif
int sh6nmbmain (SISLIntpt *,int *);
#ifndef  SH6PTOBJ
extern
#endif
void sh6ptobj(sisl_double *,SISLObject *,sisl_double,sisl_double [],sisl_double [],int *);
#ifndef  SH6PUTSING
extern
#endif
void sh6putsing( SISLIntpt *,SISLIntpt *);
#ifndef  SH6PUTTOUCH
extern
#endif
void sh6puttouch( SISLIntpt *,SISLIntpt *,int);
#ifndef  SH6RED
extern
#endif
void sh6red(SISLObject *,SISLObject *,SISLIntdat *,int *);
#ifndef  SH6REMCON
extern
#endif
void sh6remcon (SISLIntdat **,SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6REMOVEPT
extern
#endif
void sh6removept(SISLIntpt *,SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6SEPCRV
extern
#endif
void sh6sepcrv(SISLCurve *,SISLCurve *,sisl_double,sisl_double [],sisl_double *,int *);
#ifndef  SH6SETCNSDIR
extern
#endif
void sh6setcnsdir(SISLIntpt *,SISLIntpt *,int,int *);
#ifndef  SH6SETDIR
extern
#endif
void sh6setdir(SISLIntpt *,SISLIntpt *,int *);
#ifndef  SH6SETTOP
extern
#endif
void sh6settop(SISLIntpt *,int,int,int,int,int,int *);
#ifndef  SH6SPLITGEOM
extern
#endif
void sh6splitgeom(SISLSurf *,SISLSurf *,sisl_double,sisl_double [],sisl_double [],
		  sisl_double *,sisl_double *,int *);
#ifndef  SH6TOHELP
extern
#endif
void sh6tohelp(SISLIntpt *,int *);
#ifndef  SH6TOMAIN
extern
#endif
void sh6tomain(SISLIntpt *,int *);
#ifndef  SH6TRIMLIST
extern
#endif
void sh6trimlist(SISLIntpt *,SISLIntpt ***,int *,int *);
#ifndef  CRV_ARC_TANG
extern
#endif
void crv_arc_tang(SISLCurve *,sisl_double[],sisl_double,sisl_double,sisl_double[],sisl_double[],int *);
#ifndef  CRV_CRV_TANG
extern
#endif
void crv_crv_tang(SISLCurve *,SISLCurve *,sisl_double,sisl_double[],sisl_double[],int *);
#ifndef  CRV_LIN_TANG
extern
#endif
void crv_lin_tang(SISLCurve *,sisl_double[],sisl_double[],sisl_double,sisl_double,sisl_double *,int *);
#ifndef  EV_CV_OFF
extern
#endif
void ev_cv_off(SISLCurve *,int,sisl_double,int *,sisl_double,sisl_double[],int *);
#ifndef  EVAL_2_CRV
extern
#endif
void eval_2_crv(SISLCurve *,SISLCurve *,int,sisl_double[],int *,int *,
		sisl_double[],int *);
#ifndef  EVAL_CRV_ARC
extern
#endif
void eval_crv_arc(SISLCurve *,sisl_double[],sisl_double,int,sisl_double[],int *,
		  sisl_double[],int *);
#ifndef  HP_S1880
extern
#endif
void hp_s1880(SISLObject *,SISLObject *,int,int,int,SISLIntdat *,int *,
	      sisl_double **,sisl_double **,int **,int *,SISLIntcurve ***,
	      int *,SISLIntsurf ***,int *);
#ifndef  INT_JOIN_PER
extern
#endif
void int_join_per(SISLIntdat **,SISLObject *,SISLObject *,sisl_double [],
		  int,sisl_double,int *);
#ifndef  MAKE_CV_CYCLIC
extern
#endif
void make_cv_cyclic(SISLCurve *,int,int *);
#ifndef  MAKE_CV_KREG
extern
#endif
void make_cv_kreg(SISLCurve *,SISLCurve **,int *);
#ifndef  MAKE_SF_KREG
extern
#endif
void make_sf_kreg(SISLSurf *,SISLSurf **,int *);
#ifndef  MAKE_TRACKS
extern
#endif
void make_tracks(SISLObject *,SISLObject *,int,sisl_double [],int,SISLIntlist **,
		 int *,SISLTrack ***,sisl_double,int *);
#ifndef  NEWKNOTS
extern
#endif
void newknots(sisl_double [],int,int,sisl_double [],int,sisl_double,sisl_double **,int *,int *);
#ifndef  PICK_CRV_SF
extern
#endif
void pick_crv_sf(SISLObject *,SISLObject *,int,SISLIntpt *,SISLIntpt *,
		 SISLCurve **,int *);
#ifndef  PO_CRV_TANG
extern
#endif
void po_crv_tang(SISLCurve *,sisl_double[],sisl_double,sisl_double,sisl_double *,int *);
#ifndef  REFINE_ALL
extern
#endif
void refine_all(SISLIntdat **,SISLObject *,SISLObject *,sisl_double [],
		int,sisl_double,int *);
#ifndef  SH_1D_DIV
extern
#endif
void sh_1d_div(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,
	       SISLEdge *[],int *);
#ifndef  SH_DIV_CRV
extern
#endif
void sh_div_crv(SISLCurve *,int,sisl_double,SISLCurve **,int *);
#ifndef  SH_DIV_SURF
extern
#endif
void sh_div_surf(SISLSurf *,int,int,sisl_double,SISLSurf **,int *);
#ifndef  SH_SET_AT
extern
#endif
void sh_set_at(SISLObject *,SISLObject *,SISLIntdat *,int *);
#ifndef  SHAPE
extern
#endif
void shape(sisl_double [],sisl_double [],int,int,int *);
#ifndef  SHCHECKPUT
extern
#endif
void shcheckput(SISLObject *,SISLIntdat **,SISLIntdat *,int,sisl_double,int *);
#ifndef  SHCHECKTYPE
extern
#endif
int shchecktype(SISLObject *,sisl_double *);
#ifndef  SHCSFSING
extern
#endif
void shcsfsing(SISLCurve *,SISLSurf *,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SHEVALC
extern
#endif
void shevalc(SISLCurve *,int,sisl_double,sisl_double,int *,sisl_double [],int *);
#ifndef  SHMKHLPPTS
extern
#endif
void shmkhlppts(SISLObject *,SISLObject *,sisl_double,SISLIntdat **,
		SISLEdge *[],int *,int *);
#ifndef  SHSING
extern
#endif
void shsing(SISLSurf *,SISLSurf *,sisl_double [],sisl_double [],sisl_double [],int *);
#ifndef  SPLI_SILH
extern
#endif
void spli_silh (SISLIntdat ** pintdat,SISLObject * po1,int *jstat);
#ifndef  TEST_CYCLIC_KNOTS
extern
#endif
void test_cyclic_knots(sisl_double [],int,int,int *);

#else /* NOT SISLNEEDPROTOTYPES */

#ifndef CONSTRUCT
extern
#endif
SISLIntpt     *copyIntpt();
#ifndef CONSTRUCT
extern
#endif
SISLEdge      *newEdge();
#ifndef CONSTRUCT
extern
#endif
SISLIntdat    *newIntdat();
#ifndef CONSTRUCT
extern
#endif
SISLIntlist   *newIntlist();
#ifndef CONSTRUCT
extern
#endif
SISLIntpt     *newIntpt();
#ifndef CONSTRUCT
extern
#endif
SISLObject    *newObject();
#ifndef CONSTRUCT
extern
#endif
SISLPoint     *newPoint();
#ifndef CONSTRUCT
extern
#endif
SISLPtedge    *newPtedge();
#ifndef CONSTRUCT
extern
#endif
SISLIntsurf   *newIntsurf();
#ifndef CONSTRUCT
extern
#endif
SISLTrack     *newTrack();
#ifndef CONSTRUCT
extern
#endif
SISLTrimpar   *newTrimpar();
#ifndef CONSTRUCT
extern
#endif
SISLIntpt     *hp_newIntpt();
#ifndef CONSTRUCT
extern
#endif
SISLIntpt     *hp_copyIntpt();
#ifndef DESTRUCT
extern
#endif
void freeEdge();
#ifndef DESTRUCT
extern
#endif
void freeIntdat();
#ifndef DESTRUCT
extern
#endif
void freeIntlist();
#ifndef DESTRUCT
extern
#endif
void freeIntpt();
#ifndef DESTRUCT
extern
#endif
void freeObject();
#ifndef DESTRUCT
extern
#endif
void freePoint();
#ifndef DESTRUCT
extern
#endif
void freePtedge();
#ifndef DESTRUCT
extern
#endif
void freeIntsurf();
#ifndef DESTRUCT
extern
#endif
void freeTrimpar();
#ifndef DESTRUCT
extern
#endif
void freeTrack();
#ifndef MAKE3D
extern
#endif
void make3D();
#ifndef S1119
extern
#endif
void s1119();
#ifndef S1161
extern
#endif
void s1161();
#ifndef S1162
extern
#endif
void s1162();
#ifndef S1172
extern
#endif
void s1172();
#ifndef S1173
extern
#endif
void s1173();
#ifndef S1174
extern
#endif
void s1174();
#ifndef S1190
extern
#endif
void s1190();
#ifndef  S1192
extern
#endif
void s1192();
#ifndef  S1219
extern
#endif
void s1219();
#ifndef  S1220
extern
#endif
void s1220();
#ifndef  S1222
extern
#endif
void s1222();
#ifndef  S1223
extern
#endif
void s1223();
#ifndef  S1224
extern
#endif
void s1224();
#ifndef  S1231
extern
#endif
void s1231();
#ifndef  S1232
extern
#endif
void s1232();
#ifndef  S1235
extern
#endif
void s1235();
#ifndef  S1236
extern
#endif
void s1236();
#ifndef  S1239
extern
#endif
void s1239();
#ifndef  S1244
extern
#endif
void s1244();
#ifndef  S1245
extern
#endif
void s1245();
#ifndef  S1251
extern
#endif
void s1251();
#ifndef  S1252
extern
#endif
void s1252();
#ifndef  S1301
extern
#endif
void s1301();
#ifndef  S1304
extern
#endif
void s1304();
#ifndef  S1305
extern
#endif
void s1305();
#ifndef  S1306
extern
#endif
void s1306();
#ifndef  S1307
extern
#endif
void s1307();
#ifndef  S1308
extern
#endif
void s1308();
#ifndef  S1309
extern
#endif
sisl_double s1309();
#ifndef  S1311
extern
#endif
sisl_double s1311();
#ifndef  S1312
extern
#endif
void s1312();
#ifndef  S1313
extern
#endif
void s1313();
#ifndef  S1320
extern
#endif
void s1320();
#ifndef  S1321
extern
#endif
void s1321();
#ifndef  S1322
extern
#endif
void s1322();
#ifndef  S1323
extern
#endif
void s1323();
#ifndef  S1324
extern
#endif
void s1324();
#ifndef  S1325
extern
#endif
sisl_double s1325();
#ifndef  S1329
extern
#endif
void s1329();
#ifndef  S1330
extern
#endif
void s1330();
#ifndef  S1331
extern
#endif
void s1331();
#ifndef  S1333
extern
#endif
void s1333();
#ifndef  S1333_COUNT
extern
#endif
void s1333_count();
#ifndef  S1333_CYCLIC
extern
#endif
void s1333_cyclic();
#ifndef  S1334
extern
#endif
void s1334();
#ifndef  S1340
extern
#endif
void s1340();
#ifndef  S1341
extern
#endif
void s1341();
#ifndef  S1342
extern
#endif
void s1342();
#ifndef  S1343
extern
#endif
void s1343();
#ifndef  S1345
extern
#endif
void s1345();
#ifndef  S1346
extern
#endif
void s1346();
#ifndef  S1347
extern
#endif
void s1347();
#ifndef  S1348
extern
#endif
void s1348();
#ifndef  S1349
extern
#endif
void s1349();
#ifndef  S1350
extern
#endif
void s1350();
#ifndef  S1351
extern
#endif
void s1351();
#ifndef  S1352
extern
#endif
void s1352();
#ifndef  S1353
extern
#endif
void s1353();
#ifndef  S1354
extern
#endif
void s1354();
#ifndef  S1355
extern
#endif
void s1355();
#ifndef  S1358
extern
#endif
void s1358();
#ifndef  S1359
extern
#endif
void s1359();
#ifndef  S1361
extern
#endif
void s1361();
#ifndef  S1362
extern
#endif
void s1362();
#ifndef  S1366
extern
#endif
void s1366();
#ifndef  S1367
extern
#endif
void s1367();
#ifndef  S1370
extern
#endif
void s1370();
#ifndef  S1376
extern
#endif
void s1376();
#ifndef  S1377
extern
#endif
void s1377();
#ifndef  S1378
extern
#endif
void s1378();
#ifndef  S1381
extern
#endif
void s1381();
#ifndef  S1382
extern
#endif
void s1382();
#ifndef  S1384
extern
#endif
void s1384();
#ifndef  S1385
extern
#endif
void s1385();
#ifndef  S1393
extern
#endif
void s1393();
#ifndef  S1399
extern
#endif
void s1399();
#ifndef  S1435
extern
#endif
void s1435();
#ifndef  S1436
extern
#endif
void s1436();
#ifndef  S1437
extern
#endif
void s1437();
#ifndef  S1438
extern
#endif
void s1438();
#ifndef  S1452
extern
#endif
void s1452();
#ifndef S1500
extern
#endif
void s1500();
#ifndef S1504
extern
#endif
void s1504();
#ifndef S1505
extern
#endif
void s1505();
#ifndef S1507
extern
#endif
void s1507();
#ifndef S1512
extern
#endif
void s1512();
#ifndef S1513
extern
#endif
void s1513();
#ifndef S1516
extern
#endif
void s1516();
#ifndef S1517
extern
#endif
void s1517();
#ifndef S1520
extern
#endif
void s1520();
#ifndef S1521
extern
#endif
SISLCurve *s1521();
#ifndef S1528
extern
#endif
void s1528();
#ifndef S1531
extern
#endif
void s1531();
#ifndef S1540
extern
#endif
void s1540();
#ifndef S1541
extern
#endif
void s1541();
#ifndef  S1604
extern
#endif
void s1604();
#ifndef  S1605
extern
#endif
void s1605();
#ifndef  S1612
extern
#endif
void s1612();
#ifndef  S1613BEZ
extern
#endif
void s1613bez();
#ifndef S1614
extern
#endif
void s1614 ();
#ifndef S1615
extern
#endif
void s1615 ();
#ifndef S1616
extern
#endif
void s1616 ();
#ifndef S1617
extern
#endif
void s1617 ();
#ifndef S1618
extern
#endif
void s1618 ();
#ifndef S1619
extern
#endif
void s1619 ();
#ifndef  S1700
extern
#endif
void s1700();
#ifndef  S1701
extern
#endif
void s1701();
#ifndef  S1705
extern
#endif
void s1705();
#ifndef  S1707
extern
#endif
void s1707();
#ifndef  S1708
extern
#endif
void s1708();
#ifndef  S1741
extern
#endif
void s1741();
#ifndef S1753
extern
#endif
void s1753();
#ifndef S1754
extern
#endif
void s1754();
#ifndef S1755
extern
#endif
void s1755();
#ifndef  S1770
extern
#endif
void s1770();
#ifndef  S1770_2D
extern
#endif
void s1770_2D();
#ifndef  S1771
extern
#endif
void s1771();
#ifndef  S1772
extern
#endif
void s1772();
#ifndef  S1773
extern
#endif
void s1773();
#ifndef  S1780
extern
#endif
void s1780();
#ifndef  S1785
extern
#endif
void s1785();
#ifndef  S1786
extern
#endif
void s1786();
#ifndef  S1787
extern
#endif
void s1787();
#ifndef  S1788
extern
#endif
void s1788();
#ifndef  S1790
extern
#endif
void s1790();
#ifndef  S1791
extern
#endif
int s1791();
#ifndef  S1792
extern
#endif
sisl_double s1792();
#ifndef  S1795
extern
#endif
void s1795();
#ifndef  S1796
extern
#endif
void s1796();
#ifndef  S1797
extern
#endif
void s1797();
#ifndef  S1830
extern
#endif
void s1830();
#ifndef  S1834
extern
#endif
void s1834();
#ifndef  S1839
extern
#endif
void s1839();
#ifndef  S1840
extern
#endif
void s1840();
#ifndef  S1880
extern
#endif
void s1880();
#ifndef S1890
extern
#endif
void s1890();
#ifndef S1891
extern
#endif
void s1891();
#ifndef S1893
extern
#endif
void s1893();
#ifndef S1894
extern
#endif
void s1894();
#ifndef S1896
extern
#endif
void s1896();
#ifndef S1897
extern
#endif
void s1897();
#ifndef S1900
extern
#endif
void s1900();
#ifndef S1901
extern
#endif
void s1901();
#ifndef S1902
extern
#endif
void s1902();
#ifndef S1903
extern
#endif
void s1903();
#ifndef S1904
extern
#endif
void s1904();
#ifndef S1905
extern
#endif
void s1905();
#ifndef S1906
extern
#endif
void s1906();
#ifndef S1907
extern
#endif
void s1907();
#ifndef S1908
extern
#endif
void s1908();
#ifndef S1909
extern
#endif
void s1909();
#ifndef S1910
extern
#endif
void s1910();
#ifndef S1911
extern
#endif
void s1911();
#ifndef S1912
extern
#endif
void s1912();
#ifndef S1916
extern
#endif
void s1916();
#ifndef S1917
extern
#endif
void s1917();
#ifndef S1918
extern
#endif
void s1918();
#ifndef S1919
extern
#endif
void s1919();
#ifndef S1924
extern
#endif
void s1924();
#ifndef S1925
extern
#endif
void s1925();
#ifndef S1926
extern
#endif
void s1926();
#ifndef S1927
extern
#endif
void s1927();
#ifndef S1931
extern
#endif
void s1931();
#ifndef S1931UNIT
extern
#endif
void s1931unit();
#ifndef S1932
extern
#endif
void s1932();
#ifndef S1933
extern
#endif
void s1933();
#ifndef S1934
extern
#endif
void s1934();
#ifndef S1935
extern
#endif
void s1935();
#ifndef S1936
extern
#endif
void s1936();
#ifndef S1937
extern
#endif
void s1937();
#ifndef S1938
extern
#endif
void s1938();
#ifndef S1940
extern
#endif
void s1940();
#ifndef S1941
extern
#endif
void s1941();
#ifndef S1942
extern
#endif
void s1942();
#ifndef S1943
extern
#endif
void s1943();
#ifndef S1944
extern
#endif
void s1944();
#ifndef S1945
extern
#endif
void s1945();
#ifndef S1946
extern
#endif
void s1946();
#ifndef S1947
extern
#endif
void s1947();
#ifndef S1948
extern
#endif
void s1948();
#ifndef S1949
extern
#endif
void s1949();
#ifndef S1950
extern
#endif
void s1950();
#ifndef S1951
extern
#endif
void s1951();
#ifndef  S1956
extern
#endif
void s1956();
#ifndef  S1959
extern
#endif
void s1959();
#ifndef  S1960
extern
#endif
void s1960();
#ifndef S1990
extern
#endif
void s1990();
#ifndef  S1991
extern
#endif
void s1991();
#ifndef  S1992
extern
#endif
void s1992();
#ifndef  S1992
extern
#endif
void s1992cu();
#ifndef  S1992
extern
#endif
void s1992su();
#ifndef  S1993
extern
#endif
void s1993();
#ifndef  S1994
extern
#endif
void s1994();
#ifndef  S2501
extern
#endif
void s2501();
#ifndef  S2503
extern
#endif
void s2503();
#ifndef  S2505
extern
#endif
void s2505();
#ifndef  S2507
extern
#endif
void s2507();
#ifndef  S2509
extern
#endif
void s2509();
#ifndef  S2511
extern
#endif
void s2511();
#ifndef  S2512
extern
#endif
void s2512();
#ifndef  S2513
extern
#endif
void s2513();
#ifndef  S2514
extern
#endif
void s2514();
#ifndef  S2515
extern
#endif
void s2515();
#ifndef  S2516
extern
#endif
void s2516();
#ifndef  S2533
extern
#endif
void s2533();
#ifndef  S2534
extern
#endif
void s2534();
#ifndef  S2535
extern
#endif
void s2535();
#ifndef  S2541
extern
#endif
void s2541();
#ifndef  S2543
extern
#endif
void s2543();
#ifndef  S2551
extern
#endif
void s2551();
#ifndef  S2554
extern
#endif
void s2554();
#ifndef  S2555
extern
#endif
void s2555();
#ifndef  S2557
extern
#endif
void s2557();
#ifndef  S2558
extern
#endif
void s2558();
#ifndef  S2560
extern
#endif
void s2560();
#ifndef  S2561
extern
#endif
void s2561();
#ifndef  S6ADDCURVE
extern
#endif
void s6addcurve();
#ifndef  S6AFFDIST
extern
#endif
sisl_double s6affdist();
#ifndef  S6ANG
extern
#endif
sisl_double s6ang();
#ifndef  S6ANGLE
extern
#endif
sisl_double s6angle();
#ifndef  S6CHPAR
extern
#endif
void s6chpar();
#ifndef  S6CRSS
extern
#endif
void s6crss();
#ifndef  S6CRVCHECK
extern
#endif
void s6crvcheck();
#ifndef  S6CURVATURE
extern
#endif
void s6curvature();
#ifndef  s6CURVRAD
extern
#endif
void s6curvrad();
#ifndef  S6DECASTELJAU
extern
#endif
void s6deCasteljau();
#ifndef  S6DECOMP
extern
#endif
void s6decomp();
#ifndef  S6DEGNORM
extern
#endif
void s6degnorm();
#ifndef  S6DERTOPT
extern
#endif
void s6dertopt();
#ifndef  S6DIFF
extern
#endif
void s6diff();
#ifndef  S6DIST
extern
#endif
sisl_double s6dist();
#ifndef  S6DLINE
extern
#endif
sisl_double s6dline();
#ifndef  S6DPLANE
extern
#endif
sisl_double  s6dplane();
#ifndef  S6EQUAL
extern
#endif
int s6equal();
#ifndef  S6ERR
extern
#endif
void s6err();
#ifndef  S6EXISTBOX
extern
#endif
int s6existbox();
#ifndef S6FINDFAC
extern
#endif
void s6findfac();
#ifndef  S6HERM
extern
#endif
void s6herm();
#ifndef  S6HERMITE_BEZIER
extern
#endif
void s6hermite_bezier();
#ifndef  S6IDCON
extern
#endif
void s6idcon();
#ifndef  S6IDCPT
extern
#endif
void s6idcpt();
#ifndef  S6IDEDG
extern
#endif
void s6idedg();
#ifndef  S6IDENTIFY
extern
#endif
void     s6identify();
#ifndef  S6IDGET
extern
#endif
void s6idget();
#ifndef  S6IDINT
extern
#endif
void s6idint();
#ifndef  S6IDKLIST
extern
#endif
void s6idklist();
#ifndef  S6IDKPT
extern
#endif
void s6idkpt();
#ifndef  S6IDLIS
extern
#endif
void s6idlis();
#ifndef  S6IDNPT
extern
#endif
void s6idnpt();
#ifndef  S6IDPUT
extern
#endif
void s6idput();
#ifndef S6INV4
extern
#endif
void s6inv4 ();
#ifndef  S6INVERT
extern
#endif
void s6invert();
#ifndef  S6KNOTMULT
extern
#endif
int s6knotmult();
#ifndef  S6LENGTH
extern
#endif
sisl_double s6length();
#ifndef  S6LINE
extern
#endif
void s6line();
#ifndef  S6LPRJ
extern
#endif
sisl_double s6lprj();
#ifndef  S6LUFACP
extern
#endif
void s6lufacp();
#ifndef  S6LUSOLP
extern
#endif
void s6lusolp();
#ifndef  S6METRIC
extern
#endif
void s6metric();
#ifndef  S6MOVE
extern
#endif
void s6move();
#ifndef S6MULVEC
extern
#endif
void s6mulvec ();
#ifndef  S6MVEC
extern
#endif
void s6mvec();
#ifndef  S6NEWBOX
extern
#endif
void s6newbox();
#ifndef  S6NORM
extern
#endif
sisl_double s6norm();
#ifndef  S6RATDER
extern
#endif
void s6ratder();
#ifndef  S6ROTAX
extern
#endif
void s6rotax();
#ifndef S6SCPR
extern
#endif
sisl_double s6scpr();
#ifndef S6ROTMAT
extern
#endif
void s6rotmat ();
#ifndef S6SCHOEN
extern
#endif
sisl_double s6schoen();
#ifndef  S6SORTPAR
extern
#endif
void s6sortpar();
#ifndef S6SRATDER
extern
#endif
void s6sratder();
#ifndef S6STRIDER
extern
#endif
void s6strider();
#ifndef  S6TAKEUNION
extern
#endif
void s6takeunion();
#ifndef  S6TWONORM
extern
#endif
void s6twonorm();
#ifndef S9ADSIMP
extern
#endif
sisl_double s9adsimp();
#ifndef S9ADSTEP
extern
#endif
sisl_double s9adstep();
#ifndef S9BOUNDIMP
extern
#endif
void s9boundimp();
#ifndef  S9BOUNDIT
extern
#endif
void s9boundit();
#ifndef  S9CLIPIMP
extern
#endif
void s9clipimp();
#ifndef  S9CLIPIT
extern
#endif
void s9clipit();
#ifndef  S9CONMARCH
extern
#endif
void s9conmarch();
#ifndef  S9ITERATE
extern
#endif
void s9iterate();
#ifndef  S9ITERIMP
extern
#endif
void s9iterimp();
#ifndef  S9SIMPLE_KNOT
extern
#endif
void 	 s9simple_knot();
#ifndef  S9SURMARCH
extern
#endif
void s9surmarch();
#ifndef  SH1260
extern
#endif
void sh1260();
#ifndef  SH1261
extern
#endif
void sh1261();
#ifndef  SH1262
extern
#endif
void sh1262();
#ifndef  SH1263
extern
#endif
void sh1263();
#ifndef  SH1365
extern
#endif
void sh1365();
#ifndef  SH1369
extern
#endif
void sh1369();
#ifndef  SH1371
extern
#endif
void sh1371();
#ifndef  SH1372
extern
#endif
void sh1372();
#ifndef  SH1373
extern
#endif
void sh1373();
#ifndef  SH1374
extern
#endif
void sh1374();
#ifndef  SH1375
extern
#endif
void sh1375();
#ifndef  SH1460
extern
#endif
void sh1460();
#ifndef  SH1461
extern
#endif
void sh1461();
#ifndef  SH1462
extern
#endif
void sh1462();
#ifndef  SH1463
extern
#endif
void sh1463();
#ifndef  SH1464
extern
#endif
void sh1464();
#ifndef  SH1465
extern
#endif
void sh1465();
#ifndef  SH1466
extern
#endif
void sh1466();
#ifndef  SH1467
extern
#endif
void sh1467();
#ifndef  SH1502
extern
#endif
void sh1502();
#ifndef  SH1503
extern
#endif
void sh1503();
#ifndef  SH1510
extern
#endif
void sh1510();
#ifndef  SH1511
extern
#endif
void sh1511();
#ifndef  SH1761
extern
#endif
void sh1761();
#ifndef  SH1762
extern
#endif
void sh1762();
#ifndef  SH1779
extern
#endif
void sh1779();
#ifndef  SH1779_AT
extern
#endif
void sh1779_at();
#ifndef  SH1780
extern
#endif
void sh1780();
#ifndef  SH1780_AT
extern
#endif
void sh1780_at();
#ifndef  SH1781
extern
#endif
void sh1781();
#ifndef  SH1781_AT
extern
#endif
void sh1781_at();
#ifndef  SH1782
extern
#endif
void sh1782();
#ifndef  SH1783
extern
#endif
void sh1783();
#ifndef  SH1784
extern
#endif
void sh1784();
#ifndef  SH1786
extern
#endif
void sh1786();
#ifndef  SH1787
extern
#endif
void sh1787();
#ifndef  SH1790
extern
#endif
void sh1790();
#ifndef  SH1830
extern
#endif
void sh1830();
#ifndef  SH1831
extern
#endif
void sh1831();
#ifndef  SH1834
extern
#endif
void sh1834();
#ifndef  SH1839
extern
#endif
void sh1839();
#ifndef  SH1850
extern
#endif
void sh1850();
#ifndef  SH1851
extern
#endif
void sh1851();
#ifndef  SH1852
extern
#endif
void sh1852();
#ifndef  SH1853
extern
#endif
void sh1853();
#ifndef  SH1854
extern
#endif
void sh1854();
#ifndef  SH1855
extern
#endif
void sh1855();
#ifndef  SH1856
extern
#endif
void sh1856();
#ifndef  SH1857
extern
#endif
void sh1857();
#ifndef  SH1858
extern
#endif
void sh1858();
#ifndef  SH1859
extern
#endif
void sh1859();
#ifndef  SH1860
extern
#endif
void sh1860();
#ifndef  SH1870
extern
#endif
void sh1870();
#ifndef  SH1871
extern
#endif
void sh1871();
#ifndef  SH1922
extern
#endif
void sh1922();
#ifndef  SH1923
extern
#endif
void sh1923();
#ifndef  SH1924
extern
#endif
void sh1924();
#ifndef  SH1925
extern
#endif
void sh1925();
#ifndef  SH1926
extern
#endif
void sh1926();
#ifndef  SH1927
extern
#endif
void sh1927();
#ifndef  SH1928
extern
#endif
void sh1928();
#ifndef  SH1929
extern
#endif
void sh1929();
#ifndef  SH1930
extern
#endif
void sh1930();
#ifndef  SH1992
extern
#endif
void sh1992();
#ifndef  SH1992CU
extern
#endif
void sh1992cu();
#ifndef  SH1992SU
extern
#endif
void sh1992su();
#ifndef  SH1993
extern
#endif
void sh1993();
#ifndef  SH1994
extern
#endif
void sh1994();
#ifndef SH6CLOSEVERT
extern
#endif
void sh6closevert();
#ifndef SH6CVVERT
extern
#endif
void sh6cvvert();
#ifndef  SH6COMEDG
extern
#endif
void sh6comedg();
#ifndef  SH6CONNECT
extern
#endif
void sh6connect();
#ifndef  SH6COUNT
extern
#endif
int sh6count();
#ifndef  SH6DEGEN
extern
#endif
void sh6degen();
#ifndef  SH6DISCONNECT
extern
#endif
void sh6disconnect();
#ifndef  SH6EDGPOINT
extern
#endif
void sh6edgpoint();
#ifndef  SH6EDGRED
extern
#endif
void sh6edgred();
#ifndef  SH6EVALINT
extern
#endif
void sh6evalint();
#ifndef  SH6FINDSPLIT
extern
#endif
void sh6findsplit();
#ifndef  SH6FLOOP
extern
#endif
void sh6floop();
#ifndef  SH6GETGEOM
extern
#endif
void sh6getgeom();
#ifndef  SH6GETLIST
extern
#endif
void sh6getlist();
#ifndef  SH6GETMAIN
extern
#endif
SISLIntpt *sh6getmain();
#ifndef  SH6GETNEXT
extern
#endif
SISLIntpt *sh6getnext();
#ifndef  SH6GETNHBRS
extern
#endif
void sh6getnhbrs();
#ifndef  SH6GETOTHER
extern
#endif
void sh6getother();
#ifndef  SH6GETPREV
extern
#endif
int sh6getprev();
#ifndef  SH6GETTOP
extern
#endif
void sh6gettop();
#ifndef  SH6GETTOPHLP
extern
#endif
void sh6gettophlp();
#ifndef  SH6IDALLEDG
extern
#endif
void sh6idalledg();
#ifndef  SH6IDCON
extern
#endif
void sh6idcon ();
#ifndef  SH6IDFCROSS
extern
#endif
void sh6idfcross();
#ifndef  SH6IDGET
extern
#endif
void sh6idget();
#ifndef  SH6IDKPT
extern
#endif
void sh6idkpt();
#ifndef  SH6IDLIS
extern
#endif
void sh6idlis();
#ifndef  SH6IDNPT
extern
#endif
void sh6idnpt();
#ifndef  SH6IDNEWUNITE
extern
#endif
void sh6idnewunite();
#ifndef  SH6IDPUT
extern
#endif
void sh6idput();
#ifndef  SH6IDRMCROSS
extern
#endif
void sh6idrmcross();
#ifndef  SH6IDSPLIT
extern
#endif
void sh6idsplit();
#ifndef  SH6IDUNITE
extern
#endif
void sh6idunite();
#ifndef  SH6INSERT
extern
#endif
void sh6insert();
#ifndef  SH6INSERTPT
extern
#endif
void sh6insertpt();
#ifndef  SH6ISCONNECT
extern
#endif
int sh6isconnect();
#ifndef  SH6ISHELP
extern
#endif
int sh6ishelp();
#ifndef  SH6ISINSIDE
extern
#endif
void sh6isinside();
#ifndef  SH6ISMAIN
extern
#endif
int sh6ismain();
#ifndef  SH6NMBHELP
extern
#endif
int sh6nmbhelp();
#ifndef  SH6NMBMAIN
extern
#endif
int sh6nmbmain ();
#ifndef  SH6PTOBJ
extern
#endif
void sh6ptobj();
#ifndef  SH6PUTSING
extern
#endif
void sh6putsing();
#ifndef  SH6PUTTOUCH
extern
#endif
void sh6puttouch();
#ifndef  SH6RED
extern
#endif
void sh6red();
#ifndef  SH6REMCON
extern
#endif
void sh6remcon();
#ifndef  SH6REMOVEPT
extern
#endif
void sh6removept();
#ifndef  SH6SEPCRV
extern
#endif
void sh6sepcrv();
#ifndef  SH6SETCNSDIR
extern
#endif
void sh6setcnsdir();
#ifndef  SH6SETDIR
extern
#endif
void sh6setdir();
#ifndef  SH6SETTOP
extern
#endif
void sh6settop();
#ifndef  SH6SPLITGEOM
extern
#endif
void sh6splitgeom();
#ifndef  SH6TOHELP
extern
#endif
void sh6tohelp();
#ifndef  SH6TOMAIN
extern
#endif
void sh6tomain();
#ifndef  SH6TRIMLIST
extern
#endif
void sh6trimlist();
#ifndef  CRV_ARC_TANG
extern
#endif
void crv_arc_tang();
#ifndef  CRV_CRV_TANG
extern
#endif
void crv_crv_tang();
#ifndef  CRV_LIN_TANG
extern
#endif
void crv_lin_tang();
#ifndef  EV_CV_OFF
extern
#endif
void ev_cv_off();
#ifndef  EVAL_2_CRV
extern
#endif
void eval_2_crv();
#ifndef  EVAL_CRV_ARC
extern
#endif
void eval_crv_arc();
#ifndef  HP_S1880
extern
#endif
void hp_s1880();
#ifndef  INT_JOIN_PER
extern
#endif
void int_join_per();
#ifndef  MAKE_CV_CYCLIC
extern
#endif
void make_cv_cyclic();
#ifndef  MAKE_CV_KREG
extern
#endif
void make_cv_kreg();
#ifndef  MAKE_SF_KREG
extern
#endif
void make_sf_kreg();
#ifndef  MAKE_TRACKS
extern
#endif
void make_tracks();
#ifndef  NEWKNOTS
extern
#endif
void newknots();
#ifndef  PICK_CRV_SF
extern
#endif
void pick_crv_sf();
#ifndef  PO_CRV_TANG
extern
#endif
void po_crv_tang();
#ifndef  REFINE_ALL
extern
#endif
void refine_all();
#ifndef  SH_1D_DIV
extern
#endif
void sh_1d_div();
#ifndef  SH_DIV_CRV
extern
#endif
void sh_div_crv();
#ifndef  SH_DIV_SURF
extern
#endif
void sh_div_surf();
#ifndef  SH_SET_AT
extern
#endif
void sh_set_at();
#ifndef  SHAPE
extern
#endif
void shape();
#ifndef  SHCHECKPUT
extern
#endif
void shcheckput();
#ifndef  SHCHECKTYPE
extern
#endif
int shchecktype();
#ifndef  SHCSFSING
extern
#endif
void shcsfsing();
#ifndef  SHEVALC
extern
#endif
void shevalc();
#ifndef  SHMKHLPPTS
extern
#endif
void shmkhlppts();
#ifndef  SHSING
extern
#endif
void shsing();
#ifndef  SPLI_SILH
extern
#endif
void spli_silh();
#ifndef  TEST_CYCLIC_KNOTS
extern
#endif
void test_cyclic_knots();

#endif /* End forward declarations of  SISL C routines */



#endif  /* SISLP_INCLUDED */

/* DO NOT ADD ANYTHING AFTER THIS LINE */
