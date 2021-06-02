#include "pch.h"


#define S1951

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1951(sisl_double etau[], sisl_double ecoef[], int in, int ik, int idim, 
	 int ilend, int irend, int incont, sisl_double efac[])
#else
void s1951(etau, ecoef, in, ik, idim, ilend, irend, incont, efac)
   sisl_double etau[];
   sisl_double ecoef[];
   int    in;
   int    ik;
   int    idim;
   int    ilend;
   int    irend;
   int    incont;
   sisl_double efac[];
#endif     
{
   int ki, kj, kr;   
   int kstop;
   sisl_double tw;

   
   
   for (kstop=in-MAX(incont,irend), ki=ilend; ki<kstop; ki++)
     {
	tw = sqrt((sisl_double)ik/(etau[ki+ik] - etau[ki]));
	for (kj=0; kj<idim; kj++)
	  ecoef[ki*idim+kj] *= tw;
     }  
   
   
   
   for (ki=0; ki<incont; ki++)
   {
      for (kr=0; kr<idim; kr++)
      {
	 ecoef[(in-ki-1)*idim+kr] = DZERO;
	 for (kj=0; kj<=ki; kj++)
	    ecoef[(in-ki-1)*idim+kr] += ecoef[kj*idim+kr]*efac[ki*incont+kj];
      }
   }
   
}
   

