#include "pch.h"



#define SH6ISMAIN

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
int 
      sh6ismain(SISLIntpt *pt)
#else
int sh6ismain(pt)
    SISLIntpt *pt;
#endif   
{
   int flag = 0;

   if(pt != SISL_NULL && pt->iinter > 0) flag = 1;


   goto out;
   

   
   out :
      return flag;
}







