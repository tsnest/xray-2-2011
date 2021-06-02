#include "pch.h"



#define MAKE_CV_KREG

#include "sislP.h"
 

#if defined(SISLNEEDPROTOTYPES)
void
    make_cv_kreg (SISLCurve * pc, SISLCurve ** rcnew, int *jstat)
#else
void
   make_cv_kreg (pc, rcnew, jstat)
     SISLCurve *pc;
     SISLCurve **rcnew;
     int *jstat;
#endif
{
   int kn=pc->in;	
   int kk=pc->ik;	
   
   
   s1712 (pc, pc->et[kk-1], pc->et[kn], rcnew, jstat);
  if (*jstat < 0)  goto error;

   if (pc->cuopen == SISL_CRV_PERIODIC )
     (*rcnew)->cuopen = SISL_CRV_CLOSED;

  goto out;

  
error:
  s6err ("make_cv_kreg", *jstat, 0);

out:;

}

