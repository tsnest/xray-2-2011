#include "pch.h"



#define S1941

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
s1941(SISLCurve *pcurve,int icont,int *jstat)
#else
void s1941(pcurve,icont,jstat)
     SISLCurve  *pcurve;
     int        icont;
     int    	*jstat;
#endif
{
  sisl_double *scycl=SISL_NULL;                    
  sisl_double *smatrix=SISL_NULL;                   
  sisl_double *salloc=SISL_NULL;                    
  sisl_double *salfa=SISL_NULL;                     
  sisl_double *spek=SISL_NULL;                      
  sisl_double *scoef=SISL_NULL;                     
  sisl_double *sb=SISL_NULL;                        
  sisl_double *sfrom,*sto;
  sisl_double *sp;                             
  sisl_double *st1=SISL_NULL;                       
  sisl_double *stx=SISL_NULL;                       

  int    kdim = pcurve->idim;
  int    kk = pcurve->ik;
  int    kn = pcurve->in;

  int    kcont;                           
  int    kmult;                           
  int    ki,kj,kl;
  int    kperlength;
  int    kant;
  int    kleft=0;                         
  int    kpl,kfi,kla;                     
  int    kstat;
  int    *mpiv=SISL_NULL;                      
  int    kpos = 0;
  int    knst1;                           
  int    knstx;                           




  

  if (icont < 0) goto finished;
  kcont = icont;
  if (icont >= kk-2) icont = kk-2;

  
  kmult = kk - kcont - 1;

  

  kant = kk-kmult;


  

  mpiv = new0array(2*kk,sisl_int);
  if (mpiv == SISL_NULL) goto err101;

  salloc = new0array(3*kn+9*kk+4*kk*kk+kdim*kn,sisl_double);
  if (salloc == SISL_NULL) goto err101;
  scycl = salloc;                  
  smatrix = scycl + kn + kk;  
  salfa = smatrix + 4*kk*kk;     
  scoef = salfa + kk;           
  sb    = scoef + kdim*kn;    
  sp    = sb + 2*kk;              
  st1   = sp + kk;                
  stx   = st1 + kn + 2*kk;       



  

  memcopy(scoef,pcurve->ecoef,kdim*kn,sisl_double);



  


  

  memcopy(scycl,pcurve->et,kn+kk,sisl_double);

  

  kperlength = kn - kk + kmult;

  

  for (ki=kk-kmult-1 ; 0<=ki ; ki--)
    {
      scycl[ki] = scycl[kk-1] - (scycl[kn] - scycl[ki+kperlength]);
    }


  

  for (ki=kmult ; ki < kk ; ki++)
    {
      scycl[kn+ki] = scycl[kn] + (scycl[kk+ki-kmult] - scycl[kk-1]);

    }
      

  memcopy(st1,scycl,kant,sisl_double);
  memcopy(st1+kant,pcurve->et,kn+kk,sisl_double);
  memcopy(st1+kant+kk+kn,scycl+kn+kk-kant,kant,sisl_double);
  knst1 = kn + 2*kant;

  memcopy(stx,scycl,kn,sisl_double);
  memcopy(stx+kn,st1+kant+kn,kk+kant,sisl_double);
  knstx = kn + kant;

  


  


  


  

  for (ki=kant,spek=smatrix ; ki <kk+kant ; ki++, spek+=kk)
    {
      

      s1219(stx,kk,kn,&kleft,st1[ki],&kstat);
      if (kstat<0) goto error;

      s1701(ki,kleft,kk,knstx,&kpl,&kfi,&kla,st1,stx,sp,salfa,&kstat);
      if(kstat<0) goto error;

      

      memcopy(spek+kfi,salfa+kpl+kfi,kla-kfi+1,sisl_double);
    }



  

  s6lufacp(smatrix,mpiv,kk,&kstat);
  if (kstat<0) goto error;

  


  for (kl=0 ; kl<kdim ; kl++)
  {
     for (kj=0, sfrom=(pcurve->ecoef)+kl,sto=sb ;
      kj<kk ; kj++,sfrom+=kdim,sto++)
	*sto = *sfrom;
     
     
     
     s6lusolp(smatrix,sb,mpiv,kk,&kstat);
     if (kstat<0) goto error;
     
     
     
     for (kj=0, sto=scoef+kl,sfrom=sb ;
      kj<kk ; kj++,sfrom++,sto+=kdim)
	*sto = *sfrom;
  }


  


  for (ki=0,spek=smatrix ; ki<kk*kk ; ki++,spek++) *spek = DZERO;


  for (ki=kn-kk ,spek=smatrix ; ki <kn ; ki++, spek+=kk)
    {
      s1219(scycl,kk,kn,&kleft,stx[ki],&kstat);
      if (kstat<0) goto error;

      s1701(ki,kleft,kk,kn,&kpl,&kfi,&kla,stx,scycl,sp,salfa,&kstat);
      if(kstat<0) goto error;

      

      memcopy(spek+kfi-(kn-kk),salfa+kpl+kfi,kla-kfi+1,sisl_double);
    }



  

  s6lufacp(smatrix,mpiv,kk,&kstat);
  if (kstat<0) goto error;

  

  for (kl=0 ; kl<kdim ; kl++)
  {
     for (kj=0, sfrom=scoef+kdim*(kn-kk)+kl,sto=sb ;
      kj<kk ; kj++,sfrom+=kdim,sto++)
	*sto = *sfrom;
     
     
     
     s6lusolp(smatrix,sb,mpiv,kk,&kstat);
     if (kstat<0) goto error;
     
     
     
     for (kj=0, sto=scoef+kdim*(kn-kk)+kl,sfrom=sb ;
      kj<kk ; kj++,sto+=kdim,sfrom++)
	*sto = *sfrom;
  }


  

  memcopy(pcurve->ecoef,scoef,kdim*kn,sisl_double);
  memcopy(pcurve->et,scycl,kn+kk,sisl_double);

  
  pcurve->cuopen = SISL_CRV_PERIODIC;


  

 finished:

  *jstat = 0;
  goto out;

  

 err101:
  *jstat = -101;
  s6err("s1941",*jstat,kpos);
  goto out;



  

  error :
    *jstat = kstat;
  s6err("s1941",*jstat,kpos);
  goto out;
 out:

  
  if (salloc != SISL_NULL) freearray(salloc);
  if (mpiv != SISL_NULL) freearray(mpiv);

  return;

}

