#include "pch.h"



#define SH6GETPREV

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
int 
      sh6getprev(SISLIntpt *pt1,SISLIntpt *pt2)
#else
int sh6getprev(pt1,pt2)
   SISLIntpt *pt1;
   SISLIntpt *pt2;
#endif   
{
   int       ncurv;   
   int       index;   

   index = -1;

   if(pt1 == SISL_NULL || pt2 == SISL_NULL) goto out;

   ncurv = pt1->no_of_curves;  

   index=0;
   while(index < ncurv && pt1->pnext[index] != pt2) index++;
   if(index == ncurv) index = -1;  

   goto out;

   out :
      return index;
}

