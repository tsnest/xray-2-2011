#include "pch.h"


#define S1949

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1949(sisl_double *ea,sisl_double *ew,sisl_double *eb,int in,int ik,int inlr,
	 int idim,int *nstart,int *jstat)
#else
void s1949(ea,ew,eb,in,ik,inlr,idim,nstart,jstat)
   sisl_double *ea;
   sisl_double *ew;
   sisl_double *eb;
   int in;
   int ik;
   int inlr;
   int idim;
   int *nstart;
   int *jstat;
#endif     
{
   int ki,kj,kr;         
   int ki1,ki2,ki3,kih,kim;  
   int kjs,kjh; 
   int kik1 = ik-1;      
   sisl_double thelp;         
   sisl_double *ssum=SISL_NULL;    

   
   
   if ((ssum = new0array(idim,sisl_double)) == SISL_NULL) goto err101;
   
   
   
   for (ki=0; ki<in-inlr; ki++)
     {
	ki1 = ki - 1;
	memzero(ssum, idim, sisl_double);
	
	
	
	for (kjs=nstart[ki], kjh=ki+kjs-ik+1, kj=kjs;
	 kj<kik1; kjh++, kj++)
	  {
	     thelp = ea[ki*ik+kj];
	     for (kr=0; kr<idim; kr++)
	       ssum[kr] += thelp*eb[kjh*idim+kr];
	  }
	
	
	
	if (DEQUAL(ea[ki*ik+kik1],DZERO)) goto err106;
	
	thelp = (sisl_double)1.0/ea[ki*ik+kik1];
	for (kr=0; kr<idim; kr++)
	  eb[ki*idim+kr] = (eb[ki*idim+kr] - ssum[kr])*thelp;
     }

   
   
   for (ki2=0; ki<in; ki++,ki2++)
     {
	memzero(ssum, idim, sisl_double);
	
	
	
	for (kj=0; kj<ki; kj++)
	  {
	     thelp = ew[ki2*in+kj];
	     for (kr=0; kr<idim; kr++)
	       ssum[kr] += thelp*eb[kj*idim+kr];
	  }
	
	
	
	if (DEQUAL(ew[ki2*in+ki],DZERO)) goto err106;
	
	thelp = (sisl_double)1.0/ew[ki2*in+ki];
	for (kr=0; kr<idim; kr++)
	  eb[ki*idim+kr] = (eb[ki*idim+kr] - ssum[kr])*thelp;
     }
   
   
   
   for (ki=in-1, ki1=in-1, kih=0;
    kih<in; ki--, kih++)
     {
	
	
	for (;;ki1--)
	  if (nstart[ki1] < ik-ki1+ki) break;
	
	memzero(ssum, idim, sisl_double);
	
	
	
	for (kj=in-1, ki3=inlr-1; kj>MAX(ki, in-inlr-1); kj--, ki3--)
	{
	  thelp = ew[ki3*in+ki];
	  for (kr=0; kr<idim; kr++)
	     ssum[kr] += thelp*eb[kj*idim+kr];
	}
	
	
	
	for (kjs=ki+1, kim=ik-kjs+ki-1, kj=kjs;
	 kj<=MIN(ki1,in-inlr-1); kim--, kj++)
	  {
	     thelp = ea[kj*ik+kim];
	     for (kr=0; kr<idim; kr++)
	       ssum[kr] += thelp*eb[kj*idim+kr];
	  }
	
	if (ki >= in-inlr)
	   thelp = (sisl_double)1/ew[(ki-in+inlr)*in+ki];
	else
	   thelp = (sisl_double)1.0/ea[ki*ik+ik-1];
	for (kr=0; kr<idim; kr++)
	  eb[ki*idim+kr] = (eb[ki*idim+kr] - ssum[kr])*thelp;
     }
   
   
   
   
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   
   
   err106: *jstat = -106;
   goto out;
   
   out:
      
      
      if (ssum != SISL_NULL) freearray(ssum);
	  
      return;
}
   

