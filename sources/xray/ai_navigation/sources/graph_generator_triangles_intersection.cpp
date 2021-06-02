////////////////////////////////////////////////////////////////////////////
//	Created		: 06.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR

#include "graph_generator.h"
#include "navigation_mesh_types.h"

using xray::ai::navigation::graph_generator;
using xray::math::float3;
using xray::math::float2;
using namespace xray::ai::navigation;

////////////////////////////////////////////////////////////////////////////
//	http://www.cs.lth.se/home/Tomas_Akenine_Moller/code/tritri_isectline.txt
//	http://knight.temple.edu/~lakaemper/courses/cis350_2004/etc/moeller_triangle.pdf
////////////////////////////////////////////////////////////////////////////

#define FABS(x) (xray::math::abs(x))        /* implement as is fastest on your machine */

/* if USE_EPSILON_TEST is true then we do a check: 
         if |dv|<EPSILON then dv=0.0;
   else no check is done (which is less robust)
*/
#define USE_EPSILON_TEST TRUE
#define EPSILON 0.001


/* some macros */
#define CROSS(dest,v1,v2)	(dest) = (v1) ^ (v2)
#define DOT(v1,v2)			((v1) | (v2))
#define SUB(dest,v1,v2)		(dest) = ((v1) - (v2))
#define ADD(dest,v1,v2)		(dest) = ((v1) + (v2))
#define MULT(dest,v,factor) (dest) = (v)*(factor)
#define SET(dest,src)		(dest) = (src)

/* sort so that a<=b */
#define SORT(a,b)       \
             if(a>b)    \
             {          \
               float c; \
               c=a;     \
               a=b;     \
               b=c;     \
             }

#define ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,isect1) \
              isect0=VV0+(VV1-VV0)*D0/(D0-D1);    \
              isect1=VV0+(VV2-VV0)*D0/(D0-D2);


#define COMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,isect0,isect1) \
  if(D0D1>0.0f)                                         \
  {                                                     \
    /* here we know that D0D2<=0.0 */                   \
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else if(D0D2>0.0f)                                    \
  {                                                     \
    /* here we know that d0d1<=0.0 */                   \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D1*D2>0.0f || D0!=0.0f)                       \
  {                                                     \
    /* here we know that d0d1<=0.0 or that D0!=0.0 */   \
    ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,isect1);          \
  }                                                     \
  else if(D1!=0.0f)                                     \
  {                                                     \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D2!=0.0f)                                     \
  {                                                     \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else                                                  \
  {                                                     \
    /* triangles are coplanar */                        \
    return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);      \
  }



/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */ 
#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return 1;                      \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return 1;                      \
    }                                                 \
  }                                

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return 1;                 \
  }                                         \
}

int coplanar_tri_tri(float3 const& N, float3 const& V0, float3 const& V1, float3 const& V2,
                     float3 const& U0, float3 const& U1, float3 const& U2)
{
   float3 A;
   short i0,i1;
   /* first project onto an axis-aligned plane, that maximizes the area */
   /* of the triangles, compute indices: i0,i1. */
   A[0]=fabs(N[0]);
   A[1]=fabs(N[1]);
   A[2]=fabs(N[2]);
   if(A[0]>A[1])
   {
      if(A[0]>A[2])  
      {
          i0=1;      /* A[0] is greatest */
          i1=2;
      }
      else
      {
          i0=0;      /* A[2] is greatest */
          i1=1;
      }
   }
   else   /* A[0]<=A[1] */
   {
      if(A[2]>A[1])
      {
          i0=0;      /* A[2] is greatest */
          i1=1;                                           
      }
      else
      {
          i0=0;      /* A[1] is greatest */
          i1=2;
      }
    }               
                
    /* test all edges of triangle 1 against the edges of triangle 2 */
    EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
    EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2);
                
    /* finally, test if tri1 is totally contained in tri2 or vice versa */
    POINT_IN_TRI(V0,U0,U1,U2);
    POINT_IN_TRI(U0,V0,V1,V2);

    return 0;
}


int tri_tri_intersect(float3 const& V0, float3 const& V1, float3 const& V2,
                      float3 const& U0, float3 const& U1, float3 const& U2)
{
  float3 E1,E2;
  float3 N1,N2;
  float d1,d2;
  float du0,du1,du2,dv0,dv1,dv2;
  float3 D;
  float2 isect1, isect2;
  float du0du1,du0du2,dv0dv1,dv0dv2;
  short index;
  float vp0,vp1,vp2;
  float up0,up1,up2;
  float b,c,max;

  /* compute plane equation of triangle(V0,V1,V2) */
  SUB(E1,V1,V0);
  SUB(E2,V2,V0);
  CROSS(N1,E1,E2);
  d1=-DOT(N1,V0);
  /* plane equation 1: N1.X+d1=0 */

  /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
  du0=DOT(N1,U0)+d1;
  du1=DOT(N1,U1)+d1;
  du2=DOT(N1,U2)+d1;

  /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
  if(fabs(du0)<EPSILON) du0=0.0;
  if(fabs(du1)<EPSILON) du1=0.0;
  if(fabs(du2)<EPSILON) du2=0.0;
#endif
  du0du1=du0*du1;
  du0du2=du0*du2;

  if(du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute plane of triangle (U0,U1,U2) */
  SUB(E1,U1,U0);
  SUB(E2,U2,U0);
  CROSS(N2,E1,E2);
  d2=-DOT(N2,U0);
  /* plane equation 2: N2.X+d2=0 */

  /* put V0,V1,V2 into plane equation 2 */
  dv0=DOT(N2,V0)+d2;
  dv1=DOT(N2,V1)+d2;
  dv2=DOT(N2,V2)+d2;

#if USE_EPSILON_TEST==TRUE
  if(fabs(dv0)<EPSILON) dv0=0.0;
  if(fabs(dv1)<EPSILON) dv1=0.0;
  if(fabs(dv2)<EPSILON) dv2=0.0;
#endif

  dv0dv1=dv0*dv1;
  dv0dv2=dv0*dv2;
        
  if(dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute direction of intersection line */
  CROSS(D,N1,N2);

  /* compute and index to the largest component of D */
  max=fabs(D[0]);
  index=0;
  b=fabs(D[1]);
  c=fabs(D[2]);
  if(b>max) max=b,index=1;
  if(c>max) max=c,index=2;

  /* this is the simplified projection onto L*/
  vp0=V0[index];
  vp1=V1[index];
  vp2=V2[index];
  
  up0=U0[index];
  up1=U1[index];
  up2=U2[index];

  /* compute interval for triangle 1 */
  COMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,isect1[0],isect1[1]);

  /* compute interval for triangle 2 */
  COMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,isect2[0],isect2[1]);

  SORT(isect1[0],isect1[1]);
  SORT(isect2[0],isect2[1]);

  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;
  return 1;
}


#define NEWCOMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,A,B,C,X0,X1) \
{ \
        if(D0D1>0.0f) \
        { \
                /* here we know that D0D2<=0.0 */ \
            /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
        } \
        else if(D0D2>0.0f)\
        { \
                /* here we know that d0d1<=0.0 */ \
            A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
        } \
        else if(D1*D2>0.0f || D0!=0.0f) \
        { \
                /* here we know that d0d1<=0.0 or that D0!=0.0 */ \
                A=VV0; B=(VV1-VV0)*D0; C=(VV2-VV0)*D0; X0=D0-D1; X1=D0-D2; \
        } \
        else if(D1!=0.0f) \
        { \
                A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
        } \
        else if(D2!=0.0f) \
        { \
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
        } \
        else \
        { \
                /* triangles are coplanar */ \
                return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2); \
        } \
}



int NoDivTriTriIsect(float3 const& V0, float3 const& V1, float3 const& V2,
                     float3 const& U0, float3 const& U1, float3 const& U2)
{
  float3 E1,E2;
  float3 N1,N2;
  float d1,d2;
  float du0,du1,du2,dv0,dv1,dv2;
  float3 D;
  float2 isect1, isect2;
  float du0du1,du0du2,dv0dv1,dv0dv2;
  short index;
  float vp0,vp1,vp2;
  float up0,up1,up2;
  float bb,cc,max;
  float a,b,c,x0,x1;
  float d,e,f,y0,y1;
  float xx,yy,xxyy,tmp;

  /* compute plane equation of triangle(V0,V1,V2) */
  SUB(E1,V1,V0);
  SUB(E2,V2,V0);
  CROSS(N1,E1,E2);
  d1=-DOT(N1,V0);
  /* plane equation 1: N1.X+d1=0 */

  /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
  du0=DOT(N1,U0)+d1;
  du1=DOT(N1,U1)+d1;
  du2=DOT(N1,U2)+d1;

  /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
  if(FABS(du0)<EPSILON) du0=0.0;
  if(FABS(du1)<EPSILON) du1=0.0;
  if(FABS(du2)<EPSILON) du2=0.0;
#endif
  du0du1=du0*du1;
  du0du2=du0*du2;

  if(du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute plane of triangle (U0,U1,U2) */
  SUB(E1,U1,U0);
  SUB(E2,U2,U0);
  CROSS(N2,E1,E2);
  d2=-DOT(N2,U0);
  /* plane equation 2: N2.X+d2=0 */

  /* put V0,V1,V2 into plane equation 2 */
  dv0=DOT(N2,V0)+d2;
  dv1=DOT(N2,V1)+d2;
  dv2=DOT(N2,V2)+d2;

#if USE_EPSILON_TEST==TRUE
  if(FABS(dv0)<EPSILON) dv0=0.0;
  if(FABS(dv1)<EPSILON) dv1=0.0;
  if(FABS(dv2)<EPSILON) dv2=0.0;
#endif

  dv0dv1=dv0*dv1;
  dv0dv2=dv0*dv2;

  if(dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute direction of intersection line */
  CROSS(D,N1,N2);

  /* compute and index to the largest component of D */
  max=(float)FABS(D[0]);
  index=0;
  bb=(float)FABS(D[1]);
  cc=(float)FABS(D[2]);
  if(bb>max) max=bb,index=1;
  if(cc>max) max=cc,index=2;

  /* this is the simplified projection onto L*/
  vp0=V0[index];
  vp1=V1[index];
  vp2=V2[index];

  up0=U0[index];
  up1=U1[index];
  up2=U2[index];

  /* compute interval for triangle 1 */
  NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1);

  /* compute interval for triangle 2 */
  NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,d,e,f,y0,y1);

  xx=x0*x1;
  yy=y0*y1;
  xxyy=xx*yy;

  tmp=a*xxyy;
  isect1[0]=tmp+b*x1*yy;
  isect1[1]=tmp+c*x0*yy;

  tmp=d*xxyy;
  isect2[0]=tmp+e*xx*y1;
  isect2[1]=tmp+f*xx*y0;

  SORT(isect1[0],isect1[1]);
  SORT(isect2[0],isect2[1]);

  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;
  return 1;
}

/* sort so that a<=b */
#define SORT2(a,b,smallest)       \
             if(a>b)       \
             {             \
               float c;    \
               c=a;        \
               a=b;        \
               b=c;        \
               smallest=1; \
             }             \
             else smallest=0;


inline void isect2(float3 const& VTX0, float3 const& VTX1, float3 const& VTX2, float VV0, float VV1, float VV2,
	    float D0, float D1, float D2, float *isect0, float *isect1, float3& isectpoint0,float3& isectpoint1) 
{
  float tmp=D0/(D0-D1);          
  float3 diff;
  *isect0=VV0+(VV1-VV0)*tmp;         
  SUB(diff,VTX1,VTX0);              
  MULT(diff,diff,tmp);               
  ADD(isectpoint0,diff,VTX0);        
  tmp=D0/(D0-D2);                    
  *isect1=VV0+(VV2-VV0)*tmp;          
  SUB(diff,VTX2,VTX0);                   
  MULT(diff,diff,tmp);                 
  ADD(isectpoint1,VTX0,diff);          
}


#if 0
#define ISECT2(VTX0,VTX1,VTX2,VV0,VV1,VV2,D0,D1,D2,isect0,isect1,isectpoint0,isectpoint1) \
              tmp=D0/(D0-D1);                    \
              isect0=VV0+(VV1-VV0)*tmp;          \
	      SUB(diff,VTX1,VTX0);               \
	      MULT(diff,diff,tmp);               \
              ADD(isectpoint0,diff,VTX0);        \
              tmp=D0/(D0-D2);                    
/*              isect1=VV0+(VV2-VV0)*tmp;          \ */
/*              SUB(diff,VTX2,VTX0);               \     */
/*              MULT(diff,diff,tmp);               \   */
/*              ADD(isectpoint1,VTX0,diff);           */
#endif

inline int compute_intervals_isectline(float3 const& VERT0, float3 const& VERT1, float3 const& VERT2,
				       float VV0,float VV1,float VV2,float D0,float D1,float D2,
				       float D0D1,float D0D2,float *isect0,float *isect1,
				       float3& isectpoint0, float3& isectpoint1)
{
  if(D0D1>0.0f)                                        
  {                                                    
    /* here we know that D0D2<=0.0 */                  
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);
  } 
  else if(D0D2>0.0f)                                   
    {                                                   
    /* here we know that d0d1<=0.0 */             
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1);
  }                                                  
  else if(D1*D2>0.0f || D0!=0.0f)   
  {                                   
    /* here we know that d0d1<=0.0 or that D0!=0.0 */
    isect2(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,isect0,isect1,isectpoint0,isectpoint1);   
  }                                                  
  else if(D1!=0.0f)                                  
  {                                               
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,isect0,isect1,isectpoint0,isectpoint1); 
  }                                         
  else if(D2!=0.0f)                                  
  {                                                   
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,isect0,isect1,isectpoint0,isectpoint1);     
  }                                                 
  else                                               
  {                                                   
    /* triangles are coplanar */    
    return 1;
  }
  return 0;
}

#define COMPUTE_INTERVALS_ISECTLINE(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,isect0,isect1,isectpoint0,isectpoint1) \
  if(D0D1>0.0f)                                         \
  {                                                     \
    /* here we know that D0D2<=0.0 */                   \
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,&isect0,&isect1,isectpoint0,isectpoint1);          \
  }                                                     
#if 0
  else if(D0D2>0.0f)                                    \
  {                                                     \
    /* here we know that d0d1<=0.0 */                   \
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,&isect0,&isect1,isectpoint0,isectpoint1);          \
  }                                                     \
  else if(D1*D2>0.0f || D0!=0.0f)                       \
  {                                                     \
    /* here we know that d0d1<=0.0 or that D0!=0.0 */   \
    isect2(VERT0,VERT1,VERT2,VV0,VV1,VV2,D0,D1,D2,&isect0,&isect1,isectpoint0,isectpoint1);          \
  }                                                     \
  else if(D1!=0.0f)                                     \
  {                                                     \
    isect2(VERT1,VERT0,VERT2,VV1,VV0,VV2,D1,D0,D2,&isect0,&isect1,isectpoint0,isectpoint1);          \
  }                                                     \
  else if(D2!=0.0f)                                     \
  {                                                     \
    isect2(VERT2,VERT0,VERT1,VV2,VV0,VV1,D2,D0,D1,&isect0,&isect1,isectpoint0,isectpoint1);          \
  }                                                     \
  else                                                  \
  {                                                     \
    /* triangles are coplanar */                        \
    coplanar=1;                                         \
    return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);      \
  }
#endif

int tri_tri_intersect_with_isectline(float3 const& V0, float3 const& V1, float3 const& V2,
				     float3 const& U0, float3 const& U1, float3 const& U2, int *coplanar,
				     float3& isectpt1, float3& isectpt2)
{
  float3 E1,E2;
  float3 N1,N2;
  float d1,d2;
  float du0,du1,du2,dv0,dv1,dv2;
  float3 D;
  float2 isect1, isect2;
  float3 isectpointA1,isectpointA2;
  float3 isectpointB1,isectpointB2;
  float du0du1,du0du2,dv0dv1,dv0dv2;
  short index;
  float vp0,vp1,vp2;
  float up0,up1,up2;
  float b,c,max;
//  float tmp;
  float3 diff;
  int smallest1,smallest2;
  
  /* compute plane equation of triangle(V0,V1,V2) */
  SUB(E1,V1,V0);
  SUB(E2,V2,V0);
  CROSS(N1,E1,E2);
  d1=-DOT(N1,V0);
  /* plane equation 1: N1.X+d1=0 */

  /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
  du0=DOT(N1,U0)+d1;
  du1=DOT(N1,U1)+d1;
  du2=DOT(N1,U2)+d1;

  /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
  if(fabs(du0)<EPSILON) du0=0.0;
  if(fabs(du1)<EPSILON) du1=0.0;
  if(fabs(du2)<EPSILON) du2=0.0;
#endif
  du0du1=du0*du1;
  du0du2=du0*du2;

  if(du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute plane of triangle (U0,U1,U2) */
  SUB(E1,U1,U0);
  SUB(E2,U2,U0);
  CROSS(N2,E1,E2);
  d2=-DOT(N2,U0);
  /* plane equation 2: N2.X+d2=0 */

  /* put V0,V1,V2 into plane equation 2 */
  dv0=DOT(N2,V0)+d2;
  dv1=DOT(N2,V1)+d2;
  dv2=DOT(N2,V2)+d2;

#if USE_EPSILON_TEST==TRUE
  if(fabs(dv0)<EPSILON) dv0=0.0;
  if(fabs(dv1)<EPSILON) dv1=0.0;
  if(fabs(dv2)<EPSILON) dv2=0.0;
#endif

  dv0dv1=dv0*dv1;
  dv0dv2=dv0*dv2;
        
  if(dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
    return 0;                    /* no intersection occurs */

  /* compute direction of intersection line */
  CROSS(D,N1,N2);

  /* compute and index to the largest component of D */
  max=fabs(D[0]);
  index=0;
  b=fabs(D[1]);
  c=fabs(D[2]);
  if(b>max) max=b,index=1;
  if(c>max) max=c,index=2;

  /* this is the simplified projection onto L*/
  vp0=V0[index];
  vp1=V1[index];
  vp2=V2[index];
  
  up0=U0[index];
  up1=U1[index];
  up2=U2[index];

  /* compute interval for triangle 1 */
  *coplanar=compute_intervals_isectline(V0,V1,V2,vp0,vp1,vp2,dv0,dv1,dv2,
				       dv0dv1,dv0dv2,&isect1[0],&isect1[1],isectpointA1,isectpointA2);
  if(*coplanar) return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);     


  /* compute interval for triangle 2 */
  *coplanar=compute_intervals_isectline(U0,U1,U2,up0,up1,up2,du0,du1,du2,
			      du0du1,du0du2,&isect2[0],&isect2[1],isectpointB1,isectpointB2);
  if(*coplanar) return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);     

  SORT2(isect1[0],isect1[1],smallest1);
  SORT2(isect2[0],isect2[1],smallest2);

  if(isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;

  /* at this point, we know that the triangles intersect */

  if(isect2[0]<isect1[0])
  {
    if(smallest1==0) { SET(isectpt1,isectpointA1); }
    else { SET(isectpt1,isectpointA2); }

    if(isect2[1]<isect1[1])
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); }
    }
    else
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }
    }
  }
  else
  {
    if(smallest2==0) { SET(isectpt1,isectpointB1); }
    else { SET(isectpt1,isectpointB2); }

    if(isect2[1]>isect1[1])
    {
      if(smallest1==0) { SET(isectpt2,isectpointA2); }
      else { SET(isectpt2,isectpointA1); }      
    }
    else
    {
      if(smallest2==0) { SET(isectpt2,isectpointB2); }
      else { SET(isectpt2,isectpointB1); } 
    }
  }
  return 1;
}

int
intersect_triangle(float3 const& orig, float3 const dir,
                   float3 const& vert0, float3 const& vert1, float3 const& vert2,
                   float *t, float *u, float *v)
{
   float3 edge1, edge2, tvec, pvec, qvec;
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0f / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

bool xray::ai::navigation::intersect_triangles	( triangles_mesh_type const& triangles_mesh, u32 const triangle_id0, u32 const triangle_id1, bool const could_be_coplanar )
{
	u32 const* indices0		= &*triangles_mesh.indices.begin() + 3*triangle_id0;
	float3 const& v0	= triangles_mesh.vertices[ indices0[0] ];
	float3 const& v1	= triangles_mesh.vertices[ indices0[1] ];
	float3 const& v2	= triangles_mesh.vertices[ indices0[2] ];

	u32 const* indices1		= &*triangles_mesh.indices.begin() + 3*triangle_id1;
	float3 const& u0	= triangles_mesh.vertices[ indices1[0] ];
	float3 const& u1	= triangles_mesh.vertices[ indices1[1] ];
	float3 const& u2	= triangles_mesh.vertices[ indices1[2] ];

	int is_coplanar;
	float3 result0, result1;
	int const result		=
		tri_tri_intersect_with_isectline(
			v0, v1, v2, u0, u1, u2, &is_coplanar, result0, result1
		);

	if ( !result )
		return				false;

	if ( is_coplanar )
		return				could_be_coplanar;

	return					!result0.is_similar(result1, math::epsilon_3);
}

#endif // #if XRAY_DEBUG_ALLOCATOR