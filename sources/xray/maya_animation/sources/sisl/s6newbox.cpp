#include "pch.h"



#define S6NEWBOX

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void s6newbox(SISLbox *pbox,int inum,int itype,
	      sisl_double aepsge,int *jstat)
#else
void s6newbox(pbox,inum,itype,aepsge,jstat)
     SISLbox *pbox;
     int    inum;
     int    itype;
     sisl_double aepsge;
     int    *jstat;
#endif
                                     
{
   int knum = (inum == 1) ? inum : 2*inum;  

   if (itype < 0 || itype > 2) goto err126;
   
   
   
   if (pbox->e2min[itype] == SISL_NULL)
   {
      if ((pbox->e2min[itype] = newarray(knum,sisl_double)) == SISL_NULL) goto err101;
      if ((pbox->e2max[itype] = newarray(knum,sisl_double)) == SISL_NULL) goto err101;
   }
  
   
   
   if (itype != 0) pbox->etol[itype] = aepsge;
   
   *jstat = 0;
   goto out;
   
   
   
   err101 : *jstat = -101;
   goto out;
   
   
   
   err126 : *jstat = -126;
   goto out;
   
   out :
      return;
}

