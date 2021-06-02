#include "pch.h"



#define S1701

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
s1701(int ij,int imy,int ik,int in,int *jpl,int *jfi,int *jla,
	   sisl_double *et,sisl_double *etau,sisl_double *ep,sisl_double *galfa,int *jstat)
#else
void s1701(ij,imy,ik,in,jpl,jfi,jla,et,etau,ep,galfa,jstat)
     int    ij;
     int    imy;
     int    ik;
     int    in;
     int    *jpl;
     int    *jfi;
     int    *jla;
     sisl_double *et;
     sisl_double *etau;
     sisl_double *ep;
     sisl_double *galfa;
     int    *jstat;
#endif
{
  int kp;                  
  int kv,kkv;              
  sisl_double *ah;              
  sisl_double tbeta,tbeta1;     
  sisl_double td1,td2;          
  sisl_double *tu;              
  
  
  
  
  kp=ij+1; kkv=ij+ik; in--;
  while ((et[kp] == etau[imy]) && kp<kkv) {kp++; imy--;}
  
  
  
  
  for (kp=imy+1,kv=0,ij++; ij<kkv; ij++)
    if (et[ij] == etau[kp]) kp++;
    else      ep[kv++] = et[ij];
  
  
  
  
  *jpl=ik-imy-1;
  
  
  
  
  galfa += *jpl;
  
  
  
  
  galfa[imy] = 1;
  
  
  
  
  for (kp=0,kkv=ik-kv,ij=in+kv-1,in +=ik; kp<kv; kp++,kkv++,ep++)
    {
      
      
      
      
      
      if (kp>=imy) tbeta1=(*ep - *etau)*(*galfa)/(etau[kkv] - *etau);
      else         tbeta1=0;
      
      *jfi=max(1,imy-kp); *jla=min(imy,ij-kp);
      
      
      
      
      for (et=etau+ *jfi,tu=etau+ *jla,ah=galfa+ *jfi; et<=tu; et++,ah++)
	{
	  td1 = *ep - *et;
	  td2 = et[kkv] - *ep;
	  tbeta = *ah/(td1+td2);
	  *(ah-1) = td2*tbeta + tbeta1;
	  tbeta1 = td1*tbeta;
	}
      
      
      
      
      if (*jla<imy)
	{
	  et = etau + in;
	  *(ah-1) = tbeta1+(*et-*ep)*(*ah)/(*et - *(tu+1));
	} else  *(ah-1) = tbeta1;
    }
  
  
  
  
  if (kv) (*jfi)--;
  else   *jfi = *jla = imy;
  
  if ((*jfi)<0) *jfi = 0;
  if ((*jla)>in-ik) *jla = in-ik;
  
  
  
  
  *jstat = 0;
  goto out;
  
  
 out:
  return;
}

