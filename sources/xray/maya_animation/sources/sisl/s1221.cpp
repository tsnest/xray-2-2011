#include "pch.h"

/*
 *
 * $Id: s1221.c,v 1.3 2001/03/19 15:58:42 afr Exp $
 *
 */


#define S1221

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
s1221(SISLCurve *pc1,int ider,sisl_double ax,int *ileft,sisl_double eder[],int *jstat)
#else
void s1221(pc1,ider,ax,ileft,eder,jstat)
     SISLCurve *pc1;
     int ider;
     sisl_double ax;
     int *ileft;
     sisl_double eder[];
     int *jstat;
#endif
/*
*********************************************************************
*
*********************************************************************
*
* PURPOSE    : To compute the value and ider first derivatives of the
*              NURBS curve pointed to by pc1, at the point with
*              parameter value ax.
*
*
*
* INPUT      : pc1    - Pointer to the curve for which position
*                       and derivatives are to be computed.
*              ider   - The number of derivatives to compute.
*                       < 0 : Error.
*                       = 0 : Compute position.
*                       = 1 : Compute position and first derivative.
*                       etc.
*              ax     - The parameter value at which to compute
*                       position and derivatives.
*
*
*
* INPUT/OUTPUT : ileft - Pointer to the interval in the knot vector
*                        where ax is located. If et is the knot vector,
*                        the relation
*
*                          et[ileft] <= ax < et[ileft+1]
*
*                        should hold. (If ax == et[in] then ileft should
*                        be in-1. Here in is the number of NURBS
*                        coefficients.)
*                        If ileft does not have the right value upon
*                        entry to the routine, its value will be changed
*                        to the value satisfying the above condition.
*
*
*
* OUTPUT     : eder   - Double array of dimension [(ider+1)*idim]
*                       containing the position and derivative vectors.
*                       (idim is the number of components of each B-spline
*                       coefficient, i.e. the dimension of the Euclidean
*                       space in which the curve lies.)
*                       These vectors are stored in the following order:
*                       First the idim components of the position vector,
*                       then the idim components of the tangent vector,
*                       then the idim components of the second derivative
*                       vector, and so on.
*                       (The C declaration of eder as a two dimensional array
*                       would therefore be eder[ider+1,idim].)
*              jstat  - Status messages
*                                         > 0      : Warning.
*                                         = 0      : Ok.
*                                         < 0      : Error.
*
*
* METHOD     : Suppose that the given curve is of the form
*
*                   f(x) = sum(i) c(i) B(i,k)(x)
*
*              where c are the B-spline coefficients and B(i,k)(x) the
*              B-splines accociated with the knot vector et.
*              (For idim > 1 this is a vector equation with idim
*              components; however, the B-spline is not a vector.)
*              It is known that for a given value of x there are at most
*              k (the order of the splines) nonzero B-splines.
*              If ileft has the correct value these B-splines will be
*
*              B(ileft-k+1,k),B(ileft+k+2,k),...,B(ileft,k).
*
*              The position and derivatives are computed by
*              first computing the values and derivatives of all the
*              B-splines at x, and then multiplying and summing.
*
* REFERENCES :
*
*-
* CALLS      : s1220    - Computes B-spline values and derivatives at
*                         a given point.
*              s6err    - Error handling routine
*              s6ratder - Make derivative of rational expression
*
* WRITTEN BY : Knut Moerken, University of Oslo, August 1988.
* REVISED BY : Tor Dokken, SI, Oslo, Norway, January 1989
* MODIFIED BY : Mike Floater, SI, Oslo, Norway, January 1991 for rational case.
* MODIFIED BY : Mike Floater, SI, Oslo, Norway, April 1991 free arrays.
* REVISED BY : Johannes Kaasa, SI, Aug. 92 (In case of NURBS, the maximum
*              derivative is not set equal to the order).
*
*********************************************************************
*/
{
  int kstat=0;        /* Local status variable.                          */
  int kpos=0;         /* The position of the error.                      */
  int kn;             /* The number of B-splines, i.e., the dimension of
			 the spline space associated with the knot
			 vector.                                         */
  int kk;             /* The polynomial order of the curve.              */
  int kdim;           /* The dimension of the space in which the curve
			 lies. Equivalently, the number of components
			 of each B-spline coefficient.                   */
  int kleft;          /* Local version of ileft which is used in order to
			 avoid the pointer.                              */
  int kder;           /* Local version of ider. Since derivatives of order
			 higher than kk-1 are all zero, we set
			 kder = min(kk-1,ider).                          */
  int kind;           /* Type of curve                                   */
  int ki,kj,kih,kjh;  /* Control variables in for loops and for stepping
			 through arrays.                                 */
  int kl,kl1,kl2;     /* Control variables in for loops and for stepping
			 through arrays.                                 */
  sisl_double *st;         /* Pointer to the first element of the knot vector
			 of the curve. The knot vector has [kn+kk]
			 elements.                                       */
  sisl_double *scoef;      /* Pointer to the first element of the curve's
			 B-spline coefficients. This is assumed to be an
			 array with [kn*kdim] elements stored in the
			 following order:
			 First the kdim components of the first B-spline
			 coefficient, then the kdim components of the
			 second B-spline coefficient and so on.          */
  sisl_double tt;          /* Dummy variable used for holding an array element
			 in a for loop.                                  */
  sisl_double *ebder=SISL_NULL; /* Pointer to an array of dimension [kk*(ider+1)]
		       which will contain the values and ider first derivatives
			 of the kk nonzero B-splines at ax.
			 These are stored in the following order:
			 First the value, 1. derivative etc. of the
			 first nonzero B-spline, then the same for the
			 second nonzero B-spline and so on.              */
  sisl_double *sder=SISL_NULL;  /* Pointer to array used for storage of points, if
			 non rational sder points to eder, if rational sder
			 has to be allocated to make room for the homogenous
			 coordinate */

  /* Copy curve attributes to local parameters.  */

  kn = pc1 -> in;
  kk = pc1 -> ik;
  st = pc1 -> et;
  kdim = pc1 -> idim;
  kind = pc1 ->ikind;

  if (kind == 2 || kind == 4)
    {
      scoef = pc1 -> rcoef;
      kdim +=1;
      sder = newarray(kdim*(ider+1),sisl_double);
      if (sder==SISL_NULL) goto err101;
    }
  else
    {
      scoef = pc1 -> ecoef;
      sder = eder;
    }

  /* Check the input. */

  if (kdim < 1) goto err102;

  if (kk < 1) goto err110;

  if (kn < kk) goto err111;

  if (st[kk-1] == st[kk] || st[kn-1] == st[kn]) goto err112;

  if (ider < 0) goto err178;

  if (pc1->ikind == 1 || pc1->ikind == 3)
    kder = min(kk-1,ider);
  else
    kder = ider;

  /* Allocate space for B-spline values and derivatives. */

  ebder = newarray(kk*(kder+1),sisl_double);
  if (ebder == SISL_NULL) goto err101;

  /* Set all the elements of sder to 0. */

  for (ki=0; ki<(ider+1)*kdim; ki++) sder[ki] = DZERO;

  /* Compute the values and derivatives of the nonzero B-splines and
     update ileft if necessary.                                      */

  s1220(st,kk,kn,ileft,ax,kder,ebder,&kstat);

  if (kstat < 0) goto error;

  kleft = *ileft;

  /* Multiply together as indicated above. */

  /* ki steps through the appropriate kk B-spline coefficients while kih steps
     through the B-spline value and derivatives for the B-spline given by ki.*/

  kih = 0;
  for (ki=kleft-kk+1; ki<=kleft; ki++)
    {

      /* kj counts through the kder+1 derivatives to be computed.
	 kjh steps through sder once for each ki to accumulate the contribution
	 from the different B-splines.
	 kl1 points to the first component of B-spline coefficient no. ki. */

      kjh = 0; kl1 = kdim*ki;
      for (kj=0; kj<=kder; kj++)
	{

	  /* The value of the B-spline derivative is stored in tt while
	     kl2 steps through the idim components of this B-spline
	     coefficient. */

	  tt = ebder[kih++]; kl2 = kl1;
	  for (kl=0; kl<kdim; kl++,kjh++,kl2++)
	    {
	      sder[kjh] += scoef[kl2]*tt;
	    }
	}
    }

  /* Free memory. */

  /* If rational curve calculate the derivatives based on derivatives in
     homogenous coordinates */

  if (kind == 2 || kind == 4)
    {
      s6ratder(sder,pc1->idim,ider,eder,&kstat);
      if (kstat<0) goto error;
      freearray(sder);
    }

  freearray(ebder);

  /* Successful computations.  */

  *jstat = 0;
  goto out;

/* Not enough memory. */
 err101: *jstat = -101;
         s6err("s1221",*jstat,kpos);
         goto out;

/* kdim less than 1. */
 err102: *jstat = -102;
         s6err("s1221",*jstat,kpos);
         goto out;

/* Polynomial order less than 1. */
 err110: *jstat = -110;
         s6err("s1221",*jstat,kpos);
         goto out;

/* Fewer B-splines than the order. */
 err111: *jstat = -111;
         s6err("s1221",*jstat,kpos);
         goto out;

/* Error in knot vector.
   (The first or last interval of the knot vector is empty.) */
 err112: *jstat = -112;
         s6err("s1221",*jstat,kpos);
         goto out;

/* Illegal derivative requested. */
 err178: *jstat = -178;
         s6err("s1221",*jstat,kpos);
         goto out;

/* Error in lower level routine.  */

 error:  *jstat = kstat;
         s6err("s1221",*jstat,kpos);
         goto out;

out: return;
}
