#include "pch.h"



#define S6RATDER

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
s6ratder(sisl_double eder[],int idim,int ider,sisl_double gder[],int *jstat)
#else
void s6ratder(eder,idim,ider,gder,jstat)
     sisl_double eder[];
     int    idim;
     int    ider;
     sisl_double gder[];
     int    *jstat;
#endif
{
  int kpos=0;          
  sisl_double w0;           
  int ki;              
  int id;              
  int *binom = SISL_NULL;   
  sisl_double sum;          
  int idimp1;          
  int iw;              
  int igder;           
  int i,j,k;           
  int iwfix;           

  if (ider<0) goto err178;
  if (idim<1) goto err102;

  idimp1 = idim + 1;

  

  w0 = eder[idim];
  if (DEQUAL(w0,DZERO)) w0 = (sisl_double)1.0;

  

  binom = newarray(ider+1, sisl_int);
  if(binom == SISL_NULL) goto err179;

  binom[0] = 1;


  

  for(ki=0; ki<idim; ki++)
  {
      gder[ki] = eder[ki] / w0;
  }



  

  for(id=1,j=idim,k=idimp1; id<=ider; id++,k++)
  {
      

      binom[id] = 1;

      for(i=id-1; i>=1; i--)
      {
	  binom[i] += binom[i-1];
      }


      

      iwfix = k + idim;

      for(ki=0; ki<idim; ki++,j++,k++)
      {
	  

	  sum = eder[iwfix] * gder[ki];

          for(i=1,igder=idim+ki,iw=iwfix-idimp1;
	  i<id;
	  i++,igder+=idim,iw-=idimp1)
          {
	      sum += (sisl_double)binom[i] * eder[iw] * gder[igder];
          }

	  gder[j] = (eder[k] - sum) / w0;

      }

  }

  


  *jstat = 0;
  goto out;

 err102: *jstat = -102;
         s6err("s6ratder",*jstat,kpos);
         goto out;

 err178: *jstat = -178;
         s6err("s6ratder",*jstat,kpos);
         goto out;


 err179: *jstat = -179;
         s6err("s6ratder",*jstat,kpos);
         goto out;


out:
  if (binom != SISL_NULL) freearray(binom);

  return;
}

