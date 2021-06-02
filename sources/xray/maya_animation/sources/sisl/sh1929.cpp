#include "pch.h"


#define SH1929

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
      sh1929(sisl_double etau[],int in,int ik,int imu,sisl_double et[],int /*im*/,
	     int ij,sisl_double eah[],int *jmuprm,int *jnu,int *jstat)
#else
void sh1929(etau,in,ik,imu,et,im,ij,eah,jmuprm,jnu,jstat)
   sisl_double etau[];
   int in;
   int ik;
   int imu;
   sisl_double et[];
   int im;
   int ij;
   sisl_double eah[];
   int *jmuprm;
   int *jnu;
   int *jstat;
#endif     
{ 
   int ki,kp;
   int kih,kil,kiu,kkh;
   int k1 = ik - 1;
   sisl_double tbeta,tbeta1;
   sisl_double tj;
   sisl_double td1,td2;
   sisl_double *ssi = SISL_NULL;
   
   
   
   if ((ssi = newarray(ik-1,sisl_double)) == SISL_NULL) goto err101;

   
   
   for (ki=ij+1, *jmuprm=imu; DEQUAL(et[ki],etau[*jmuprm]) && ki<ij+ik;
        ki++, (*jmuprm)--);
   kih = (*jmuprm) + 1;
   *jnu = 0;
   
   

   for (kp=1; kp<=k1; kp++)
     {
	if (DEQUAL(et[ij+kp],etau[kih])) kih++;
	else
	{
	   ssi[*jnu] = et[ij+kp];
	   (*jnu)++;
	}
     }
   
   
   
   eah[k1] = (sisl_double)1.0;
   for (kp=0; kp<(*jnu); kp++)
     {
	tbeta1 = (sisl_double)0.0;
	tj = ssi[kp];
	kih = kp + ik - (*jnu);
	if (kp >= (*jmuprm))
	   tbeta1 = (tj-etau[0])*eah[ik-(*jmuprm)-1]/(etau[kih]-etau[0]);
	kil = (*jmuprm) - kp;
	kiu = in + (*jnu) - kp;
	if (kil < 1 ) kil = 1;
	if (kiu > (*jmuprm)) kiu = (*jmuprm);
	for (ki=kil; ki<=kiu; ki++)
	  {
	     td1 = tj - etau[ki];
	     td2 = etau[ki+kih] - tj;
	     tbeta = eah[ki+ik-(*jmuprm)-1]/(td1+td2);
	     eah[ki+ik-(*jmuprm)-2] = td2*tbeta + tbeta1;
	     tbeta1 = td1*tbeta;
	  }
	kkh = kiu + ik - (*jmuprm) - 1;
	eah[kkh] = tbeta1;
	if (kiu < (*jmuprm)) 
	   eah[kkh] = tbeta1 + 
	   (etau[in+ik-1]-tj)*eah[kkh+1]/(etau[in+ik-1]-etau[kiu+1]);
     }
   
   

   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   out:
      
      
      if (ssi != SISL_NULL) freearray(ssi);
	  
      return;
}
   
   

