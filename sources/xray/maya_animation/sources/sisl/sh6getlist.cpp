#include "pch.h"



#define SH6GETLIST

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
      sh6getlist(SISLIntpt *pt1,SISLIntpt *pt2,int *index1,int *index2,int *jstat)
#else
void sh6getlist(pt1,pt2,index1,index2,jstat)
   SISLIntpt *pt1;
   SISLIntpt *pt2;
   int       *index1;
   int       *index2;
   int       *jstat;
#endif   
{
   *index1 = -1;
   *index2 = -1;

   *jstat=0;

   

   *index1 = sh6getprev(pt1,pt2);
   *index2 = sh6getprev(pt2,pt1);

   if(*index1 >= 0 && *index2 < 0) goto err1;
   if(*index2 >= 0 && *index1 < 0) goto err1;

   if(*index1 < 0 && *index2 < 0) *jstat=1;


   goto out;

err1:
   

   *jstat = -1;
   s6err("sh6getlist",*jstat,0);
   goto out;
   
   out :
      return;
}

