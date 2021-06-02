#include "pch.h"


#define SH1927

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
      sh1927(sisl_double etau[],int ik,int in,int idim,SISLCurve *pcurve,
	     int ilend,int irend,sisl_double ec[],int *jstat)
#else
void sh1927(etau,ik,in,idim,pcurve,ilend,irend,ec,jstat)
   sisl_double etau[];
   int ik;
   int in;
   int idim;
   SISLCurve *pcurve;
   int ilend;
   int irend;
   sisl_double ec[];
   int *jstat;
#endif     
{ 
   int kstat;
   int kn = xray::memory::uninitialized_value<int>(),km;
   int ki,kj,kr,kp;
   int krh,kjh,khc,kjhc;
   int kstart,kstop;
   int kleft = ik - 1;
   sisl_double tm;
   sisl_double th;
   sisl_double *sder=SISL_NULL;
   sisl_double *sbder=SISL_NULL;
   sisl_double *ssum = SISL_NULL;
   sisl_double *shc = SISL_NULL;

   

   if (pcurve->ik != ik) goto err106;
   if (pcurve->idim != idim) goto err103;
  
   
   
   if ((sder = newarray(idim*MAX(ilend,irend),sisl_double)) == SISL_NULL)
      goto err101;
   if ((sbder=newarray(ik*ik,sisl_double)) == SISL_NULL) goto err101;
   if ((ssum = newarray(idim,sisl_double)) == SISL_NULL) goto err101;
   if ((shc = newarray(MAX(1,irend)*idim,sisl_double)) == SISL_NULL) goto err101;
   
   
   
   if (ilend < 0) ilend = 0;
   if (ilend > ik) ilend = ik;
   if (irend < 0) irend = 0;
   if (irend > ik) irend = ik;
   
   
   
   memzero(ec,in*idim,sisl_double);
   
   
   
   if (ilend > 0)
   {
      
      
      s1221(pcurve,ilend-1,pcurve->et[ik-1],&kleft,sder,&kstat);
      if (kstat < 0) goto error;
      
      s1220(etau,ik,in,&kleft,etau[ik-1],ilend-1,sbder,&kstat);
      if (kstat < 0) goto error;

      
		     
      for (km=1; ; km++)
	if (km == ik || etau[ik-km-1] < etau[ik-km]) break;
      
      
      
      kn = ik - km;
	
      
      
      if (km < ik)
      {
	 for (kr=0; kr<kn-1; kr++)
	   for (ki=kr+1; ki<kn; ki++)
	     {
		tm = sbder[kr*ilend+ki]/sbder[kr*ilend+kr];
		for (kj=kr+1; kj<kn; kj++)
		  sbder[kj*ilend+ki] -= tm*sbder[kj*ilend+kr];
		for (kp=0; kp<idim; kp++)
		  sder[ki*idim+kp] -= tm*sder[kr*idim+kp];
	     }
	 
	 
	 
	 tm = (sisl_double)1.0/sbder[(kn-1)*ilend+kn-1];
	 for (kp=0; kp<idim; kp++)
	   ec[(kn-1)*idim+kp] = sder[(kn-1)*idim+kp]*tm;
	 for (kr=kn-2; kr>=0; kr--)
	   {
	      memzero(ssum,idim,sisl_double);
	      for (kj=kr+1; kj<kn; kj++)
		{
		   tm = sbder[kj*ilend+kr];
		   for (kp=0; kp<idim; kp++)
		     ssum[kp] += ec[kj*idim+kp]*tm;
		}
	      
	      tm = (sisl_double)1.0/sbder[kr*ilend+kr];
	      for (kp=0; kp<idim; kp++)
		ec[kr*idim+kp] = (sder[kr*idim+kp] - ssum[kp])*tm;
	   }
      }
      
      
      
      for (kr=kn; kr<ilend; kr++)
	{
	   memzero(ssum,idim,sisl_double);
	   for (kj=0; kj<kr; kj++)
	     {
		tm = sbder[kj*ilend+kr];
		for (kp=0; kp<idim; kp++)
		  ssum[kp] += ec[kj*idim+kp]*tm;
	     }
	   
	   tm = (sisl_double)1.0/sbder[kr*ilend+kr];
	   for (kp=0; kp<idim; kp++)
	     ec[kr*idim+kp] = (sder[kr*idim+kp] - ssum[kp])*tm;
	}
      
      
   }
      
   
   
   if (irend > 0)
   {
      
      
      kleft = pcurve->in;
      s1221(pcurve,irend-1,pcurve->et[pcurve->in],&kleft,sder,&kstat);
      if (kstat < 0) goto error;
		     
      kleft = in;		     
      s1220(etau,ik,in,&kleft,etau[in],irend-1,sbder,&kstat);
      if (kstat < 0) goto error;
				    
      
				    
      for (km=1; ; km++)
	if (km == ik || etau[in+km-1] < etau[in+km]) break;
      
      

      khc = idim*(irend-1);

      
      
      if (km < ik)
      {
	 
	 
	 for (krh=0, kstop=ik-km, kr=km+1; kr<ik-1; kr++)
	   for (krh++, ki=krh+1; ki<kstop; ki++)
	     {
		tm = sbder[kr*irend+ki]/sbder[kr*irend+krh];
		for (kj=kr+1; kj<ik; kj++)
		  sbder[kj*irend+ki] -= tm*sbder[kj*irend+krh];
		for (kp=0; kp<idim; kp++)
		  sder[ki*idim+kp] -= tm*sder[krh*idim+kp];
	     }
	 
	 
	 
	 krh = ik - km;
	 tm = (sisl_double)1.0/sbder[ik*irend+krh];
	 for (kp=0; kp<idim; khc++, kp++)
	   shc[khc] = sder[krh*idim+kp]*tm;
	 for (kstop=in-ik+km+1, kr=in-1; kr>=kstop; kr--)
	   {
	      khc -= 2*idim;
	      krh--;
	      memzero(ssum,idim,sisl_double);
	      for (kjhc=khc+idim, kjh=km+krh, kj=kr+1; kj<kn; kj++)
		{
		   tm = sbder[kjh*irend+krh];
		   for (kp=0; kp<idim; kjhc++, kp++)
		     ssum[kp] += shc[kjhc]*tm;
		}
	      
	      tm = (sisl_double)1.0/sbder[(krh+km)*irend+krh];
	      for (kp=0; kp<idim; khc++, kp++)
		shc[khc] = (sder[krh*idim+kp] - ssum[kp])*tm;
	   }
	 khc -= 2*idim;
      }


   
   
   for (krh=ik-km, kstart=in-ik+km-1, kstop=in-irend, kr=kstart;
    kr>=kstop; krh++, kr--)
     {
	memzero(ssum,idim,sisl_double);
	for (kjhc=khc+idim, kjh=ik-krh, kj=kr+1; kj<in; kjh++, kj++)
	  {
	     tm = sbder[kjh*irend+krh];
	     for (kp=0; kp<idim; kjhc++, kp++)
	       ssum[kp] += shc[kjhc]*tm;
	  }
	
	tm = (sisl_double)1.0/sbder[(ik-krh-1)*irend+krh];
	for (kp=0; kp<idim; khc++, kp++)
	  shc[khc] = (sder[krh*idim+kp] - ssum[kp])*tm;
	khc -= 2*idim;
     }
   
   
   
   for (khc=0, ki=in-irend; ki<in; ki++)
     for (kp=0; kp<idim; khc++, kp++)
       {
	  if (ki < ilend) th = ec[ki*idim+kp];
	  else th = shc[khc];
	  ec[ki*idim+kp] = (th + shc[khc])/(sisl_double)2.0;
       }
   }
   
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;

   
   
   err103: *jstat = -103;
   goto out;
   
   
   
   err106: *jstat = -106;
   goto out;
   
   
   
   error: *jstat = kstat;
   goto out;
   
   out:
      
      
      if (sder != SISL_NULL) freearray(sder);
      if (sbder != SISL_NULL) freearray(sbder);
      if (ssum != SISL_NULL) freearray(ssum);
      if (shc != SISL_NULL) freearray(shc);

      return;
}
   

