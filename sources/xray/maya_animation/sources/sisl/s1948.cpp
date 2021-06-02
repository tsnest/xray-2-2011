#include "pch.h"


#define S1948

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1948(sisl_double *ea,sisl_double *ew,int in,int ik,int inlr, 
	 int *nstart,int *jstat)
#else
void s1948(ea,ew,in,ik,inlr,nstart,jstat)
   sisl_double *ea;
   sisl_double *ew;
   int in;
   int ik;
   int inlr;
   int *nstart;
   int *jstat;
#endif     
{
   int ki,kj,kr;           
   int kjs,kjh,krhs,krh;   
   int ki2, ki3;           
   int kik1 = ik-1;        
   sisl_double tsum;            
   sisl_double thelp;           

   
   
   
   for (ki=0; ki<in-inlr; ki++)
     {
	
	
	for (kjs=nstart[ki], kjh=ki+kjs-ik+1, krhs=kik1, kj=kjs;
	 kj<kik1; kjh++, krhs--, kj++)
	  {
	     tsum = (sisl_double)0.0;
	     krh = krhs;
	     for (kr=kjs; kr<kj; krh++,kr++)
	       tsum += ea[ki*ik+kr]*ea[kjh*ik+krh];
	     ea[ki*ik+kj] -= tsum;
	     ea[ki*ik+kj] /= ea[kjh*ik+kik1];
	  }
	
	
	
	tsum = (sisl_double)0.0;
	for (kr=kjs; kr<kik1; kr++)
	  {
	     thelp = ea[ki*ik+kr];
	     tsum += thelp*thelp;
	  }
	
	
	
	tsum = ea[ki*ik+kik1] - tsum;
	if (tsum <= DZERO) goto err106;
        ea[ki*ik+kik1] = sqrt(tsum);
     }
   
   for (ki2=0; ki<in; ki++, ki2++)
     {
	
	
	for (kjs=nstart[ki], kjh=ki+kjs-ik+1, krhs=kik1, kj=0;
	 kj<MIN(ki,in-inlr); kjh++, krhs--, kj++)
	  {
	     tsum = (sisl_double)0.0;
	     krh = nstart[kj];
	     for (kr=kj-kik1+nstart[kj]; kr<kj; krh++,kr++)
	       tsum += ew[ki2*in+kr]*ea[kj*ik+krh];
	     ew[ki2*in+kj] -= tsum;
	     ew[ki2*in+kj] /= ea[kj*ik+kik1];
	  }
	
	
	
	for (ki3=0; kj<ki; kj++, ki3++)
	{
	     tsum = (sisl_double)0.0;
	     for (kr=0; kr<kj; kr++)
		tsum += ew[ki2*in+kr]*ew[ki3*in+kr];
	     ew[ki2*in+kj] -= tsum;
	     ew[ki2*in+kj] /= ew[ki3*in+kj];
	}
	
	
	
	tsum = (sisl_double)0.0;
	for (kr=0; kr<ki; kr++)
	  {
	     thelp = ew[ki2*in+kr];
	     tsum += thelp*thelp;
	  }
	
	
	
	tsum = ew[ki2*in+ki] - tsum;
	if (tsum <= DZERO) goto err106;
        ew[ki2*in+ki] = sqrt(tsum);
     }
   
   
   
   *jstat = 0;
   goto out;
   
   
   
   err106: *jstat = -106;
   goto out;
   
   out:
      return;
}
   

