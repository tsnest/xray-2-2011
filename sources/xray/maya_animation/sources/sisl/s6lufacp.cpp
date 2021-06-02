#include "pch.h"



#define S6LUFACP

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
s6lufacp(sisl_double ea[],int nl[],int im,int *jstat)
#else
void s6lufacp(ea,nl,im,jstat)
     sisl_double ea[];
     int    nl[];
     int    im;
     int    *jstat;
#endif
{
  int kpos = 0;   
  int ki,kj,kk;   
  int kmax = 0;   
  int kchange;    
  sisl_double tmult;   
  sisl_double t1;      
  sisl_double tmax;    
  sisl_double tdiv;    
  sisl_double *smax = SISL_NULL; 
  
  
  
  if ((smax = new0array(im,sisl_double)) == SISL_NULL) goto err101;
  
  
  
  for (ki=0; ki<im; ki++)
    {                      
      nl[ki] = ki;
      for (kj=0; kj<im; kj++) 
	smax[ki] = MAX(smax[ki],fabs(ea[ki*im+kj]));
    }
  
  for (ki=0; ki<im-1; ki++)
    {
      
      
      
      tmax = DZERO;  
      for (kj=ki; kj<im; kj++)
	{
	  tdiv = smax[nl[kj]];
	  if (DEQUAL(tdiv,DZERO)) goto warn1;
	  t1 = fabs(ea[nl[kj]*im+ki]/tdiv);
	  if (t1 > tmax)
	    {
	      tmax = t1; 
	      kmax = kj;
	    }
	}
      kchange  = nl[kmax];
      nl[kmax] = nl[ki];
      nl[ki]   = kchange;
      
      
      
      for (kj=ki+1; kj<im; kj++)
	{
	  tdiv = ea[ki+kchange*im];
	  if (DEQUAL(tdiv,DZERO)) goto warn1;
	  tmult = ea[ki+nl[kj]*im]/tdiv;
	  ea[ki+nl[kj]*im] = tmult;
	  
	  for (kk=ki+1; kk<im; kk++)
	    ea[kk+nl[kj]*im] -= ea[kk+kchange*im]*tmult;
	}                                     
    }
  
  
  
  *jstat = 0;
  goto out;


warn1 : *jstat = 1;
        goto out;


err101: *jstat = -101;
        s6err("s6lufacp",*jstat,kpos);
        goto out;

out:


if (smax != SISL_NULL) freearray(smax);

return;
}

                                    

                                        

