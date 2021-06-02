#include "pch.h"



#define S1700

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
s1700(int imy,int ik,int in,int iv,
	   int *jpl,int *jfi,int *jla,sisl_double *et,sisl_double apar,sisl_double *galfa,int *jstat)
#else
void s1700(imy,ik,in,iv,jpl,jfi,jla,et,apar,galfa,jstat)
     int    imy;
     int    ik;
     int    in;
     int    iv;
     int    *jpl;
     int    *jfi;
     int    *jla;
     sisl_double *et;
     sisl_double apar;
     sisl_double *galfa;
     int    *jstat;
#endif
{
  int kpos=0;              
  int kj,kv;               
  int kp;                  
  sisl_double *salfa;           
  sisl_double tbeta,tbeta1;     
  sisl_double td1,td2;          
  sisl_double *t1,*t2;          
  
  
  
  
  if (iv >= ik) goto err152;
  
  
  
  
  *jpl=ik-imy-1;
  
  
  
  
  galfa += *jpl;
  
  
  
  
  galfa[imy] = 1;
  
  
  
  
  for (kj=in+iv-2,in+=ik-1,kv=ik-iv,kp=0; kp<iv; kp++,kv++)
    {
      
      
      
      
      
      if (kp>=imy) tbeta1=(apar - *et)* *galfa/(et[kv] - *et);
      else         tbeta1=(sisl_double)0.0;
      
      
      *jfi=max(1,imy-kp); *jla=min(imy,kj-kp);
      
      
      
      
      for (salfa=galfa+*jfi,t1=et+*jfi,t2=et+*jla; t1<=t2; t1++,salfa++)
	{
	  td1 = apar - *t1;
	  td2 = t1[kv] - apar;
	  tbeta = *salfa/(td1 + td2);
	  salfa[-1] = td2*tbeta + tbeta1;
	  tbeta1 = td1*tbeta;
	}
      
      
      
      
      if (*jla<imy)
	{
	  t1 = et + in;
	  *(salfa-1) = tbeta1+(*t1-apar)* *salfa/(*t1 - *(t2+1));
	} else  *(salfa-1) = tbeta1;
    }
  
  
  
  
  if (iv) (*jfi)--;
  else   *jfi = *jla = imy;
  
  
  
  
  *jstat = 0;
  goto out;
  
  
  
  
 err152:
  *jstat = -152;
  s6err("s1700",*jstat,kpos);
  goto out;
  
 out: 
  return;
}

