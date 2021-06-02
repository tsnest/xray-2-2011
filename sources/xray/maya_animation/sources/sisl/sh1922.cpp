#include "pch.h"


#define SH1922

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
      sh1922(sisl_double et[],int im,int ik,sisl_double etau[],int in,sisl_double ea[],
	     int nfirst[],int nlast[],int *jstat)
#else
void sh1922(et,im,ik,etau,in,ea,nfirst,nlast,jstat)
   sisl_double et[];
   int im;
   int ik;
   sisl_double etau[];
   int in;
   sisl_double ea[];
   int nfirst[];
   int nlast[];
   int *jstat;
#endif     
{ 
   int kstat = 0;
   int kj1,kj2;
   int kih1,kih2;
   int ki,kj,kjh,kkj;
   int kmu;
   int kmuprm;
   int knu;
   sisl_double *sah = SISL_NULL;
   
   if ((sah = new0array(ik,sisl_double)) == SISL_NULL) goto err101;
   
   

   for (kmu=in+ik-1; DEQUAL(etau[kmu-1],etau[kmu]); kmu--);
   
   for (kjh=im-1; et[kjh+ik]>etau[kmu]; kjh--);
   
   ki = kjh;
   if (DEQUAL(et[kjh+ik],etau[kmu]))
      {
	 for (; DEQUAL(et[ki-1+ik],et[kjh+ik]); ki--);
      }
      
   kj2 = MIN(im-1,ki+in+ik-kmu-1);
   
   
       
   for (kmu=0; DEQUAL(etau[kmu+1],etau[kmu]); kmu++);
   
   for (kjh=0; et[kjh]<etau[kmu]; kjh++);
   
   ki = kjh;
   if (DEQUAL(et[kjh],etau[kmu]))
      {
	 for (; DEQUAL(et[ki+1],et[kjh]); ki++);
      }
   kj1 = MAX(0,ki-kmu);      
      
   
      
   memzero(ea,im*ik,sisl_double);
   
   
   
   
   for (kj=0; kj<kj1; kj++)
     {
	nfirst[kj] = ik;
	nlast[kj] = 0;
     }
   
   
   
   for (kj=kj2+1; kj<im; kj++)
     {
	nfirst[kj] = ik;
	nlast[kj] = 0;
     }
   
   
   
   for (kj=kj1; kj<=kj2; kj++)
     {
	for (; etau[kmu+1] <= et[kj]; kmu++);
	kkj = kj;
	sh1929(etau,in,ik,kmu,et,im,kkj,sah,&kmuprm,&knu,&kstat);
	if (kstat < 0) goto error;
		       
       
		       
       nfirst[kj] = MAX(kmuprm-knu,0);
       nlast[kj] = MIN(kmuprm,in-1);
       kih1 = nfirst[kj] + ik - kmuprm - 1;
       kih2 = nlast[kj] + ik - kmuprm - 1;
       for (ki=kih1; ki<=kih2; ki++)
	 ea[kj*ik+ki] = sah[ki];
     }

   

   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   
   
   error: *jstat = kstat;
   goto out;
   
   out:
      
      
      if (sah != SISL_NULL) freearray(sah);
	  
      return;
}
   

