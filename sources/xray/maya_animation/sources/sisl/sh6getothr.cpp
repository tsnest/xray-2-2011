#include "pch.h"



#define SH6GETOTHER

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void 
      sh6getother(SISLIntpt *pt,SISLIntpt *pt1,SISLIntpt **pt2,int *jstat)
#else
void sh6getother(pt,pt1,pt2,jstat)
   SISLIntpt *pt;
   SISLIntpt *pt1;
   SISLIntpt **pt2;
   int       *jstat;
#endif   
{
  int kstat;              
  int index,index1;      
  int num;              
  int i;              
  
   *pt2 = SISL_NULL;
   *jstat = 0;
  
  sh6getlist(pt,pt1,&index1,&index,&kstat);
  if(kstat < 0) goto error;
  if(kstat == 1) goto err1;

  if(sh6ismain(pt))  
  {
      if(!sh6ismain(pt1)) goto err1;
      num=0;
      
      if (pt->iinter == SI_SING)
      {
	  *pt2 = SISL_NULL;
          *jstat = 2;
          goto out;
      }
	 
      for(i=0; i < pt->no_of_curves; i++)
      {
	  if(i != index1 && sh6ismain(pt->pnext[i]))
	  {
	      *pt2 = pt->pnext[i];
	      num++;
	  }
      }

      if(num == 0) *jstat = 1; 
      else if(num > 1) 
      {
	  *pt2 = SISL_NULL;
          *jstat = 2;
      }
  }
  else  
  {
      num=0;

      for(i=0; i < pt->no_of_curves; i++)
      {
	  if(i != index1)
	  {
	      *pt2 = pt->pnext[i];
	      num++;
	  }
      }

      if(num > 1) goto err2; 
      if(num == 0) *jstat = 1; 
  }
  
  goto out;
  


err1: *jstat = -1;
      s6err("sh6getother",*jstat,0);
      goto out;



err2: *jstat = -2;
      s6err("sh6getother",*jstat,0);
      goto out;


error:  *jstat = kstat;
        s6err("sh6getother",*jstat,0);
        goto out;

   out:
      return;
}

