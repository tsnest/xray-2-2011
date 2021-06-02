#include "pch.h"

#define S6LENGTH

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
sisl_double 
s6length(sisl_double e1[],int idim,int *jstat)
#else
sisl_double s6length(e1,idim,jstat)
     sisl_double e1[]; 
     int    idim;
     int    *jstat;
#endif
{
  register int ki;            
  register sisl_double tsum=DZERO; 
  
  
  
  if (idim == 1)
    tsum = fabs(e1[0]);
  else
    {
      for (ki=0;ki<idim;ki++)
	tsum += (e1[ki]*e1[ki]);

      tsum = sqrt(tsum);
    }
  
  if (DNEQUAL(tsum,DZERO))
    goto mes01;

  

  *jstat = 0;
  goto out;

  

 mes01: *jstat = 1;
        goto out;

 out: return(tsum);
}

