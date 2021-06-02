#include "pch.h"

#define S1942

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1942(SISLCurve *pc1,SISLCurve *pc2,int idim,sisl_double ea[],int nstart[],
	     int nstop[],sisl_double emxerr[],sisl_double el2err[],int *jstat)
#else
void s1942(pc1,pc2,idim,ea,nstart,nstop,emxerr,el2err,jstat)
   SISLCurve *pc1;
   SISLCurve *pc2;
   int idim;
   sisl_double ea[];
   int nstart[];
   int nstop[];
   sisl_double emxerr[];
   sisl_double el2err[];
   int *jstat;
#endif     
{
   int ki,kj,kr;
   int kjh;
   int kk = pc1->ik;
   int km = pc1->in;
   int kj1,kj2;
   sisl_double tkindv = (sisl_double)1.0/(sisl_double)kk;
   sisl_double thelp;
   sisl_double *st = pc1->et;
   sisl_double *sd = pc1->ecoef;
   sisl_double *sc = pc2->ecoef;
   sisl_double *stemp = SISL_NULL;
   
   
   
   if ((stemp = newarray(idim,sisl_double)) == SISL_NULL) goto err101;
  
   
   
   memzero(stemp,idim,sisl_double);
   memzero(emxerr,idim,sisl_double);
   memzero(el2err,idim,sisl_double);
		  
   
   
   for (ki=0; ki<km; ki++)
     {
	memzero(stemp,idim,sisl_double);
	
	
	
	kj1 = nstart[ki];
	kj2 = nstop[ki];
	for (kjh=kk+kj1-kj2-1, kj=kj1; kj<=kj2; kjh++, kj++)
	  {
	     thelp = ea[ki*kk+kjh];
	     for (kr=0; kr<idim; kr++)
	       stemp[kr] += thelp*sc[kj*idim+kr];
	  }
	
	
	
	thelp = (st[ki+kk] - st[ki])*tkindv;  
	for (kr=0; kr<idim; kr++)
	  {
	     stemp[kr] = fabs(stemp[kr] - sd[ki*idim+kr]);
	     el2err[kr] += thelp*stemp[kr]*stemp[kr];
	     if (stemp[kr] > emxerr[kr]) emxerr[kr] = stemp[kr];
	  }
     }
   for (kr=0; kr<idim; kr++)
     el2err[kr] = sqrt(el2err[kr]);
	
   
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   out:
      
      
      if (stemp != SISL_NULL) freearray(stemp);
	  
      return;
}
   

