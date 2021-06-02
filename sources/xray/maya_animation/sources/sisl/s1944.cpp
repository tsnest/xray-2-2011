#include "pch.h"


#define S1944

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void
   s1944(sisl_double etau[],int ik,int in,int idim,sisl_double et[],sisl_double ed[],
	     int im,int inlc,int inlr,int inorm,sisl_double ea[],sisl_double ew1[],
	     int nfirst[],int nlast[],sisl_double eb[],sisl_double ew2[],
	     int n2sta[],sisl_double ec[],int *jstat)
#else
void s1944(etau,ik,in,idim,et,ed,im,inlc,inlr,inorm,ea,ew1,nfirst,nlast,eb,ew2,
	   n2sta,ec,jstat)
   sisl_double etau[];
   int ik;
   int in;
   int idim;
   sisl_double et[];
   sisl_double ed[];
   int im;
   int inlc;
   int inlr;
   int inorm;
   sisl_double ea[];
   sisl_double ew1[];
   int nfirst[];
   int nlast[];
   sisl_double eb[];
   sisl_double ew2[];
   int n2sta[];
   sisl_double ec[];
   int *jstat;
#endif     
{ 
  int ki,kj,kr;
  int kj2;
  int ki1,ki2,kr1,kj1;
  int kik,kih,krh;
  sisl_double tw;
  sisl_double thelp;
  sisl_double *swa = SISL_NULL;
  
  
  
  if ((swa = newarray(in,sisl_double)) == SISL_NULL)goto err101;
  
  
  
  for (kj=0; kj<in; kj++) n2sta[kj] = -1;
  memzero(ec,in*idim,sisl_double);
  memzero(eb,in*ik,sisl_double);
  if (inorm > 0)
     memzero(ew2,in*inorm,sisl_double);
  
  
  
  for (kj=0; kj<im; kj++)
  {
     ki1 = nfirst[kj];
     ki2 = nlast[kj];
     kr1 = ik - ki2 + ki1 - 1;
     tw = (et[kj+ik] - et[kj])/(sisl_double)ik;
     kik = ik - 1;
     for (kr = kr1; kr<ik; ki1++, kik--, kr++)
     {
	if (n2sta[ki1] == -1) n2sta[ki1] = kik;
	kih = ik - ki2 + ki1 - 1;
	krh = ik - 1;
	thelp = tw*ea[kj*ik+kr];
	for (ki=ki1; ki<=ki2; kih++, krh--, ki++)
	   eb[ki*ik+krh] += ea[kj*ik+kih]*thelp;
	kih = kr - ik + ki2 + 1;
	for (ki=0; ki<idim; ki++)
	   ec[kih*idim+ki] += ed[kj*idim+ki]*thelp;
     }
  }
  
  
  
  for (kj=im-inlr, kj2=0; kj<im; kj++, kj2++)
  {
     ki1 = nfirst[kj];
     ki2 = nlast[kj];
     tw = (et[kj+ik] - et[kj])/(sisl_double)ik; 
     for (kr=0; kr<inlc; kr++)
     {
	kih = ik - ki2 + ki1 - 1;
	krh = ik-1;
	thelp = tw*ew1[kj2*inlc+kr];
	for (ki=kr; ki<inlc; ki++, krh--)
	{
	   eb[ki*ik+krh] += ew1[kj2*inlc+ki]*thelp;
	   if (krh < n2sta[ki]) n2sta[ki] = krh;
	}
	for (ki=0; ki<idim; ki++)
	   ec[kr*idim+ki] += ed[kj*idim+ki]*thelp;
	for (ki=ki1; ki<=ki2; kih++, krh--, ki++)
	   ew2[(ki-in+inorm)*in+kr] += ea[kj*ik+kih]*thelp;
     }
  }
  
  
  
  for (ki=0; ki<in; ki++)
    swa[ki] = sqrt((sisl_double)ik/(etau[ki+ik]-etau[ki]));
  for (ki=0; ki<in; ki++)
    {
       thelp = swa[ki];
       for (kj=0; kj<idim; kj++)
	 ec[ki*idim+kj] *= thelp;
       for (kj1=n2sta[ki], kih=kj1-ik+ki+1, kj=kj1;
	kj<ik; kih++, kj++)
	 eb[ki*ik+kj] *= swa[kih]*thelp;
    }
  for (ki=0; ki<inorm; ki++)
     for (kj=0; kj<inlc; kj++)
	ew2[ki*in+kj] *= swa[in-inorm+ki]*swa[kj]; 
  
  
  
  for (ki2=0, ki=in-inorm; ki<in; ki2++, ki++)
  {
     for (kj=n2sta[ki]; kj<ik; kj++)
	ew2[ki2*in+ki-ik+kj+1] += eb[ki*ik+kj];
  }

  
  
   *jstat = 0;
   goto out;
   
   
   
   err101: *jstat = -101;
   goto out;
   
   out:
      
      
      if (swa != SISL_NULL) freearray(swa);
	  
      return;
}
   

