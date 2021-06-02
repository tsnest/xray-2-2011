#include "pch.h"



#define S6EXISTBOX

#include "sislP.h" 

#if defined(SISLNEEDPROTOTYPES)
int s6existbox(SISLbox *pbox,int itype,sisl_double aepsge)
#else
int s6existbox(pbox,itype,aepsge)
     SISLbox *pbox;
     int    itype;
     sisl_double aepsge;
#endif
                                     
{
   if (pbox->e2min[itype] == SISL_NULL) return(0);  
   
   if (itype != 0 && DNEQUAL(pbox->etol[itype],aepsge))
      return(-1);  
   
   return(1);
}
   

