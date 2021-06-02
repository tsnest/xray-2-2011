#include "pch.h"



#define S6DIST

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
sisl_double 
s6dist(sisl_double epoint1[],sisl_double epoint2[],int idim)
#else
sisl_double s6dist(epoint1,epoint2,idim)
     sisl_double epoint1[];
     sisl_double epoint2[];
     int    idim;
#endif
                                     
{
  register sisl_double *s1,*s2,*s3; 
  register sisl_double tdist=DZERO; 
  
  for (s1=epoint1,s2=epoint2,s3=epoint1+idim; s1<s3; s1++,s2++)
    tdist += (*s1 - *s2)*(*s1 - *s2);
  
  return(sqrt(tdist));
}

