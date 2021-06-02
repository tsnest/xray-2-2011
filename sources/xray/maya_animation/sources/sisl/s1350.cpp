#include "pch.h"


#define S1350

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void s1350(sisl_double ep[],sisl_double epar[],
	   int im,int idim,int ik,
	   SISLCurve **rc, int *jstat)
#else
void s1350(ep,epar,im,idim,ik,rc,jstat)
     sisl_double ep[];
     sisl_double epar[];
     int im;
     int idim;
     int ik;	   
     SISLCurve **rc;
     int    *jstat;
#endif
{
  int i, j, k;                   
  int kic, kit, kw1, kw2;        
  sisl_double ts = xray::memory::uninitialized_value<sisl_double>(), tw1, tw2;           
  int kpos = 0;                  
  int in;
  int jidim;                     
  int jidimp1;                   
  sisl_double *et = SISL_NULL;             
  sisl_double *ec = SISL_NULL;             
  sisl_double ikinv;                  
  int kclosed;                   

  
  
  if (im < 2 || idim < 1 || ik < 2) goto err103;

  

  in = (ik-1)*im + 2 - ik;
  et = newarray(in+ik, sisl_double);
  ec = newarray(in*idim, sisl_double);
  if (et==SISL_NULL || ec == SISL_NULL) goto err101;

  

  ikinv = sisl_double(1.)/(ik-1);

  

  for(i=0; i<ik; i++)
      et[i] = epar[0];
  for(i=0; i<idim; i++)
      ec[i] = ep[i];
  
  

  kic = idim;
  kit = ik;
  for(j=0, jidim=0, jidimp1=idim; j<im-1; j++, jidim+=idim, 
       jidimp1+=idim)
    {
      ts = epar[j+1];
      
      

      kw1 = ik-1;
      kw2 = 0;
      for (i=1; i<ik; i++)
	{
	  et[kit] = ts;
	  kit++;
	  kw1--;
	  kw2++;
	  tw1 = kw1*ikinv;
	  tw2 = kw2*ikinv;
	  for (k=0; k<idim; k++)	 
	    ec[kic + k] = tw1*ep[jidim + k] + 
	      tw2*ep[jidimp1 + k];
	  kic += idim;
	}
    }

  

  et[kit] = ts;
  if ((*rc = newCurve(in,ik,et,ec,1,idim,2)) == SISL_NULL)
        goto err101;

  
  
  for (kclosed=1, i=0; i<idim; i++)
     if (DNEQUAL(ep[i], ep[(im-1)*idim+i])) kclosed = 0;
  if (kclosed) (*rc)->cuopen = SISL_CRV_CLOSED;
     
  
  
  *jstat = 0;
  goto out;

  

  err101 :
    *jstat = -101;
    if (et != SISL_NULL) freearray(et);  
    if (ec != SISL_NULL) freearray(ec);
    goto out;

  

 err103: 
  *jstat = -103;
  s6err("s1350",*jstat,kpos);
  goto out;
  
  

 out:
  return;
}

