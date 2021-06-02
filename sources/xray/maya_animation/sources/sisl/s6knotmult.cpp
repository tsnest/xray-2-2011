#include "pch.h"



#define S6KNOTMULT

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
int 
s6knotmult(sisl_double et[],int ik,int in,int *ileft,sisl_double ax,int *jstat)
#else
int s6knotmult(et,ik,in,ileft,ax,jstat)
     sisl_double et[];
     int    in;
     int    ik;
     int    *ileft;
     sisl_double ax;
     int    *jstat;
#endif
                                     
{
  int kpos=0;         
  int kstat;          
  int kmult=0;        
  int ki;             
  
  
  
  s1219(et,ik,in,ileft,ax,&kstat);
  if (kstat<0) goto error;
  
  if (et[*ileft] == ax)
    {
      kmult = 1;
      ki    = *ileft-1;
      for (ki=(*ileft)-1; 0 <= ki; ki--)
        if (et[ki] == ax) kmult++;
    }
  if (et[in] == ax)
    {
      for (ki=in ; ki<in+ik;ki++)
        if (et[ki] == ax) kmult++;
    }
  
  *jstat = 0;
  goto out;


error:  *jstat = kstat;
        s6err("s6knotmult",*jstat,kpos);
        goto out;

out:
return(kmult);
}

