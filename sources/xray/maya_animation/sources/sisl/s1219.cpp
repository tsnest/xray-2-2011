#include "pch.h"



#define S1219

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void 
s1219(sisl_double *et,int ik,int in,int *ileft,sisl_double ax,int *jstat)
#else
void s1219(et,ik,in,ileft,ax,jstat)
     sisl_double *et;
     int    ik;
     int    in;
     int    *ileft;
     sisl_double ax;
     int    *jstat;
#endif
                                     
{
  int kpos=0;         
  int kleft;          
  
  
  
  if (ik < 1) goto err110;
  
  if (in < ik) goto err111;
  
  if (et[ik-1] == et[ik] || et[in-1] == et[in]) goto err112;
  
  
  
  kleft = min(max(ik-1,*ileft),in-1);
  
  
  
  if (et[kleft] <= ax && ax < et[kleft+1]) ;
  
  
  
  else if (ax >= et[in-1])
    kleft = in - 1;
  else if (ax <= et[ik-1])
    kleft = ik - 1;
  
  
  
  else if (et[kleft+1] <= ax && ax < et[kleft+2])
    kleft += 1;
  else if (kleft > 0 && et[kleft-1] <= ax && ax < et[kleft])
    kleft -= 1;
  
  
  else
    {
      
      
      
      int kmin,kmax;
      
      kmin = ik - 1; kmax = in - 1;
      kleft = (kmin+kmax)/2;
      
      while (ax < et[kleft] || et[kleft+1] <= ax)
	{
	  if (ax < et[kleft])
	    kmax = kleft;
	  else
	    kmin = kleft;
	  
	  kleft = (kmin+kmax)/2;
	}
    }
  
  *ileft = kleft;
  
  
  
  *jstat = 0;
  goto out;
  
  
 err110: *jstat = -110;
  s6err("s1219",*jstat,kpos);
  goto out;
  
  
 err111: *jstat = -111;
  s6err("s1219",*jstat,kpos);
  goto out;
  
  
 err112: *jstat = -112;
  s6err("s1219",*jstat,kpos);
  goto out;
  
 out: return;
}

