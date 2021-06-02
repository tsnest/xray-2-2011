#include "pch.h"


#define S1947

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1947(sisl_double ea[], int nfirst[], int nlast[], int ik, int im, 
	 sisl_double etau[], int in, int incont, sisl_double ew[], int inlr, 
	 int *jnred, sisl_double efac[], int *jstat)
#else
   void s1947(ea, nfirst, nlast, ik, im, etau, in, incont, ew, inlr, 
	      jnred, efac, jstat)
      sisl_double ea[];
      int    nfirst[];
      int    nlast[];
      int    ik;
      int    im;
      sisl_double etau[];
      int    in;
      int    incont;
      sisl_double ew[];
      int    inlr;
      int    *jnred;
      sisl_double efac[];
      int    *jstat;
#endif
{
   int ki, kj, kr, kih;       
   int khindx;                
   sisl_double tdiv, th;           
   sisl_double *sr1 = SISL_NULL;   
   sisl_double *sr2 = SISL_NULL;   
   sisl_double *stw1 = SISL_NULL;  
   sisl_double *stw2 = SISL_NULL;  
   sisl_double *shelp = SISL_NULL; 
      
   
   
   if (DEQUAL(etau[ik-1], etau[ik]) || DEQUAL(etau[in-1], etau[in])) 
      goto err112;
   
   
   
   if ((sr1 = new0array(2*incont*incont+3*incont, sisl_double)) == SISL_NULL) 
      goto err101;
   sr2 = sr1 + incont*incont;
   stw1 = sr2 + incont*incont;
   stw2 = stw1 + incont;
   shelp = stw2 + incont;
   
   
     
   
   
   stw1[0] = etau[ik] - etau[ik-1];
   stw2[0] = etau[in] - etau[in-1];
      
   
   
   sr1[0] = sr2[0] = (sisl_double)1;
   for (ki=1; ki<incont; ki++)
   {
      stw1[ki] = etau[ik+ki] - etau[ik-1];
      stw2[ki] = etau[in] - etau[in-ki-1];
      
      sr1[ki*incont] = -sr1[(ki-1)*incont]/stw1[0];
      sr2[ki*incont] = sr2[(ki-1)*incont]/stw2[0];
      for (kj=1; kj<=ki; kj++)
      {
	 sr1[ki*incont+kj] = sr1[(ki-1)*incont+kj-1]/stw1[kj-1] -
	    sr1[(ki-1)*incont+kj]/stw1[kj];
	 sr2[ki*incont+kj] = -sr2[(ki-1)*incont+kj-1]/stw2[kj-1] +
	    sr2[(ki-1)*incont+kj]/stw2[kj];
      }
   }

   
   
   for (ki=1; ki<incont; ki++)
   {
      tdiv = sr2[ki*incont+ki];
      for (kj=ki; kj>=0; kj--)
      {
	 th = sr1[ki*incont+kj];
	 for (kr = ki-1; kr>= kj; kr--)
	    th -= sr2[ki*incont+kr]*sr2[kr*incont+kj];
	 shelp[kj] = th/tdiv;
      }
      memcopy(sr2+ki*incont, shelp, ki+1, sisl_double);
   }
   
   
   
   for (kj=im-1; kj>=0; kj--)
   {
      if (nlast[kj] < in-incont) break;  
      
      for (kih=0, ki=0; ki<incont; ki++)
      {
	 if (nlast[kj] < in-ki-1) continue;  
	 
	 for (kr=0; kr<=ki; kr++)
	    ew[(kj-im+inlr)*incont+kr] += sr2[ki*incont+kr]*
	       ea[(kj+1)*ik-kih-1];
	    
	 kih++;
      }
	 
      
      
      
      if (nlast[kj] >= in-incont) 
      {
	 khindx = nlast[kj] - in + incont + 1;
	 
	 
	 
	 for (ki=ik-1; ki>=ik-in+incont+nfirst[kj]; ki--)
	    ea[kj*ik+ki] = ea[kj*ik+ki-khindx];
	 for (; ki>=0; ki--)
	    ea[kj*ik+ki] = DZERO;
      }
      nlast[kj] = in-incont-1;
      if (nlast[kj] < nfirst[kj]) (*jnred)++;
      
      
      
      if (nfirst[kj] < incont)
      {
	 for (ki=nfirst[kj]; ki<incont; ki++)
	    ew[(kj-im+inlr)*incont+ki] += ea[kj*ik+ik-1-nlast[ki]+nfirst[ki]];
	 nfirst[kj] = incont;
      }
   }
   
   
   
   memcopy(efac, sr2, incont*incont, sisl_double);
   *jstat = 0;
   goto out;
   
   
   
   err101 : *jstat = -101;
   s6err("s1947", *jstat, 0);
   goto out;

   
   
   err112 : *jstat = -112;
   s6err("s1947", *jstat, 0);
   goto out;
   
   out:
      
      
      if (sr1 != SISL_NULL) freearray(sr1);
      
      return;
}

