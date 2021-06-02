#include "pch.h"

#define S1961

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void s1961(sisl_double ep[],int im,int idim,int ipar,sisl_double epar[],
	   sisl_double eeps[],int ilend,int irend,int iopen,sisl_double afctol,
	   int itmax,int ik,SISLCurve **rc,sisl_double emxerr[],
	   int *jstat)
#else
void s1961(ep,im,idim,ipar,epar,eeps,ilend,irend,iopen,afctol,
           itmax,ik,rc,emxerr,jstat)
     sisl_double ep[];
     int    im;
     int    idim;
     int    ipar;
     sisl_double epar[];
     sisl_double eeps[];
     int    ilend;
     int    irend;
     int    iopen;
     sisl_double afctol;
     int    itmax;
     int    ik;
     SISLCurve  **rc;
     sisl_double emxerr[];
     int    *jstat;
#endif
/*
************************************************************
*
* Purpose: To compute a spline-approximation to the data given by the
*          points ep, and represent it as a B-spline curve with
*          parameterization determined by the parameter ipar.
*          The approximation is determined by first forming the piecewise
*          linear interpolant to the data, and then performing knot
*          removal on this initial approximation.
*
* Input :
*        Ep     - Array (length idim*im) containing the points to
*                 be approximated.
*        Im     - The no. of data points.
*        Idim   - The dimension of the euclidean space in which the data
*                 points lie, i.e. the number of components of each data point.
*        Ipar   - Flag indicating the type of parameterization to be used:
*                  = 1 : Paramterize by accumulated cord length.
*                        (Arc length parametrization for the piecewise
*                        linear interpolant.)
*                  = 2 : Uniform parameterization.
*                  = 3 : Parametrization given by epar.
*                 If ipar<1 or ipar>3, it will be set to 1.
*        Epar   - Array (length im) containing a parametrization
*                 of the given data.
*        Eeps   - Array (length idim) containing the tolerance to be
*                 used during the data reduction stage. The final
*                 approximation to the data will deviate less than eeps
*                 from the piecewise linear interpolant in each of the
*                 idim components.
*        Ilend  - The no. of derivatives that are not allowed to change
*                 at the left end of the curve.
*                 The (0 - (ilend-1)) derivatives will be kept fixed.
*                 If ilend <0, this routine will set it to 0.
*                 If ilend<ik, this routine will set it to ik.
*        Irend  - The no. of derivatives that are not allowed to change
*                 at the right end of the curve.
*                 The (0 - (irend-1)) derivatives will be kept fixed.
*                 If irend <0, this routine will set it to 0.
*                 If irend<ik, this routine will set it to ik.
*        iopen  - Open/closed parameter.
*                        =  1 : Produce open curve.
*                        =  0 : Produce closed, non-periodic curve.
*                        = -1 : Produce closed, periodic curve.
*                 If a closed or periodic curve is to be produced and the
*                 start- and endpoint is more distant than the length of
*                 the tolerance, a new point is added. Note that if the
*                 parametrization is given as input, the parametrization
*                 if the last point will be arbitrary.
*        Afctol = Number indicating how the tolerance is to be shared
*                 between the two data reduction stages. For the linear
*                 reduction, a tolerance of afctol*eeps will be used,
*                 while a tolerance of (1-afctol)*eeps will be used
*                 during the final data reduction. (Similarly for edgeps.)
*        Itmax  - Max. no. of iterations in the data-reduction routine.
*        Ik     - The polynomial order of the approximation.
*
* Output:
*        Jstat  - Output staus:
*                  < 0 : Error.
*                  = 0 : Ok.
*                  > o : Warning.
*        Rc     - Pointer to curve.
*        Emxerr - Array (length idim) (allocated outside this routine.)
*                 containing for each component an upper bound on the
*                 max. deviation of the final approximation from the
*                 initial piecewise linear interpolant.
*
* Method: First the piecewise linear interpoolant is computed by a call
*         to s1350 or s1351, and then knots are removed from this
*         interpolant. The degree of the resulting linear spline is then
*         raised to ik, and a second data-reduction is performed.
*
* Written by: C.R.Birkeland  Si  Oslo,Norway April 1993.
* Changed and renamed by : Vibeke Skytt, SINTEF Oslo, 02.95. Introduced
*                                                            periodicity.
********************************************************************
*/
{
  sisl_double *maxerr = SISL_NULL;    /* Arrays used to store error estimates */
  sisl_double *error1 = SISL_NULL;
  int i;
  int stat = 0;             /* Loop control variables               */
  int kpos = 0;
  SISLCurve *ocurve = SISL_NULL; /* Local spline curve                   */
  sisl_double *sp = SISL_NULL;        /* Extended data points in closed/periodic case. */
  sisl_double *spar = SISL_NULL;      /* Extended par. values.                */
  
  /* Check Input */
  
  if (im < 2 || idim < 1) goto err103;
  if (ipar < 1 || ipar > 3) ipar = 1;

  if ((iopen == 0 || iopen == -1) &&
      s6dist(ep, ep+(im-1)*idim, idim) > s6length(eeps, idim, &stat))
  {
     /* Add an extra point to the input points. First allocated scratch
	for extended arrays. */
     
     if ((sp = newarray((im+1)*idim, sisl_double)) == SISL_NULL) goto err101;
     memcopy(sp, ep, im*idim, sisl_double);
     memcopy(sp+im*idim, ep, idim, sisl_double);
     if (ipar == 3)
     {
	if ((spar = newarray(im+1, sisl_double)) == SISL_NULL) goto err101;
	memcopy(spar, epar, im, sisl_double);
	spar[im] = spar[im-1] + s6dist(sp+(im-1)*idim, sp+im*idim, idim);
     }
  }
  else 
  {
     sp = ep;
     spar = epar;
  }
  
  /* Set default value for afctol */

  if(afctol < 0 || afctol > 1) afctol =0;

  /* Piecewise linear interpolant to the data */

  if (ipar == 3)
    s1350(sp, spar, im, idim, 2, &ocurve, &stat);
  else
    s1351(sp, ipar, im, idim, 2, &ocurve, &stat);
  if (stat<0) goto error;

  /* Compute tolerance for linear reduction */

  error1 = newarray(idim, sisl_double);
  maxerr = newarray(idim, sisl_double);
  if (error1 == SISL_NULL || maxerr == SISL_NULL) goto err101;
  for (i=0; i<idim; i++)
    maxerr[i] = eeps[i]*afctol;

  /* Data reduction on piecewise linear interpolant */

  s1940(ocurve, maxerr, ilend, irend, iopen, itmax, rc,
	error1, &stat);
  if (stat<0) goto error;
  freeCurve(ocurve);

  /* Piecewise linear interpolant to the reduced data 
     (coefficients of linear spline curve rc)
     expressed as B-spline of order ik */

  s1350((*rc)->ecoef, &((*rc)->et)[1], (*rc)->in, 
	idim, ik, &ocurve, &stat);
  if (stat<0) goto error;
  freeCurve(*rc);
  
  /* Compute tolerance for last reduction */

  for (i=0; i<idim; i++)
    maxerr[i] = eeps[i] - error1[i];

  /* Perform data reduction on the hermite interpolant */

  s1940(ocurve, maxerr, ilend, irend, iopen, itmax, rc,
	emxerr, &stat);
  if (stat<0) goto error;

  /* Compute total error after reduction */

  for (i=0; i<idim; i++)
    emxerr[i] += error1[i];
  
  *jstat = 0;
  goto out;

  /* Error in scratch allocation.  */

  err101 :
     *jstat = -101;
  s6err("s1961",*jstat,kpos);
  goto out;

  /* Error in input */

 err103: 
  *jstat = -103;
  s6err("s1961",*jstat,kpos);
  goto out;
  
  /* Error in lower level routine. */

 error:
  *jstat = stat;
  s6err("s1961",*jstat,kpos);
  goto out;

  /* Exit */

 out:
  if (maxerr != SISL_NULL) freearray(maxerr);
  if (error1 != SISL_NULL) freearray(error1);
  if (ocurve != SISL_NULL) freeCurve(ocurve);
  if (spar != SISL_NULL && spar != epar) freearray(spar);
  if (sp != SISL_NULL && sp != ep) freearray(sp);
  return;
}
