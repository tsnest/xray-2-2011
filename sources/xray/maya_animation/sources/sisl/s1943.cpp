#include "pch.h"


#define S1943

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1943(SISLCurve *pcurve,sisl_double etau[],int ik,int in,int ileftfix,
	     int irightfix,int incont,SISLCurve **rnewcurve,
	     sisl_double gmaxerr[],sisl_double gl2err[],int *jstat)
#else
void s1943(pcurve,etau,ik,in,ileftfix,
	    irightfix,incont,rnewcurve,gmaxerr,gl2err,jstat)
     SISLCurve *pcurve;
     sisl_double etau[];
     int ik;
     int in;
     int ileftfix;
     int irightfix;
     int incont;
     SISLCurve **rnewcurve;
     sisl_double gmaxerr[];
     sisl_double gl2err[];
     int *jstat;
#endif     
{
  int kstat = 0;            
  int kdim = pcurve->idim;  
  int kj;                   
  int kkind = 1;            
  int kcopy = 1;            
  int kn = pcurve->in;      
  int knlr;                 
  int knred = 0;            
  int knormr;               
  sisl_double *scoef = SISL_NULL;     
  sisl_double *sa = SISL_NULL;        
  sisl_double *sa2 = SISL_NULL;       
  sisl_double *sw1 = SISL_NULL;       
  sisl_double *sb = SISL_NULL;        
  sisl_double *sw2 = SISL_NULL;       
  sisl_double *sfac = SISL_NULL;      
  int *lfirst = SISL_NULL;       
  int *lfirst2 = SISL_NULL;      
  int *llast = SISL_NULL;        
  int *llast2 = SISL_NULL;       
  int *l2sta = SISL_NULL;        
  sisl_double *sc = SISL_NULL;        
  int kk = pcurve->ik;      
  int knh;                  

  

  if (kk != ik) goto err109;

  
  
  if (incont < 0) incont = 0;
  if (incont >= ik) incont = ik-1;
  
  if (ileftfix >= incont && irightfix >= incont) incont = 0;
  else if (irightfix >= ileftfix && irightfix >= incont)
  {
     ileftfix = incont;
     irightfix -= incont;
  }
  else if (irightfix > ileftfix)
  {
     ileftfix = irightfix;
     irightfix = 0;
  }
  else if (incont >= irightfix) irightfix = 0;
  knh = in - ileftfix - irightfix - incont;
  
  

  if ((scoef = newarray(in*kdim,sisl_double)) == SISL_NULL) goto err101;
  
  
  
  if (kn == in && incont == 0)
  {
     memcopy(scoef,pcurve->ecoef,in*kdim,sisl_double);
     
     
     
     if ((*rnewcurve = newCurve(in,ik,etau,scoef,kkind,kdim,kcopy)) == SISL_NULL)
	goto err101;
     (*rnewcurve)->cuopen = pcurve->cuopen;
     
     memzero(gmaxerr,kdim,sisl_double);
     memzero(gl2err,kdim,sisl_double);
  }     
  else
  {
     
     
     if ((sa = newarray((kn+in)*ik+incont*incont,sisl_double)) == SISL_NULL) goto err101;
     sb = sa + kn*ik;
     if ((lfirst = newarray(3*kn,sisl_int)) == SISL_NULL) goto err101;
     llast = lfirst + kn;
     l2sta = llast + kn;
     
     if (incont > 0)
     {
	sfac = sb + in*ik;
	memzero(sfac, incont*incont, sisl_double);
     }
     
     
     
     sh1922(pcurve->et,kn,ik,etau,in,sa,lfirst,llast,&kstat);
     if (kstat < 0) goto error;
		
     
     
     for (knormr=in, kj=kn-1; kj>=0; kj--)
     {
	if (llast[kj] < in-incont) break;
	knormr = MIN(knormr, lfirst[kj]);
     }
     knlr = kn - kj - 1;
     knormr = MIN(knh, in - knormr);
     
     
     if (knlr > 0)
     {
	
	
	if ((sa2 = new0array(knlr*(ik+incont)+knormr*in,sisl_double)) == SISL_NULL) 
	   goto err101;
	sw1 = sa2 + knlr*ik;
	sw2 = sw1 + knlr*incont;
	if ((lfirst2 = newarray(2*knlr,sisl_int)) == SISL_NULL) goto err101;
	llast2 = lfirst2 + knlr;
     
     
     
	memcopy(sa2, sa+(kn-knlr)*ik, knlr*ik, sisl_double);
	
	
	
	memcopy(lfirst2, lfirst+kn-knlr, knlr, sisl_int);
	memcopy(llast2, llast+kn-knlr, knlr, sisl_int);
     }
     
     if (incont > 0)
     {
	
	
	
	s1947(sa, lfirst, llast, ik, kn, etau, in, incont, sw1, 
	      knlr, &knred, sfac, &kstat);
	if (kstat < 0) goto error;
     }
     
     
		    
     if (ileftfix == 0 && irightfix == 0)
     {
	
	
	s1944(etau,ik,in-incont,kdim,pcurve->et,pcurve->ecoef,kn,incont,
	      knlr,knormr,sa,sw1,lfirst,llast,sb,sw2,l2sta,scoef,&kstat);
	if (kstat < 0) goto error;
      }
     else
     {
	
	
	if ((sc = newarray(kn*kdim,sisl_double)) == SISL_NULL) goto err101;
	memcopy(sc,pcurve->ecoef,kn*kdim,sisl_double);
     
	
	
	sh1927(etau,ik,in,kdim,pcurve,ileftfix,irightfix,scoef,&kstat);
	if (kstat < 0) goto error;
		       
        
		       
        if (knh > 0)
	{
	   
	   
	   s1946(sa,sw1,lfirst,llast,sc,scoef,ik,in,kn,kdim,ileftfix,
		  irightfix,knlr,incont,&kstat);
	   if (kstat < 0) goto error;
			  
           
			  
	   s1945(etau,ik,in,kdim,pcurve->et,sc,kn,ileftfix,irightfix,
		 incont,knlr,knormr,sa,sw1,knh,lfirst,llast,sb,sw2,scoef,
		 l2sta,&kstat);
	   if (kstat < 0) goto error;
        }
     }
     
     if (knh > 0)
     {
	
	
	s1948(sb,sw2,knh,ik,(knlr==knred)?0:knormr,l2sta,&kstat);
	if (kstat < 0) goto error;
		       
        
		       
        s1949(sb,sw2,scoef+ileftfix*kdim,knh,ik,(knlr==knred)?0:knormr,
	      kdim,l2sta,&kstat);
	if (kstat < 0) goto error;
      }
     
     
     
     s1951(etau, scoef, in, ik, kdim, ileftfix, irightfix, incont, sfac);

     
     
     if ((*rnewcurve = newCurve(in,ik,etau,scoef,kkind,kdim,kcopy)) == SISL_NULL)
	goto err101;
     (*rnewcurve)->cuopen = pcurve->cuopen;
     
     
     
     if (knlr > 0)
     {
	memcopy(sa+(kn-knlr)*ik, sa2, knlr*ik, sisl_double);
	memcopy(lfirst+kn-knlr, lfirst2, knlr, sisl_int);
	memcopy(llast+kn-knlr, llast2, knlr, sisl_int);
     }
     
     
     
     s1942(pcurve,*rnewcurve,kdim,sa,lfirst,llast,gmaxerr,gl2err,&kstat);
     if (kstat < 0) goto error;
  }
  
  

  *jstat = 0;
  goto out;
  
  

  err101 :
    *jstat = -101;
  goto out;
  
  

  err109 :
    *jstat = -109;
  goto out;
  
  

  error :
    *jstat = kstat;
  goto out;
  
  out :

    

    if (scoef != SISL_NULL) freearray(scoef);
    if (sc != SISL_NULL) freearray(sc);
    if (sa != SISL_NULL) freearray(sa);
    if (sa2 != SISL_NULL) freearray(sa2);
    if (lfirst != SISL_NULL) freearray(lfirst);
    if (lfirst2 != SISL_NULL) freearray(lfirst2);
  
    return;
}

