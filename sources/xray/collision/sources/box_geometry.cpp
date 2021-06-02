////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.03.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: box geometry
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "box_geometry.h"
#include "sphere_geometry.h"
#include "triangle_mesh_base.h"

#include <xray/collision/contact_info.h>
#include <xray/render/base/debug_renderer.h>

using xray::memory::base_allocator;

namespace xray {
namespace collision {

box_geometry::box_geometry					( base_allocator* allocator, const float3 &half_sides ):
	m_allocator( allocator ),
	m_half_sides( half_sides )
{
}

void box_geometry::generate_contacts		(  on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )		const
{
	og.generate_contacts( c, transform, self_transform, *this );
}

void box_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const triangle_mesh_base& og )			const 
{
	og.generate_contacts( c, transform, self_transform, *this );
}

void box_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og )			const
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void box_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const compound_geometry& og )			const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void box_geometry::render					( render::debug::renderer& renderer, float4x4 const& matrix ) const 
{
	renderer.draw_obb( matrix, m_half_sides, math::color( 255u, 255u, 255u, 255u ) );
	//.draw_sphere( matrix.c.xyz(), m_radius, math::color_xrgb( 255, 255, 255 ) );
}

xray::math::aabb& box_geometry::get_aabb		( math::aabb& result ) const
{
	result				= math::create_min_max( -m_half_sides, m_half_sides );
	return				result;
}

bool box_geometry::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const 
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	return false; 
}

bool box_geometry::cuboid_query	( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	return false; 
}

bool box_geometry::ray_query		( object const* object,
									 float3 const& origin,
									 float3 const& direction,
									 float max_distance,
									 float& distance,
									 ray_triangles_type& triangles,
									 triangles_predicate_type const& predicate ) const				
{
	XRAY_UNREFERENCED_PARAMETERS	( object, &origin, &direction, max_distance, distance, &triangles, &predicate );
	return false; 
}

bool box_geometry::aabb_test				( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER				( aabb );
	return true;
}

bool box_geometry::cuboid_test				( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER				( cuboid );
	return true;
}

bool box_geometry::ray_test					( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS			( &origin, &direction, max_distance, distance );
	return true;
}

void dLineClosestApproach (const float3 &pa, const float3 &ua,
			   const float3 &pb, const float3 &ub,
			   float &alpha, float &beta)
{
  float3 p;
  //p[0] = pb[0] - pa[0];
  //p[1] = pb[1] - pa[1];
  //p[2] = pb[2] - pa[2];
  p = pb - pa;
  float uaub = (ua|ub);
  float q1 =  (ua|p);
  float q2 = -(ub|p);
  float d = 1-uaub*uaub;
  if (d <= (0.0001f)) {
    // @@@ this needs to be made more robust
    alpha = 0;
    beta  = 0;
  }
  else {
    d = 1.f/d;//dRecip(d);
    alpha = (q1 + uaub*q2)*d;
    beta  = (uaub*q1 + q2)*d;
  }
}
static int intersectRectQuad (float h[2], float p[8], float ret[16])
{
  // q (and r) contain nq (and nr) coordinate points for the current (and
  // chopped) polygons
  int nq=4,nr=-1;
  float buffer[16];
  float *q = p;
  float *r = ret;
  for (int dir=0; dir <= 1; dir++) {
    // direction notation: xy[0] = x axis, xy[1] = y axis
    for (int sign=-1; sign <= 1; sign += 2) {
      // chop q along the line xy[dir] = sign*h[dir]
      float *pq = q;
      float *pr = r;
      nr = 0;
      for (int i=nq; i > 0; i--) {
	// go through all points in q and all lines between adjacent points
	if (sign*pq[dir] < h[dir]) {
	  // this point is inside the chopping line
	  pr[0] = pq[0];
	  pr[1] = pq[1];
	  pr += 2;
	  nr++;
	  if (nr & 8) {
	    q = r;
	    goto done;
	  }
	}
	float *nextq = (i > 1) ? pq+2 : q;
	if ((sign*pq[dir] < h[dir]) ^ (sign*nextq[dir] < h[dir])) {
	  // this line crosses the chopping line
	  pr[1-dir] = pq[1-dir] + (nextq[1-dir]-pq[1-dir]) /
	    (nextq[dir]-pq[dir]) * (sign*h[dir]-pq[dir]);
	  pr[dir] = sign*h[dir];
	  pr += 2;
	  nr++;
	  if (nr & 8) {
	    q = r;
	    goto done;
	  }
	}
	pq += 2;
      }
      q = r;
      r = (q==ret) ? buffer : ret;
      nq = nr;
    }
  }
 done:
  if (q != ret) memory::copy(ret,nr*2*sizeof(float),q,nr*2*sizeof(float));
  return nr;
}


inline bool CrossBoxSide44(const float3 &point,const float4x4 &R1,const int ax_num,
						   const float3 &p,const float4x4 &R2,const float3 &side,
						   const int side_num,const float sign, float3 &out_p)
{
	//float3 plane_point(p[0],p[1],p[2]);
	float3 plane_point = p;
	//int i;

	//for (i=0;i<3;i++)plane_point[i]+=R2[side_num+i*4]*side[side_num]/2.f*sign;
	plane_point += R2.lines[side_num].xyz()*side[side_num]*sign;///2.f
	float _cos=(R1.lines[ax_num].xyz()|R2.lines[side_num].xyz());

	float length=((point|R2.lines[side_num].xyz())-(plane_point|R2.lines[side_num].xyz()))/_cos;
	//for (i=0;i<3;i++)out_p[i]=point[i]-R1[i*4+ax_num]*length;
	out_p = point-R1.lines[ax_num].xyz() * length;
	int nx1=(side_num+1)%3;
	int nx2=(side_num+2)%3;
	//dReal pr1 =dFabs(dDOT14(out_p,R2+nx1)-dDOT14(plane_point,R2+nx1));
	//dReal pr2 =dFabs(dDOT14(out_p,R2+nx2)-dDOT14(plane_point,R2+nx2));
	return !((abs((out_p|R2.lines[nx1].xyz())-(plane_point|R2.lines[nx1].xyz()))>side[nx1]/2.f))&&
		!((abs((out_p|R2.lines[nx2].xyz())-(plane_point|R2.lines[nx2].xyz()))>side[nx2]/2.f));
}


void cullPoints (int n, float p[], int m, int i0, int iret[])
{
  // compute the centroid of the polygon in cx,cy
  int i,j;
  float a,cx,cy,q;
  if (n==1) {
    cx = p[0];
    cy = p[1];
  }
  else if (n==2) {
    cx = (0.5f)*(p[0] + p[2]);
    cy = (0.5f)*(p[1] + p[3]);
  }
  else {
    a = 0;
    cx = 0;
    cy = 0;
    for (i=0; i<(n-1); i++) {
      q = p[i*2]*p[i*2+3] - p[i*2+2]*p[i*2+1];
      a += q;
      cx += q*(p[i*2]+p[i*2+2]);
      cy += q*(p[i*2+1]+p[i*2+3]);
    }
    q = p[n*2-2]*p[1] - p[0]*p[n*2-1];
    a = 1.f/(3.0f)*(a+q);//dRecip((3.0f)*(a+q));
    cx = a*(cx + q*(p[n*2-2]+p[0]));
    cy = a*(cy + q*(p[n*2-1]+p[1]));
  }

  // compute the angle of each point w.r.t. the centroid
  float A[8];
  for (i=0; i<n; i++) A[i] = math::atan2(p[i*2+1]-cy,p[i*2]-cx);

  // search for points that have angles closest to A[i0] + i*(2*pi/m).
  int avail[8];
  for (i=0; i<n; i++) avail[i] = 1;
  avail[i0] = 0;
  iret[0] = i0;
  iret++;
  for (j=1; j<m; j++) {
	  a = float(j)*(2*math::pi/m) + A[i0];
    if (a > math::pi) a -= 2*math::pi;
    float maxdiff=1e9,diff;
    for (i=0; i<n; i++) {
      if (avail[i]) {
	diff = abs (A[i]-a);
	if (diff > math::pi) diff = 2*math::pi - diff;
	if (diff < maxdiff) {
	  maxdiff = diff;
	  *iret = i;
	}
      }
    }
    avail[*iret] = 0;
    iret++;
  }
}



void dBoxBox ( 
		     on_contact& on_c,
			 const float3 &p1, const float4x4 &RR1,
			 const float3 &side1, const float3 &p2,
			 const float4x4 &RR2, const float3 &side2
			// float3 normal, float *depth, int *return_code,
			 //int maxc, dContactGeom *contact, int skip
			 )
{

	
	float3 normal;
	float depth; 
	int return_code;
	
	int maxc = 8;

	float4x4 R1 = RR1;
	R1.c.xyz().set(0,0,0);
	float4x4 R2 = RR2;
	R2.c.xyz().set(0,0,0);

	const float3	&R1i = R1.lines[0].xyz();
	const float3	&R1j = R1.lines[1].xyz();
	const float3	&R1k = R1.lines[2].xyz();

	const float3	&R2i = R2.lines[0].xyz();
	const float3	&R2j = R2.lines[1].xyz();
	const float3	&R2k = R2.lines[2].xyz();



	const float fudge_factor = 1.05f;//1.05->1.25 @slipch
	float3 p,pp,normalC;
	const float3 *normalR = 0;
	float3 A,B;
	float R11,R12,R13,R21,R22,R23,R31,R32,R33,
		Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l;
	int i,j,invert_normal,code;

	// get vector from centers of box 1 to box 2, relative to box 1
	//p[0] = p2[0] - p1[0];
	//p[1] = p2[1] - p1[1];
	//p[2] = p2[2] - p1[2];
	p = p2 - p1;

	const float4x4 invR1 = transpose( R1 );
	//dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1
	pp = p * invR1;

	// get side lengths / 2
	//A[0] = side1[0];
	//A[1] = side1[1];
	//A[2] = side1[2];

	//B[0] = side2[0];
	//B[1] = side2[1];
	//B[2] = side2[2];
	A = side1;
	B = side2;

	// Rij is R1'*R2, i.e. the relative rotation between R1 and R2
	R11 = (R1i|R2i); R12 = (R1i|R2j); R13 = (R1i|R2k);
	R21 = (R1j|R2i); R22 = (R1j|R2j); R23 = (R1j|R2k);
	R31 = (R1k|R2i); R32 = (R1k|R2j); R33 = (R1k|R2k);

	Q11 = abs(R11); Q12 = abs(R12); Q13 = abs(R13);
	Q21 = abs(R21); Q22 = abs(R22); Q23 = abs(R23);
	Q31 = abs(R31); Q32 = abs(R32); Q33 = abs(R33);

	// for all 15 possible separating axes:
	//   * see if the axis separates the boxes. if so, return 0.
	//   * find the depth of the penetration along the separating axis (s2)
	//   * if this is the largest depth so far, record it.
	// the normal vector will be set to the separating axis with the smallest
	// depth. note: normalR is set to point to a column of R1 or R2 if that is
	// the smallest depth normal so far. otherwise normalR is 0 and normalC is
	// set to a vector relative to body 1. invert_normal is 1 if the sign of
	// the normal should be flipped.

#define TST(expr1,expr2,norm,cc) \
	s2 = abs(expr1) - (expr2); \
	if (s2 > 0) return ; \
	if (s2 > s) { \
	s = s2; \
	normalR = &norm; \
	invert_normal = ((expr1) < 0); \
	code = (cc); \
	}

	s = -math::infinity;
	invert_normal = 0;
	code = 0;

	// separating axis = u1,u2,u3
	TST (pp[0],(A[0] + B[0]*Q11 + B[1]*Q12 + B[2]*Q13),R1i,1);
	TST (pp[1],(A[1] + B[0]*Q21 + B[1]*Q22 + B[2]*Q23),R1j,2);
	TST (pp[2],(A[2] + B[0]*Q31 + B[1]*Q32 + B[2]*Q33),R1k,3);

	// separating axis = v1,v2,v3
	TST ((R2i|p),(A[0]*Q11 + A[1]*Q21 + A[2]*Q31 + B[0]),R2i,4);
	TST ((R2j|p),(A[0]*Q12 + A[1]*Q22 + A[2]*Q32 + B[1]),R2j,5);
	TST ((R2k|p),(A[0]*Q13 + A[1]*Q23 + A[2]*Q33 + B[2]),R2k,6);

	// note: cross product axes need to be scaled when s is computed.
	// normal (n1,n2,n3) is relative to box 1.
#undef TST
#define TST(expr1,expr2,n1,n2,n3,cc) \
	s2 = abs(expr1) - (expr2); \
	if (s2 > 0) return ; \
	l = sqrt ((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
	if (l > 0) { \
	s2 /= l; \
	if (s2*fudge_factor > s) { \
	s = s2; \
	normalR = 0; \
	normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
	invert_normal = ((expr1) < 0); \
	code = (cc); \
	} \
	}

	// separating axis = u1 x (v1,v2,v3)
	TST(pp[2]*R21-pp[1]*R31,(A[1]*Q31+A[2]*Q21+B[1]*Q13+B[2]*Q12),0,-R31,R21,7);
	TST(pp[2]*R22-pp[1]*R32,(A[1]*Q32+A[2]*Q22+B[0]*Q13+B[2]*Q11),0,-R32,R22,8);
	TST(pp[2]*R23-pp[1]*R33,(A[1]*Q33+A[2]*Q23+B[0]*Q12+B[1]*Q11),0,-R33,R23,9);

	// separating axis = u2 x (v1,v2,v3)
	TST(pp[0]*R31-pp[2]*R11,(A[0]*Q31+A[2]*Q11+B[1]*Q23+B[2]*Q22),R31,0,-R11,10);
	TST(pp[0]*R32-pp[2]*R12,(A[0]*Q32+A[2]*Q12+B[0]*Q23+B[2]*Q21),R32,0,-R12,11);
	TST(pp[0]*R33-pp[2]*R13,(A[0]*Q33+A[2]*Q13+B[0]*Q22+B[1]*Q21),R33,0,-R13,12);

	// separating axis = u3 x (v1,v2,v3)
	TST(pp[1]*R11-pp[0]*R21,(A[0]*Q21+A[1]*Q11+B[1]*Q33+B[2]*Q32),-R21,R11,0,13);
	TST(pp[1]*R12-pp[0]*R22,(A[0]*Q22+A[1]*Q12+B[0]*Q33+B[2]*Q31),-R22,R12,0,14);
	TST(pp[1]*R13-pp[0]*R23,(A[0]*Q23+A[1]*Q13+B[0]*Q32+B[1]*Q31),-R23,R13,0,15);

#undef TST

	if (!code) return ;
	contact_info ci[8];

	// if we get to this point, the boxes interpenetrate. compute the normal
	// in global coordinates.
	if (normalR) {
		//normal[0] = normalR[0];
		//normal[1] = normalR[4];
		//normal[2] = normalR[8];
		normal = *normalR;
	}
	else {
		//dMULTIPLY0_331 (normal,R1,normalC);
		normal = normalC * R1;
	}
	if (invert_normal) {
		//normal[0] = -normal[0];
		//normal[1] = -normal[1];
		//normal[2] = -normal[2];
		normal = -normal;
	}
	depth = -s;

	// compute contact point(s)

	if (code > 6) {
		// an edge from box 1 touches an edge from box 2.
		int iacr=((code)-7)/3;
		int ibcr=((code)-7)%3;
		int iamx,ianx,ibmx,ibnx;
		int ret=0;
		if(
			abs((normal|R1.lines[(iacr+1)%3].xyz()))>
			abs((normal|R1.lines[(iacr+2)%3].xyz()))
			)
		{
			iamx=(iacr+1)%3;
			ianx=(iacr+2)%3;
		}
		else
		{
			iamx=(iacr+2)%3;
			ianx=(iacr+1)%3;
		}

		if(
			abs((normal|R2.lines[(ibcr+1)%3].xyz()))>
			abs((normal|R2.lines[(ibcr+2)%3].xyz()))
			)
		{
			ibmx=(ibcr+1)%3;
			ibnx=(ibcr+2)%3;
		}
		else
		{
			ibmx=(ibcr+2)%3;
			ibnx=(ibcr+1)%3;
		}
		


		// find a point pa on the intersecting edge of box 1
		float3 pa;
		float sign;
		//for (i=0; i<3; i++) pa[i] = p1[i];
		pa = p1;

		for (j=0; j<3; j++) {
			const float3 &side_ax  = R1.lines[j].xyz();
			sign = ((normal|side_ax) > 0.f) ? (1.0f) : (-1.0f);
			//for (i=0; i<3; i++) pa[i] += sign * A[j] * R1[i*4+j];
			pa += sign * A[j] * side_ax;
		}

		float3  pa1,pa2,pa3;//psf,

		float3  pb1,pb2,pb3;

		// find a point pb on the intersecting edge of box 2
		float3 pb;
		//for (i=0; i<3; i++) pb[i] = p2[i];
		pb = p2;
		for (j=0; j<3; j++) {
			const float3 &side_ax  = R2.lines[j].xyz();
			sign = ((normal|side_ax) > 0.f) ? (-1.0f) : (1.0f);
			pb += sign * B[j] * side_ax;
			//sign = (dDOT14(normal,R2+j) > 0.f) ? REAL(-1.0) : REAL(1.0);
			//for (i=0; i<3; i++) pb[i] += sign * B[j] * R2[i*4+j];
		}

		float alpha,beta;
		float3 ua,ub,pa0,pb0;
		//for (i=0; i<3; i++) ua[i] = R1[((code)-7)/3 + i*4];
		ua = R1.lines[((code)-7)/3].xyz();

		//for (i=0; i<3; i++) ub[i] = R2[((code)-7)%3 + i*4];
		ub = R2.lines[((code)-7)%3].xyz();

		dLineClosestApproach (pa,ua,pb,ub,alpha,beta);
		//for (i=0; i<3; i++) pa0[i] =pa[i]+ ua[i]*alpha;
		pa0 = pa + ua * alpha;

		//for (i=0; i<3; i++) pb0[i] =pb[i]+ ub[i]*beta;
		pb0 = pb + ub * beta;

		//for (i=0; i<3; i++) contact[0].pos[i] = (0.5f)*(pa0[i]+pb0[i]);
		//contact[0].depth = *depth;
		ci[0].position = (0.5f)*(pa0+pb0);
		ci[0].depth = depth;
		ret++;



		float3 dif;
		//for (i=0; i<3; i++) dif[i]=p2[i]-pa0[i];
		dif = p2 - pa0;

		sign = ((dif|R2.lines[ibnx].xyz()) > 0.f) ? (1.0f) : (-1.0f);

		if(CrossBoxSide44(pa0,R1,iacr,p2,R2,side2,ibnx,sign,pb1))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pb1[i];
			ci[ret].position = pb1;
			//for (i=0; i<3; i++) dif[i]=pb1[i]-pa0[i];
			dif = pb1 - pa0;

			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R2+ibmx,dif)*dDOT41(R2+ibmx,normal));
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
			//	ret++;
			ci[ret].depth = depth-abs((R2.lines[ibmx].xyz()|dif)*(R2.lines[ibmx].xyz()|normal));
			if( ci[ret].depth>0.f )
				ret++;
		}


		if(CrossBoxSide44(pa0,R1,iacr,p2,R2,side2,ibcr,1,pb2))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pb2[i];
			ci[ret].position = pb2;
			//for (i=0; i<3; i++) dif[i]=pb2[i]-pa0[i];
			dif = pb2 - pa0;
			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R2+ibmx,dif)*dDOT41(R2+ibmx,normal));
			ci[ret].depth=depth-abs((R2.lines[ibmx].xyz()|dif)*(R2.lines[ibmx].xyz()|normal));
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
			//	ret++;
			if(ci[ret].depth>0.f) 
				ret++;
		}


		if(CrossBoxSide44(pa0,R1,iacr,p2,R2,side2,ibcr,-1,pb3))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pb3[i];
			ci[ret].position = pb3;
			//for (i=0; i<3; i++) dif[i]=pb3[i]-pa0[i];
			dif = pb3-pa0;
			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R2+ibmx,dif)*dDOT41(R2+ibmx,normal));
			ci[ret].depth= depth-abs((R2.lines[ibmx].xyz()|dif)*(R2.lines[ibmx].xyz()|normal));
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
			//	ret++;
			if(ci[ret].depth>0.f) 
				ret++;
		}



		//for (i=0; i<3; i++) dif[i]=p1[i]-pb0[i];
		dif = p1 - pb0;
		sign = ((dif|R1.lines[ianx].xyz()) > 0.f) ? (1.0f) : (-1.0f);
		if(CrossBoxSide44(pb0,R2,ibcr,p1,R1,side1,ianx,sign,pa1))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pa1[i];
			ci[ret].position = pa1;
			//for (i=0; i<3; i++) dif[i]=pa1[i]-pb0[i];
			dif = pa1-pb0;
			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R1+iamx,dif)*dDOT41(R1+iamx,normal));
			ci[ret].depth = depth-abs((R1.lines[iamx].xyz()|dif)*(R1.lines[iamx].xyz()|normal));
			if(ci[ret].depth>0.f) 
				ret++;
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
			//	ret++;
		}


		if(CrossBoxSide44(pb0,R2,ibcr,p2,R1,side1,iacr,1,pa2))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pa2[i];
			ci[ret].position = pa2;
			//for (i=0; i<3; i++) dif[i]=pa2[i]-pb0[i];
			dif = pa2-pb0;
			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R1+iamx,dif)*dDOT41(R1+iamx,normal));
			ci[ret].depth = depth-abs((R1.lines[iamx].xyz()|dif)*(R1.lines[iamx].xyz()|normal));
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
				//ret++;
			if(ci[ret].depth>0.f) 
				ret++;
		}


		if(CrossBoxSide44(pb0,R2,ibcr,p2,R1,side1,iacr,-1,pa3))
		{
			//for (i=0; i<3; i++)CONTACT(contact,skip*ret)->pos[i]=pa3[i];
			ci[ret].position = pa3;
			//for (i=0; i<3; i++) dif[i]=pa3[i]-pb0[i];
			dif = pa3-pb0;
			//CONTACT(contact,skip*ret)->depth=*depth-abs(dDOT41(R1+iamx,dif)*dDOT41(R1+iamx,normal));
			ci[ret].depth = depth-abs((R1.lines[iamx].xyz()|dif)*(R1.lines[iamx].xyz()|normal));
			//if(CONTACT(contact,skip*ret)->depth>0.f) 
			//	ret++;
			if(ci[ret].depth>0.f) 
				ret++;
		}
		return_code = code;
		for( int ri = 0; ri < ret; ri++ )
		{
			ci[ri].normal = normal;
			on_c(ci[ri]);
		}
		return ;
	}

	// okay, we have a face-something intersection (because the separating
	// axis is perpendicular to a face). define face 'a' to be the reference
	// face (i.e. the normal vector is perpendicular to this) and face 'b' to be
	// the incident face (the closest face of the other box).

	const float4x4 *Ra,*Rb;
	const float3 *pa,*pb,*Sa,*Sb;
	if (code <= 3) {
		Ra =&R1;
		Rb =&R2;
		pa =&p1;
		pb =&p2;
		Sa =&A;
		Sb =&B;
	}
	else {
		Ra = &R2;
		Rb = &R1;
		pa = &p2;
		pb = &p1;
		Sa = &B;
		Sb = &A;
	}

	// nr = normal vector of reference face dotted with axes of incident box.
	// anr = absolute values of nr.
	float3 normal2,nr,anr;
	if (code <= 3) {
		//normal2[0] = normal[0];
		//normal2[1] = normal[1];
		//normal2[2] = normal[2];
		normal2 = normal;
	}
	else {
		//normal2[0] = -normal[0];
		//normal2[1] = -normal[1];
		//normal2[2] = -normal[2];
		normal2 = -normal;
	}
	const float4x4 invRb = transpose( *Rb  );
	//dMULTIPLY1_331 (nr,Rb,normal2);
	nr = normal2 * invRb;
	anr[0] = abs (nr[0]);
	anr[1] = abs (nr[1]);
	anr[2] = abs (nr[2]);

	// find the largest compontent of anr: this corresponds to the normal
	// for the indident face. the other axis numbers of the indicent face
	// are stored in a1,a2.
	int lanr,a1,a2;
	if (anr[1] > anr[0]) {
		if (anr[1] > anr[2]) {
			a1 = 0;
			lanr = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}
	else {
		if (anr[0] > anr[2]) {
			lanr = 0;
			a1 = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}

	// compute center point of incident face, in reference-face coordinates
	float3 center;
	if (nr[lanr] < 0) {
		//for (i=0; i<3; i++) center[i] = pb[i] - pa[i] + Sb[lanr] * Rb[i*4+lanr];
		center = (*pb) - (*pa) + ( (*Sb)[lanr] * ( Rb->lines[lanr].xyz() ) );
	}
	else {
		//for (i=0; i<3; i++) center[i] = pb[i] - pa[i] - Sb[lanr] * Rb[i*4+lanr];
		center = (*pb) - (*pa) - (*Sb)[lanr] * Rb->lines[lanr].xyz();
	}

	// find the normal and non-normal axis numbers of the reference box
	int codeN,code1,code2;
	if (code <= 3) codeN = code-1; else codeN = code-4;
	if (codeN==0) {
		code1 = 1;
		code2 = 2;
	}
	else if (codeN==1) {
		code1 = 0;
		code2 = 2;
	}
	else {
		code1 = 0;
		code2 = 1;
	}

	// find the four corners of the incident face, in reference-face coordinates
	float quad[8];	// 2D coordinate of incident face (x,y pairs)
	float c1,c2,m11,m12,m21,m22;
	c1 = (center|Ra->lines[code1].xyz());
	c2 = (center|Ra->lines[code2].xyz());
	// optimize this? - we have already computed this data above, but it is not
	// stored in an easy-to-index format. for now it's quicker just to recompute
	// the four dot products.
	m11 =  (Ra->lines[code1].xyz()|Rb->lines[a1].xyz());
	m12 =  (Ra->lines[code1].xyz()|Rb->lines[a2].xyz());
	m21 =  (Ra->lines[code2].xyz()|Rb->lines[a1].xyz());
	m22 =  (Ra->lines[code2].xyz()|Rb->lines[a2].xyz());
	{
		float k1 = m11* (*Sb)[a1];
		float k2 = m21* (*Sb)[a1];
		float k3 = m12* (*Sb)[a2];
		float k4 = m22* (*Sb)[a2];
		quad[0] = c1 - k1 - k3;
		quad[1] = c2 - k2 - k4;
		quad[2] = c1 - k1 + k3;
		quad[3] = c2 - k2 + k4;
		quad[4] = c1 + k1 + k3;
		quad[5] = c2 + k2 + k4;
		quad[6] = c1 + k1 - k3;
		quad[7] = c2 + k2 - k4;
	}

	// find the size of the reference face
	float rect[2];
	rect[0] = (*Sa)[code1];
	rect[1] = (*Sa)[code2];

	// intersect the incident and reference faces
	float ret[16];
	int n = intersectRectQuad (rect,quad,ret);
	if (n < 1) return ;		// this should never happen

	// convert the intersection points into reference-face coordinates,
	// and compute the contact position and depth for each point. only keep
	// those points that have a positive (penetrating) depth. delete points in
	// the 'ret' array as necessary so that 'point' and 'ret' correspond.
	float point[3*8];		// penetrating contact points
	float dep[8];			// depths for those points
	//float det1 = dRecip(m11*m22 - m12*m21);
	float det1 = 1.f/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;
	int cnum = 0;			// number of penetrating contact points found
	for (j=0; j < n; j++) {
		float k1 =  m22*(ret[j*2]-c1) - m12*(ret[j*2+1]-c2);
		float k2 = -m21*(ret[j*2]-c1) + m11*(ret[j*2+1]-c2);

		for (i=0; i<3; i++) point[cnum*3+i] =
			center[i] + k1*Rb->lines[a1].xyz()[i] + k2*Rb->lines[a2].xyz()[i];

		const float3  point_cnum_3( point[cnum*3], point[cnum*3+1], point[cnum*3+2] );
		dep[cnum] = (*Sa)[codeN] - (normal2|point_cnum_3);
		if (dep[cnum] >= 0) {
			ret[cnum*2] = ret[j*2];
			ret[cnum*2+1] = ret[j*2+1];
			cnum++;
		}
	}
	if (cnum < 1) return ;	// this should never happen

	// we can't generate more contacts than we actually have
	if (maxc > cnum) maxc = cnum;
	if (maxc < 1) maxc = 1;

	if (cnum <= maxc) {
		// we have less contacts than we need, so we use them all
		for (j=0; j < cnum; j++) {
			//dContactGeom *con = CONTACT(contact,skip*j);
			contact_info &con =  ci[j]; 
			for (i=0; i<3; i++) con.position[i] = point[j*3+i] + (*pa)[i];
			
			con.depth = dep[j];
		}
	}
	else {
		// we have more contacts than are wanted, some of them must be culled.
		// find the deepest point, it is always the first contact.
		int i1 = 0;
		float maxdepth = dep[0];
		for (i=1; i<cnum; i++) {
			if (dep[i] > maxdepth) {
				maxdepth = dep[i];
				i1 = i;
			}
		}

		int iret[8];
		cullPoints (cnum,ret,maxc,i1,iret);

		for (j=0; j < maxc; j++) {
			//dContactGeom *con = CONTACT(contact,skip*j);
			contact_info &con =  ci[j]; 
			for (i=0; i<3; i++) con.position[i] = point[iret[j]*3+i] + (*pa)[i];
			con.depth = dep[iret[j]];
		}
		cnum = maxc;
	}

	return_code = code;
	for( int ri = 0; ri < cnum; ri++ )
	{
		ci[ri].normal = normal;
		on_c(ci[ri]);
	}
	//return cnum;
}


void dCollideSphereBox (
				on_contact& on_c,
				const float3 &sphere_pos,
				float sphere_radius,
				const float3 &box_pos,
				const float3 &box_half_sides,
				const float4x4 &box_transform
			  //dxGeom *o1, dxGeom *o2, int flags,
		      // dContactGeom *contact, int skip
			   )
{
  // this is easy. get the sphere center `p' relative to the box, and then clip
  // that to the boundary of the box (call that point `q'). if q is on the
  // boundary of the box and |p-q| is <= sphere radius, they touch.
  // if q is inside the box, the sphere is inside the box, so set a contact
  // normal to push the sphere to the closest box face.

  float3 l,t,p,q,r;
  float depth;
  int onborder = 0;

  //dIASSERT (skip >= (int)sizeof(dContactGeom));
  //dIASSERT (o1->type == dSphereClass);
  //dIASSERT (o2->type == dBoxClass);
  //dxSphere *sphere = (dxSphere*) o1;
  //dxBox *box = (dxBox*) o2;

  //contact->g1 = o1;
  //contact->g2 = o2;

  //p[0] = o1->pos[0] - o2->pos[0];
  //p[1] = o1->pos[1] - o2->pos[1];
  //p[2] = o1->pos[2] - o2->pos[2];

	const float3	&Ri = box_transform.lines[0].xyz();
	const float3	&Rj = box_transform.lines[1].xyz();
	const float3	&Rk = box_transform.lines[2].xyz();
	
	float4x4 R = box_transform ;
	R.c.xyz().set(0,0,0);

	//const float4x4 invR = transpose( box_transform );
  p = sphere_pos - box_pos;

  //l[0] = box->side[0]*REAL(0.5);
  l[0] = box_half_sides[0];
  t[0] = (p|Ri);
  if (t[0] < -l[0]) { t[0] = -l[0]; onborder = 1; }
  if (t[0] >  l[0]) { t[0] =  l[0]; onborder = 1; }

  l[1] =  box_half_sides[1]; //box->side[1]*REAL(0.5);
  t[1] = (p|Rj);
  if (t[1] < -l[1]) { t[1] = -l[1]; onborder = 1; }
  if (t[1] >  l[1]) { t[1] =  l[1]; onborder = 1; }

  t[2] = (p|Rk);
  l[2] =  box_half_sides[2]; //box->side[2]*REAL(0.5);
  if (t[2] < -l[2]) { t[2] = -l[2]; onborder = 1; }
  if (t[2] >  l[2]) { t[2] =  l[2]; onborder = 1; }

  if (!onborder) {
    // sphere center inside box. find closest face to `t'
    float min_distance = l[0] - abs(t[0]);
    int mini = 0;
    for (int i=1; i<3; i++) {
      float face_distance = l[i] - abs(t[i]);
      if (face_distance < min_distance) {
	min_distance = face_distance;
	mini = i;
      }
    }
	contact_info	ci;
    // contact position = sphere center
    //contact->pos[0] = o1->pos[0];
    //contact->pos[1] = o1->pos[1];
    //contact->pos[2] = o1->pos[2];
	ci.position = sphere_pos;
    // contact normal points to closest face
    float3 tmp;
    tmp[0] = 0;
    tmp[1] = 0;
    tmp[2] = 0;
    tmp[mini] = (t[mini] > 0) ? (1.0f) : (-1.0f);
	
    //dMULTIPLY0_331 (contact->normal,o2->R,tmp);
	ci.normal = -(tmp * R);
    // contact depth = distance to wall along normal plus radius
    ci.depth = min_distance + sphere_radius;
	on_c(ci);
    return;
  }

  //t[3] = 0;			//@@@ hmmm
  //dMULTIPLY0_331 (q,o2->R,t);
  q = t*R;
  //r[0] = p[0] - q[0];
  //r[1] = p[1] - q[1];
  //r[2] = p[2] - q[2];
  r = p - q;
  depth = sphere_radius - sqrt((r|r));
  if (depth < 0) 
			return ;
  contact_info	ci;

  //contact->pos[0] = q[0] + o2->pos[0];
  //contact->pos[1] = q[1] + o2->pos[1];
  //contact->pos[2] = q[2] + o2->pos[2];
  ci.position = q + box_pos;
  //contact->normal[0] = r[0];
  //contact->normal[1] = r[1];
  //contact->normal[2] = r[2];
  ci.normal = -r;
  //dNormalize3 (contact->normal);
  ci.normal.normalize();
  ci.depth = depth;
  on_c(ci);
}

void box_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const sphere_geometry& og )			const 
{

	dCollideSphereBox	( c, transform.c.xyz(), og.radius(), self_transform.c.xyz(), half_sides(), self_transform );
}

void box_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )				const 
{
	dBoxBox				( c, self_transform.c.xyz(), self_transform, half_sides(), transform.c.xyz(), transform, og.half_sides() );
}

void box_geometry::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangles );
	UNREACHABLE_CODE();
}

math::float3 const* box_geometry::vertices	( ) const
{
	UNREACHABLE_CODE(return 0);
}

u32 const* box_geometry::indices			( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangle_id );
	UNREACHABLE_CODE(return 0);
}

} // namespace collision
} // namespace xray