#include "pch.h"


#define S1353

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void s1353(SISLCurve *curve,sisl_double eps[],rank_info *ranking,int *stat)
#else
void s1353(curve, eps, ranking, stat)
     SISLCurve	*curve;
     sisl_double	eps[];
     rank_info	*ranking;
     int *stat;
#endif
{
  int k = curve->ik;            
  int dim = curve->idim;        
  int n = curve->in;            
  sisl_double *et = curve->et;       
  sisl_double *ecoef = curve->ecoef;
  int *prio = ranking->prio;    
  int *groups = ranking->groups;
  int antgr;                    
  int antrem;
  int pos=0;                    
                                
  int i, ii, i2, inxt, mult, dm, j1, j2, nkt, dih, dj, dj1, hmult,
      p, dp, di2, j, count, di, iw, ph, gh;
  sisl_double h, x, pm1h, pm1, muj1, muj, mm, h1, h2, denom, ln2inv;
  sisl_double *c=0, *hh=0, *mu=0, *w=0, *e=0, *s=0;

  

  

  c = newarray(dim*(k+1), sisl_double);
  if (c == SISL_NULL) goto err101;

  

  hh = newarray(k+1, sisl_double);
  if (hh == SISL_NULL) goto err101;

  

  mu = newarray(k+1, sisl_double);
  if (mu == SISL_NULL) goto err101;

  

  w = newarray(dim*(n-k), sisl_double);
  if (w == SISL_NULL) goto err101;

  

  e = newarray(dim, sisl_double);
  if (e == SISL_NULL) goto err101;

  s = newarray(dim, sisl_double);
  if (s == SISL_NULL) goto err101;

  

  

  i = k - 1; dih = 0; nkt = n - k;

  while (i < n-1)
    {

      

      inxt = i + 1; x = et[inxt];
      while (x == et[inxt+1]) ++inxt;

      

      mult = inxt - i; dm = k - mult + 2;

      

      

	       pm1h = 1.0;
      if ( fmod((float) dm, float(2.0)) == 0 )  pm1h = -1.0;

      

      muj1 = 1.0; mu[0] = 1.0;
      j1 = inxt - k + 1; dj1 = dim*j1;
      j2 = inxt + 1;
      hh[0] = pm1 = pm1h;

      di2 = (j1-1)*dim;

      for (ii = 0; ii < dim; ++ii, ++di2) c[ii] = ecoef[di2];

      j = 1; dj = dim;
      i2 = 0; di2 = 0;

      

      while (muj1 <= 2 && j < dm - 1)
	{
	  h = et[j1];

	  

	  if (h == et[j2] || x == h) goto err112;
	  muj = (x-h) / (et[j2]-h);
	  mm = (1-muj) * muj1;
	  mu[j] = muj1 = 1 / muj;
	  pm1 = -pm1;
	  hh[j] = pm1 - mm*hh[i2];
	  for (ii=0; ii<dim; ++ii, ++dj, ++dj1, ++di2)
	    c[dj] = ecoef[dj1] - mm*c[di2];

	  i2 = j; ++j; ++j1; ++j2;
	}

      

      p = i2;

      

      j1 = i; dj1 = dim*(j1+1)-1;
      j2 = j1 + k;
      i2 = j1 + 1; di2 = dim*(i2+1) - 1;
      pm1 = 1;
      hh[dm-1] = 1.0;
      dj = dim*dm - 1;

      for (ii=0; ii<dim; ++ii, --dj, --di2) c[dj] = ecoef[di2];

      muj1 = mu[dm-1] = 1.0;
      i2 = dm - 1; di2 = dim*dm - 1;

      

      for (j=dm-2; j>p; --j)
	{
	  h = et[j2];
	  if (h == x || h == et[j1]) goto err112;
	  muj = (h-x) / (h-et[j1]);
	  mm = (1-muj)*muj1;
	  muj1 = 1/muj;
	  mu[j] = muj1;
	  pm1 = -pm1;
	  hh[j] = pm1 - mm*hh[i2];
	  for (ii=0; ii<dim; ++ii, --dj, --dj1, --di2)
	    c[dj] = ecoef[dj1] - mm*c[di2];

	  i2 = j; --j1; --j2;
	}

      

      h1 = mu[i2]; h2 = mu[p];
      denom = 1/(h1*hh[i2]-h2*hh[p]);
      dp = dim*p; di2 = dim*i2;
      for (ii=0; ii<dim; ++ii, ++di2, ++dp, ++dih)
	{
	  h = e[ii] = (h1*c[di2] - h2*c[dp])*denom;
	  w[dih] = fabs(h);
	}

      

      count = 1;

      for (hmult=mult-1; hmult>0; --hmult)
	{

	  

	  

	  i2 = i + count; dj = dim*(p+1);
	  for (j=p+1; j<dm; ++j)
	    {
	      h1 = hh[j]; h2 = mu[j];
	      for (ii=0; ii<dim; ++ii, ++dj)
		c[dj] = (c[dj] - h1*e[ii])*h2;
	    }

	  

	  dj1 = dim*(p+1) - 1; dj = dj1 - dim;
	  for (j=p-1; j>=0; --j)
	    {
	      h1 = hh[j]; h2 = mu[j];
	      for (ii=dim-1; ii>-1; --ii, --dj, --dj1)
		{
		  c[dj1] = (c[dj] - h1*e[ii])*h2;
		}
	    }

	  

	  
	  ++count; ++dm;
	  pm1h = -pm1h;
	  pm1 = hh[0] = pm1h;
	  j1 = i + hmult - k + 1;
	  j2 = inxt + 1;
	  i2 = j1 - 1; di2 = dim*i2;
	  muj1 = 1.0;

	  for (ii=0; ii<dim; ++ii, ++di2) c[ii] = ecoef[di2];


	  j = 1; dj = dim;
	  i2 = 0; di2 = 0;

	  

	  while (muj1 <= 2 && j < dm - 1)
	    {
	      h = et[j1];
	      if (h == et[j2] || x == h) goto err112;
	      muj = (x-h) / (et[j2]-h);
	      mm = (1-muj) * muj1;
	      mu[j] = muj1 = 1 / muj;
	      pm1 = -pm1;
	      hh[j] = pm1 - mm*hh[i2];
	      for (ii=0; ii<dim; ++ii, ++dj, ++di2)
		c[dj] = c[dj] - mm*c[di2];

	      i2 = j; ++j; ++j1; ++j2;

	    }

	  

	  p = i2;

	  

	  j1 = i; dj1 = dim*(j1+1) - 1;
	  j2 = j1 + k + count - 1;
	  i2 = i + count; di2 = dim*(i2+1) - 1;
	  pm1 = 1;
	  hh[dm-1] = 1.0;
	  dj = dim*dm - 1;

	  for (ii=0; ii<dim; ++ii, --dj, --di2) c[dj] = ecoef[di2];


	  muj1 = mu[dm-1] = 1.0;
	  i2 = dm - 1; di2 = dim*dm - 1;

	  
	  for (j=dm-2; j>p; --j)
	    {
	      h = et[j2];
	      if (h == x || h == et[j1]) goto err112;
	      muj = (h-x) / (h-et[j1]);
	      mm = (1-muj)*muj1;
	      muj1 = 1/muj;
	      mu[j] = muj1;
	      pm1 = -pm1;
	      hh[j] = pm1 - mm*hh[i2];
	      for (ii=0; ii<dim; ++ii, --dj, --di2)
		c[dj] = c[dj] - mm*c[di2];

	      i2 = j; --j1; --j2;
	    }

	  

	  h1 = mu[i2]; h2 = mu[p];
	  denom = 1/(h1*hh[i2]-h2*hh[p]);
	  dp = dim*p; di2 = dim*i2;
	  for (ii=0; ii<dim; ++ii, ++di2, ++dp, ++dih)
	    {
	      h = e[ii] = (h1*c[di2] - h2*c[dp])*denom;
	      w[dih] = w[dih-dim] + fabs(h);
	    }
	}

      

      i = inxt;

    }

  

  

  for (ii=0; ii<dim; ++ii)
    s[ii] = sisl_double ( 2.0 )/MAX( eps[ii], sisl_double ( MIN_VALUE ) );

  

  ln2inv = sisl_double ( 1.0 ) / log( sisl_double ( 2.0 ));
  di = 0;

  

  for (i=0; i<nkt; ++i)
    {
      iw = 0;
      for (ii=0; ii<dim; ++ii, ++di)
	{
	  h = w[di];
	  if (h > 0.5*eps[ii])
	    iw = MAX(iw, (int)floor(log(h*s[ii])*ln2inv+1));
	}
      groups[i] = iw;
    }

  

  for (i=0; i<nkt; ++i) prio[i]=i;

  for (i=1; i<nkt; ++i)
    {
      gh = groups[i]; ph = prio[i];

      for (j=i-1; gh < groups[j];)
	{
	  groups[j+1] = groups[j];
	  prio[j+1] = prio[j];
	  --j;
	  if (j <0) goto out;
	}

    out:
      groups[j+1] = gh; prio[j+1] = ph;
    }

  

  if (groups[0] == 0) antrem = 0;
    else
      antrem = -1;

  

  j = 0;
  for (i=0; i<nkt-1; ++i)
    if (groups[i] < groups[i+1])
      {
	groups[j] = i+1; ++j;
      }

  antgr = ++j;
  groups[antgr-1] = n - k;

  

  if (antrem == 0)
    {
      if (antgr > 1)
	antrem = int(0.75*groups[1]/(antgr-1));
      else
	antrem = int(0.9*groups[0]);
    }
  else
    antrem = 1;

  antrem = MAX(MIN(1, nkt), antrem);

  

  ranking->antgr = antgr;
  ranking->antrem = antrem;



  

  freearray(hh);
  freearray(c);
  freearray(mu);
  freearray(w);
  freearray(e);
  freearray(s);

  

  return;

  

 err101:
  *stat = -101;
  s6err("s1353", *stat, pos);
  goto err;

  

 err112:
  *stat = -112;
  s6err("s1353", *stat, pos);
  goto err;

  

 err:
  if (c != SISL_NULL) freearray(c);
  if (hh != SISL_NULL) freearray(hh);
  if (mu != SISL_NULL) freearray(mu);
  if (w != SISL_NULL) freearray(w);
  if (e != SISL_NULL) freearray(e);
  if (s != SISL_NULL) freearray(s);

  return;

}

