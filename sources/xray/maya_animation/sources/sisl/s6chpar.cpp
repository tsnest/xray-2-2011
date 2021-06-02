#include "pch.h"



#define S6CHPAR

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
s6chpar(sisl_double ecoef1[],int in1,int in2,int idim,sisl_double ecoef2[])
#else
void s6chpar(ecoef1,in1,in2,idim,ecoef2)
     sisl_double ecoef1[];
     int    in1;
     int    in2;
     int    idim;
     sisl_double ecoef2[];
#endif
{
  register int ki,kj,kk;  
  
  for (ki=0; ki<in1; ki++)
    for (kj=0; kj<in2; kj++)
      for (kk=0; kk<idim; kk++)
	ecoef2[(ki*in2+kj)*idim+kk] = ecoef1[(kj*in1+ki)*idim+kk];
}

