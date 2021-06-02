#include "pch.h"


#define S1950

#include "sislP.h"
 
#if defined(SISLNEEDPROTOTYPES)
void s1950(SISLCurve *oldcurve,SISLCurve *rankcurve,rank_info *ranking,
	   sisl_double eps[],sisl_double epsco[],int startfix,int endfix,int *jncont,
	   int mini,int maxi,SISLCurve **newcurve,sisl_double maxerr[],int *stat)
#else
void s1950(oldcurve, rankcurve, ranking, eps, epsco,
	   startfix, endfix, jncont, mini, maxi, newcurve, maxerr, stat)
     SISLCurve	*oldcurve;
     SISLCurve	*rankcurve;
     rank_info	*ranking;
     sisl_double	eps[];
     sisl_double	epsco[];
     int	startfix;
     int	endfix;
     int        *jncont;
     int	mini;
     int	maxi;
     SISLCurve	**newcurve;
     sisl_double	maxerr[];
     int	*stat;
#endif
{
  int k = oldcurve->ik;           
  int dim = oldcurve->idim;       
  int mprio = rankcurve->in;
  int *prio = ranking->prio;
  int *group = ranking->groups;
  int antrem = ranking->antrem;
  int antgr = ranking->antgr;
  char *del_array;                
  char big, bigco;                
  int lstat=0;			  
  int pos=0;			  
  int nlim = MAX(k, startfix+endfix); 

				
  int i, start, stop, indx, count, r, p, hn;
  int kncont = 0;
  SISLCurve *hcurve = SISL_NULL;
  sisl_double h;
  sisl_double *local_err = SISL_NULL, *l2_err = SISL_NULL, *ltau = SISL_NULL;
  sisl_double tend_domain = oldcurve->et[oldcurve->in];

  

  del_array = newarray(mprio, char);
  if (del_array == SISL_NULL) goto err101;
  if ((local_err = newarray(dim, sisl_double)) == SISL_NULL) goto err101;
  if ((l2_err = newarray(dim, sisl_double)) == SISL_NULL) goto err101;

  

  *newcurve = newCurve(mprio, k, rankcurve->et, rankcurve->ecoef, 1, dim, 1);
  if (newcurve == SISL_NULL) goto err101;

  

  while (mini+1 < maxi)
  {

    

    for (i=0; i<mprio; i++) del_array[i] = 0;
    
    
    
    kncont = 0;

    

    start = 0;
    stop = group[0];
    count = 0;

    

    while (stop <= antrem)
    {
      for (i=start; i<stop; i++) del_array[prio[i]] = 1;

      count++;

      if (count < antgr)
      {
	start = stop;
	stop = group[count];
      }
      else
      {

	

	stop = stop + 1;
	start = stop + 1;
      }
    }

    

    r = stop - start;
    p = antrem - start;

    if (p > 0)
    {
      h = (sisl_double) (r+1) / (sisl_double) p;
      for (i=0; i<p; i++)
      {
	indx = start - 1 + (int) floor( h*(i+0.5)+0.5 );
	del_array[prio[indx]] = 1;
      }
    }

    

    hn = mprio - antrem ;

    

    if (ltau != SISL_NULL) freearray(ltau);
    ltau = newarray(hn+2*k, sisl_double);
    if (ltau == SISL_NULL) goto err101;

    

    for (i=0; i<k; i++)
    {
      ltau[i] = rankcurve->et[i];
    }

    

    for (indx=k, i=0; i<mprio; i++)
    {
       if (!del_array[i]) ltau[indx++] = rankcurve->et[i+k];
       else if (rankcurve->et[i+k] == tend_domain)
       {
	  
	  
	  ltau[indx++] = tend_domain;
	  hn++;
	  kncont++;
       }
    }
    
    

    
    *jncont = MIN(kncont, k-1);
    s1943(oldcurve, ltau, k, hn, startfix, endfix, *jncont,
	   &hcurve, local_err, l2_err, &lstat);
    if (lstat < 0) goto err;

    
    big = 0;
    bigco = 0;

    for (i=0; i<dim; i++)
    {
      big = big || (local_err[i] > eps[i]);
      bigco = bigco || (local_err[i] > epsco[i]);
    }

    

    big = big || (bigco && hn < nlim);

    if (big)
    {

      

      if (hcurve != SISL_NULL) freeCurve(hcurve);
      hcurve = SISL_NULL;
      maxi = antrem;
    }
    else
    {

      

      mini = antrem;
      if (*newcurve != SISL_NULL) freeCurve(*newcurve);
      *newcurve = hcurve;
      hcurve = SISL_NULL;
      memcopy(maxerr, local_err, dim, sisl_double);
    }

    

    antrem = mini + (maxi-mini)/2;
  }

  *stat = 0;

  goto out;



  

err101:
  *stat = -101;
  goto out;

  

err:
  *stat = lstat;
  s6err("s1950", *stat, pos);

  

out:
  if (hcurve != SISL_NULL) freeCurve(hcurve);
  if (del_array != SISL_NULL) freearray(del_array);
  if (local_err != SISL_NULL) freearray(local_err);
  if (l2_err != SISL_NULL) freearray(l2_err);
  if (ltau != SISL_NULL) freearray(ltau);

  return;
}

