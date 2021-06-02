#include "pch.h"


#define S1946

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1946(sisl_double ea[],sisl_double ew1[],int nfirst[],int nlast[],sisl_double ed[],
	 sisl_double ec[],int ik,int in,int im,int idim,int ilend,int irend,
	 int inlr, int inlc,int *jstat)
#else
void s1946(ea,ew1,nfirst,nlast,ed,ec,ik,in,im,idim,ilend,irend,inlr,inlc,jstat)
   sisl_double ea[];
   sisl_double ew1[];
   int nfirst[];
   int nlast[];
   sisl_double ed[];
   sisl_double ec[];
   int ik;
   int in;
   int im;
   int idim;
   int ilend;
   int irend;
   int inlr;
   int inlc;
   int *jstat;
#endif     
{ 
   int ki,kj,kr;
   int kjst;
   int knk1;
   sisl_double th;
   sisl_double *shelp = SISL_NULL;

   
   
   if (ilend + irend >= in) goto err203;

   
			    
   if ((shelp = new0array(idim,sisl_double)) == SISL_NULL) goto err101;
   
   
   
   for (ki=0; ; ki++)
     {
	
	
	if (ki >= im) break;
	
	
	
	if (nfirst[ki] >= ilend && inlc == 0) break;
	if (nfirst[ki] >= ilend && ki < im - inlr) continue;
	
	
	
	kjst = MIN(ilend-1,nlast[ki]);
	
	
	
	for (kj=nfirst[ki]; kj<=kjst; kj++)
	  {
	     th = ea[ki*ik+ik-nlast[ki]+kj-1];
	     for (kr=0; kr<idim; kr++)
	       shelp[kr] += th*ec[kj*idim+kr];
	  }
	
	
	
	if (inlc > 0 && ki >= im-inlr)
	{
	   for (kj=0; kj<MIN(ilend,inlc); kj++)
	   {
	      th = ew1[(ki-im+inlr)*inlc+kj];
	      for (kr=0; kr<idim; kr++)
		 shelp[kr] += th*ec[kj*idim+kr];
	   }
	}
	
	
	
	for (kr=0; kr<idim; kr++)
	  {
	     ed[ki*idim+kr] -= shelp[kr];
	     shelp[kr] = (sisl_double)0.0;
	  }
     }
   
   
   
   for (knk1=in-irend, ki=im-1; ; ki--)
     {
	
	
	if (ki < 0) break;
	
	
	
	if (nlast[ki] < knk1) break;
	
	
	
	kjst = MAX(knk1,nfirst[ki]);
	
	
	
	for (kj=kjst; kj<=nlast[ki]; kj++)
	  {
	     th = ea[ki*ik+ik-nlast[ki]+kj-1];
	     for (kr=0; kr<idim; kr++)
	       shelp[kr] += th*ec[kj*idim+kr];
	  }
	
	
	
	for (kr=0; kr<idim; kr++)
	  {
	     ed[ki*idim+kr] -= shelp[kr];
	     shelp[kr] = (sisl_double)0.0;
	  }
     }
   
   
	      
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   
   
   err203: *jstat = -203;
   goto out;
   
   out:
      
      
      if (shelp != SISL_NULL) freearray(shelp);
	  
      return;
}
   

