#include "pch.h"

#define S1945

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1945(sisl_double etau[],int ik,int in,int idim,sisl_double et[],
	     sisl_double ed[],int im,int ilend,int irend, int inlc,
	     int inlr, int inorm, sisl_double ea[], sisl_double ew1[], int inh,
	     int nfirst[],int nlast[],sisl_double eb[], 
	     sisl_double ew2[], sisl_double ec[],int n2sta[],int *jstat)
#else
void s1945(etau,ik,in,idim,et,ed,im,ilend,irend,inlc,inlr,inorm,ea,ew1,inh,
	   nfirst,nlast,eb,ew2,ec,n2sta,jstat)
   sisl_double etau[];
   int ik;
   int in;
   int idim;
   sisl_double et[];
   sisl_double ed[];
   int im;
   int ilend;
   int irend;
   int inlc;
   int inlr;
   int inorm;
   sisl_double ea[];
   sisl_double ew1[];
   int inh;
   int nfirst[];
   int nlast[];
   sisl_double eb[];
   sisl_double ew2[];
   sisl_double ec[];
   int n2sta[];
   int *jstat;
#endif     
{ 
  int ki,kj,kr;
  int ki1,ki2,kr1,kj1,kj2;
  int kik,kih,krh,kihh,krhh;
  sisl_double tw;
  sisl_double thelp;
  sisl_double *swa = SISL_NULL;
  
  
  
  if ((swa = newarray(in,sisl_double)) == SISL_NULL)goto err101;
  
  
  
  for (kj=0; kj<inh; kj++) n2sta[kj] = -1;
  memzero(ec+ilend*idim,inh*idim,sisl_double);
  memzero(eb,inh*ik,sisl_double);
  if (inorm > 0)
     memzero(ew2,inh*inorm,sisl_double);
  
  
  
  for (kj=0; kj<im; kj++)
    {
       ki1 = MAX(nfirst[kj],ilend);
       ki2 = MIN(nlast[kj],in-irend-1);
       kr1 = ik - ki2 + ki1 - 1;
       tw = (et[kj+ik] - et[kj])/(sisl_double)ik;
       kik = ik - 1;
       krhh = ik - nlast[kj] + ki1 - 1;
       for (kr = kr1; kr<ik; ki1++, kik--, krhh++, kr++)
	 {
	    kihh = ki1 - ilend;
	    if (n2sta[kihh] == -1) n2sta[kihh] = kik;
	    kih = ik - nlast[kj] + ki1 - 1;
	    krh = ik - 1;
	    thelp = tw*ea[kj*ik+krhh];
	    for (ki=ki1; ki<=ki2; kih++, kihh++, krh--, ki++)
	      eb[kihh*ik+krh] += ea[kj*ik+kih]*thelp;
	    kih = kr - ik + ki2 + 1;
	    for (ki=0; ki<idim; ki++)
	      ec[kih*idim+ki] += ed[kj*idim+ki]*thelp;
	 }
    }
  
  
  
  for (kj=im-inlr, kj2=0; kj<im; kj++, kj2++)
  {
     ki1 = MAX(nfirst[kj],ilend);
     ki2 = MIN(nlast[kj],in-irend-1);
     tw = (et[kj+ik] - et[kj])/(sisl_double)ik; 
     for (kr=ilend; kr<inlc; kr++)
     {
	kih = ik - ki2 + ki1 - 1;
	krh = ik-1;
	thelp = tw*ew1[kj2*inlc+kr];
	for (ki=kr; ki<inlc; ki++, krh--)
	{
	   eb[(ki-ilend)*ik+krh] += ew1[kj2*inlc+ki]*thelp;
	   if (krh < n2sta[ki-ilend]) n2sta[ki-ilend] = krh;
	}
	for (ki=0; ki<idim; ki++)
	   ec[kr*idim+ki] += ed[kj*idim+ki]*thelp;
	for (ki=ki1; ki<=ki2; kih++, krh--, ki++)
	   ew2[(ki-ilend-inh+inorm)*inh+kr-ilend] += ea[kj*ik+kih]*thelp;
     }
  }
  
  
  
  for (kihh=ilend, ki=0; ki<inh; kihh++, ki++)
       swa[ki] = sqrt((sisl_double)ik/(etau[kihh+ik]-etau[kihh]));

  for (kihh=ilend, ki=0; ki<inh; kihh++, kih++, ki++)
    {
       thelp = swa[ki];
       for (kj=0; kj<idim; kj++)
	 ec[kihh*idim+kj] *= thelp;
       for (kj1=n2sta[ki], kih=kj1-ik+ki+1, kj=kj1;
	kj<ik; kih++, kj++)
	 eb[ki*ik+kj] *= swa[kih]*thelp;
    }
  for (ki=0; ki<inorm; ki++)
     for (kj=ilend; kj<inlc; kj++)
	ew2[ki*inh+kj-ilend] *= 
	   swa[inh-inorm+ki]*swa[kj-ilend]; 
   

  
  
  for (ki2=0, ki=inh-inorm; ki<inh; ki2++, ki++)
  {
     for (kj=n2sta[ki]; kj<ik; kj++)
	ew2[ki2*inh+ki-ik+kj+1] += eb[ki*ik+kj];
  }
  
  
  
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   out:
      
      
      if (swa != SISL_NULL) freearray(swa);
	  
      return;
}
   

