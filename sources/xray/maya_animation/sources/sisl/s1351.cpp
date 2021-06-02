#include "pch.h"


#define S1351

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void s1351(sisl_double ep[],int ipar,
	   int im,int idim,int ik,
	   SISLCurve **rc, int *jstat)
#else
void s1351(ep,ipar,im,idim,ik,rc,jstat)
     sisl_double ep[];
     int ipar;
     int im;
     int idim;
     int ik;	   
     SISLCurve **rc;
     int    *jstat;
#endif
{
  int stat = 0;                   
  int kpos = 0;
  int par=0;                     
  sisl_double *epar = SISL_NULL;            
  int i;                          
  int kpek1, kpek2;              

  
  
  if (im < 2 || idim < 1 || ik < 2) goto err103;
  if (ipar < 1 || ipar > 2) goto err103;

  

  if( (epar = newarray(im, sisl_double)) == SISL_NULL ) goto err101;
  epar[0] = (sisl_double) 0.0;

  

  if (ipar == 1)
    {
      

      kpek1 = 0;
      for (i=1; i<im; i++)
	{
	  kpek2 = kpek1 + idim;
	  epar[i] = epar[i-1] + s6dist(&ep[kpek2], &ep[kpek1], idim);
	  kpek1 = kpek2;
	}
      if (epar[im-1] == 0.) par=2;
    }

  if (ipar == 2 || par == 2)
    

    for (i=1; i<im; i++)
      epar[i] = sisl_double(i);
      
  

  s1350(ep, epar, im, idim, ik, rc, &stat);
  if (stat<0) goto error;
  
  

  *jstat = 0;
  goto out;


  

  err101 :
    *jstat = -101;
    s6err("s1351",*jstat,kpos);
    goto out;

  

  err103: 
    *jstat = -103;
    s6err("s1351",*jstat,kpos);
    goto out;
  
  

  error:
    *jstat = stat;
    s6err("s1351",*jstat,kpos);
    goto out;

  

  out:
    if(epar != SISL_NULL) freearray(epar);
    return;
}

