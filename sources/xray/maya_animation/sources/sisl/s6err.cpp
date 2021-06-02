#include "pch.h"



#define S6ERR

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void s6err(char *rut,int jstat,int ipos)
#else
void s6err(rut,jstat,ipos)
     char *rut;
     int  jstat;
     int  ipos;
#endif
{
	(void)rut;
	(void)jstat;
	(void)ipos;
   //(void)fprintf(stderr,"\nError status : %d",jstat);
   //(void)fprintf(stderr,"   Call from routine : %s",rut);
   //(void)fprintf(stderr,"   Position : %d\n",ipos);
}

