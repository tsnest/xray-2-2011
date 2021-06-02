#include "pch.h"



#define S1220
#include "sislP.h"
 

 
#if defined(SISLNEEDPROTOTYPES)
void 
s1220(sisl_double *et,int ik,int in,int *ileft,
	   sisl_double ax,int ider,sisl_double ebder[],int *jstat)
#else
void s1220(et,ik,in,ileft,ax,ider,ebder,jstat)
     sisl_double *et;
     int    ik;
     int    in;
     int    *ileft;
     sisl_double ax;
     int    ider;
     sisl_double ebder[];
     int    *jstat;
#endif
                                     
{
  int kstat=0;        
  int kpos=0;         
  int kdeg;           
  int kleft;          
  int ki,kj,ks;       
  int ki1,ki2,kjh;    
  int kder;           
  sisl_double td1,td2;     
  sisl_double tw1,tw2;     
  sisl_double ts1,ts2 = xray::memory::uninitialized_value<sisl_double>();     
  sisl_double tt,tth;      
                                         
  
  if (ider < 0) goto err178;
  
  
  
  s1219(et,ik,in,ileft,ax,&kstat);
  
  if (kstat < 0) goto error;
  
  
  
  kleft = *ileft;
  kdeg = ik - 1;
  kder = min(ik-1,ider);
  
  
  
  ki2 = (ik-1)*(ider+1) + kder;
  ebder[ki2] = (sisl_double)1.0;
  
  if (ik == 1)
  {
     
     
     *jstat = 0;
     goto out;
  }
  
  
  
  ki1 = ki2 - ider - 2;
  if (kder == kdeg)
    {
      ebder[ki2-1] = (sisl_double)1.0;
      ki1 -= 1;
    }
  
  
  
  ki2 = ki1 + ider + 1;
  
  
  
  for(ks=1; ks<ik; ks++)
    {
      
      
      
      kj = kleft - ks + 1;
      kjh = kleft + 1;
      tt = et[kjh++];
      tth = tt - et[kj];
      if (tth <= (sisl_double)0.0) goto err112;
      td2 = (sisl_double)1.0/tth;
      tw2 = (tt-ax)*td2;
      
      ebder[++ki1] = tw2*ebder[++ki2];
      
      
      
      if (ks >= kdeg-kder && kder > 0)
	{
	  
	  
	  if (ks < kdeg)
	    ebder[ki1-1] = ebder[ki1];
	  
	  
	  
	  ts2 = ks*td2;
	  for (ki=0; ki<ks-kdeg+kder; ki++)
	    ebder[++ki1] = -ts2*ebder[++ki2];
	  
	  
	  
	  ki1 += ider - kder + kdeg - ks;
	  ki2 = ki1 + ider + 1;
	}
      else
	{
	  
	  
	  
	  ki1 += ider;
	  ki2 += ider;
	}
      
      
      
      for (kj=kleft-ks+2; kj<=kleft; kj++)
	{
	  
	  
	  
	  tt = et[kjh++];
	  tth = tt - et[kj];
	  if (tth <= (sisl_double)0.0) goto err112;
	  td1 = td2; td2 = (sisl_double)1.0/tth;
	  tw1 = (sisl_double)1.0 - tw2; tw2 = (tt-ax)*td2;
	  
	  ki1 += 1;
	  ebder[ki1] = tw1*ebder[ki1] + tw2*ebder[++ki2];
	  
	  
	  
	  if (ks >= kdeg-kder && kder > 0)
	    {
	      
	      
	      
	      if (ks < kdeg)
		ebder[ki1-1] = ebder[ki1];
	      
	      
	      
	      ts1 = ts2; ts2 = ks*td2;
	      for (ki=0; ki<ks-kdeg+kder; ki++)
		{
		  ki1 += 1;
		  ebder[ki1] = ts1*ebder[ki1] - ts2*ebder[++ki2];
		}
	      
	      
	      
	      ki1 += ider - kder + kdeg - ks;
	      ki2 = ki1 + ider + 1;
	    }
	  else
	    {
	      
	      
	      
	      ki1 += ider;
	      ki2 += ider;
	    }
	}
      
      
      
      td1 = td2;
      tw1 = (sisl_double)1.0 - tw2;
      
      ki1 += 1;
      ebder[ki1] = tw1*ebder[ki1];
      
      
      
      if (ks >= kdeg-kder && kder > 0)
	{
	  
	  
	  
	  if (ks < kdeg)
	    ebder[ki1-1] = ebder[ki1];
	  
	  
	  
	  ts1 = ts2;
	  for (ki=0; ki<ks-kdeg+kder; ki++)
	    {
	      ki1 += 1;
	      ebder[ki1] = ts1*ebder[ki1];
	    }
	  
	  
	  
	  ki1 -= ks - kdeg + kder + 1;
	}
      
      
      
      ki1 -= (ks+1)*(ider+1) + 1;
      ki2 = ki1 + ider + 1;
    }
  
  
  
  for (ki=kder+1; ki<=ider; ki++)
    {
      ki1 = ki;
      for (kj=0; kj<ik; kj++)
	{
	  ebder[ki1] = (sisl_double)0.0;
	  ki1 += ider + 1;
	}
    }
  
  
  
  *jstat = 0;
  goto out;
  
  
  
 err112: *jstat = -112;
  s6err("s1220",*jstat,kpos);
  goto out;
  
  
 err178: *jstat = -178;
  s6err("s1220",*jstat,kpos);
  goto out;
  
  
  
 error:  *jstat = kstat;
  s6err("s1220",*jstat,kpos);
  goto out;
  
 out: return;
}

