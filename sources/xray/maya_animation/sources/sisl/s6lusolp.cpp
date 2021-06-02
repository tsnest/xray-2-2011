#include "pch.h"



#define S6LUSOLP

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
s6lusolp(sisl_double ea[],sisl_double eb[],int nl[],int im,int *jstat)
#else
void s6lusolp(ea,eb,nl,im,jstat)
     sisl_double ea[];
     sisl_double eb[];
     int    nl[];
     int    im;
     int    *jstat;
#endif
{
  int kpos = 0;      
  int ki,kj;         
  sisl_double *sx = SISL_NULL; 
  sisl_double tdiv;       
  
  
  
  if ((sx = newarray(im,sisl_double)) == SISL_NULL) goto err101;
  
  for (ki=0; ki<im-1; ki++)
    {
      
      
      for (kj=ki+1; kj<im; kj++)      
	eb[nl[kj]] -= eb[nl[ki]]*ea[ki+nl[kj]*im];
    }
  
  tdiv = ea[im-1+nl[im-1]*im];
  if (DEQUAL(tdiv,DZERO)) goto warn1;
  sx[im-1] = eb[nl[im-1]]/tdiv;
  
  for (ki=im-2; ki>=0; ki--)
    {
      
      
      for (kj=ki+1; kj<im; kj++)
	eb[nl[ki]] -= sx[kj]*ea[kj+nl[ki]*im];
      
      tdiv = ea[ki+nl[ki]*im];
      if (DEQUAL(tdiv,DZERO)) goto warn1;
      sx[ki] = eb[nl[ki]]/tdiv;
    }   
  for (ki=0; ki<im; ki++) eb[ki] = sx[ki];
  
  
  
  *jstat = 0; 
  goto out;


warn1 : *jstat = 1;
        goto out;


err101: *jstat = -101;
        s6err("s6lusolp",*jstat,kpos);
        goto out;

out:


if (sx != SISL_NULL) freearray(sx);

return;
}

