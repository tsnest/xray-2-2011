////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cylinder_geometry.h"

#include "sphere_geometry.h"
#include "box_geometry.h"
#include "triangle_mesh_base.h"
#include <xray/collision/contact_info.h>
#include <xray/render/base/debug_renderer.h>

namespace xray {
namespace collision {

cylinder_geometry::cylinder_geometry( xray::memory::base_allocator* allocator, float radius, float half_length ):
m_radius( radius ), m_half_length( half_length )
{
	XRAY_UNREFERENCED_PARAMETERS(allocator);
}

void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )const
{
		og.generate_contacts( c, transform, self_transform, *this );
}

void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const triangle_mesh_base& og )const
{
		og.generate_contacts( c, transform, self_transform, *this );
}

void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const compound_geometry& og )const
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void	cylinder_geometry::render( render::debug::renderer& renderer, float4x4 const& matrix ) const
{
	renderer.draw_cylinder_solid( matrix, float3( radius(), half_length(), radius() ), math::color( 255u, 0u, 0u, 255u ) );
}

void cylinder_geometry::add_triangles				( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		(triangles);
	UNREACHABLE_CODE();
}

bool cylinder_geometry::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	return	false;
}

bool cylinder_geometry::cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	return	false;
}

bool cylinder_geometry::ray_query			(	object const* object,
												float3 const& origin,
												float3 const& direction,
												float max_distance,
												float& distance,
												ray_triangles_type& triangles,
												triangles_predicate_type const& predicate	) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &origin, &direction, max_distance, distance, &triangles, &predicate);
	return	false;
}

xray::math::aabb& cylinder_geometry::get_aabb		( math::aabb& result ) const
{
	result				= 
		math::create_min_max(
			float3( -m_radius, -m_half_length, -m_radius ),
			float3( +m_radius, +m_half_length, +m_radius )
		);
	return				result;
}

bool cylinder_geometry::aabb_test		( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETERS		(&aabb);
	return true;
}

bool cylinder_geometry::cuboid_test		( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETERS		(&cuboid);
	return true;
}

bool cylinder_geometry::ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	XRAY_UNREFERENCED_PARAMETERS		(&origin, &direction, max_distance, distance);
	return true;
}

void cylinder_geometry::get_aabb		( math::aabb &bbox, const float4x4 &self_transform ) const
{
	
	float3 size = float3( self_transform.j.xyz() * half_length() ).abs() +  
					(	float3( self_transform.i.xyz() ).abs() + 
						float3( self_transform.k.xyz() ).abs() ) * radius();

	bbox = math::create_min_max( self_transform.c.xyz() - size, self_transform.c.xyz()  + size );

}

////////////////////////////////////////////////NOT//REFACTORED//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////NOT//REFACTORED//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////NOT//REFACTORED//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////NOT//REFACTORED//////////////////////////////////////////////////////////////////////////////////////////

// given a pointer `p' to a dContactGeom, return the dContactGeom at
// p + skip bytes.

static const float  M_SIN_PI_3		=0.8660254037844386467637231707529362f;
static const float  M_COS_PI_3		=0.5000000000000000000000000000000000f;
static const float  M_SQRT1_2		=0.7071067811865475244008443621048490f;	
//struct dxCylinder {	// cylinder
//  float radius,lz;	// radius, length along y axis //
//};

//int dCylinderClassUser = -1;

#define NUMC_MASK (0xffff)

//#define CONTACT(p,skip) ((dContactGeom*) (((char*)p) + (skip)))

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////circleIntersection//////////////////////////////////////////////////
//this does following:
//takes two circles as normals to planes n1,n2, center points cp1,cp2,and radiuses r1,r2
//finds line on which circles' planes intersect
//finds four points O1,O2 - intersection between the line and sphere with center cp1 radius r1
//					O3,O4 - intersection between the line and sphere with center cp2 radius r2
//returns false if there is no intersection
//computes distances O1-O3, O1-O4, O2-O3, O2-O4
//in "point" returns mean point between intersection points with smallest distance
/////////////////////////////////////////////////////////////////////////////////////////////////
inline bool circleIntersection(const float3& n1,const float3 &cp1,float r1,const float3 &n2,const float3 &cp2,float r2,float3 &point){
float c1=(cp1|n1);
float c2=(cp2|n2);
float _cos=(n1|n2);
float cos_2=_cos*_cos;
float sin_2=1.f-cos_2;

if(sin_2<math::epsilon_7)
 return false;

float p1=(c1-c2*_cos)/sin_2;
float p2=(c2-c1*_cos)/sin_2;
float3 lp= p1*n1 + p2 * n2;//{p1*n1[0]+p2*n2[0],p1*n1[1]+p2*n2[1],p1*n1[2]+p2*n2[2]};
float3 n = n1^n2;
//dCROSS144(n,=,n1,n2);

//float3 LC1={lp[0]-cp1[0],lp[1]-cp1[1],lp[2]-cp1[2]};
//float3 LC2={lp[0]-cp2[0],lp[1]-cp2[1],lp[2]-cp2[2]};
float3 LC1= lp - cp1;
float3 LC2= lp - cp2;

float A,B,C,B_A,B_A_2,D;
float t1,t2,t3,t4;
A=(n|n);
B=(LC1|n);
C=(LC1|LC1)-r1*r1;
B_A=B/A;
B_A_2=B_A*B_A;
D=B_A_2-C;
if(D<0.f){	//somewhat strange solution 
			//- it is needed to set some 
			//axis to sepparate cylinders
			//when their edges approach
	t1=-B_A+sqrt(-D);
	t2=-B_A-sqrt(-D);
//	return false;
	}
else{
t1=-B_A-sqrt(D);
t2=-B_A+sqrt(D);
}
B=(LC2|n);
C=(LC2|LC2)-r2*r2;
B_A=B/A;
B_A_2=B_A*B_A;
D=B_A_2-C;

if(D<0.f) {
	t3=-B_A+sqrt(-D);
	t4=-B_A-sqrt(-D);
//	return false;
	}
else{
t3=-B_A-sqrt(D);
t4=-B_A+sqrt(D);
}
//float3 O1={lp[0]+n[0]*t1,lp[1]+n[1]*t1,lp[2]+n[2]*t1};
//float3 O2={lp[0]+n[0]*t2,lp[1]+n[1]*t2,lp[2]+n[2]*t2};
float3 O1= lp + n * t1;
float3 O2= lp + n * t2;


//float3 O3={lp[0]+n[0]*t3,lp[1]+n[1]*t3,lp[2]+n[2]*t3};
//float3 O4={lp[0]+n[0]*t4,lp[1]+n[1]*t4,lp[2]+n[2]*t4};
float3 O3= lp + n * t3;
float3 O4= lp + n * t4;



//float3 L1_3={O3[0]-O1[0],O3[1]-O1[1],O3[2]-O1[2]};
//float3 L1_4={O4[0]-O1[0],O4[1]-O1[1],O4[2]-O1[2]};
float3 L1_3= O3 - O1;
float3 L1_4= O4 - O1;


//float3 L2_3={O3[0]-O2[0],O3[1]-O2[1],O3[2]-O2[2]};
//float3 L2_4={O4[0]-O2[0],O4[1]-O2[1],O4[2]-O2[2]};
float3 L2_3= O3 - O2;
float3 L2_4= O4 - O2;



float l1_3=(L1_3|L1_3);
float l1_4=(L1_4|L1_4);

float l2_3=(L2_3|L2_3);
float l2_4=(L2_4|L2_4);


if (l1_3<l1_4)
	if(l2_3<l2_4)
		if(l1_3<l2_3)
			{
			//l1_3;
			point[0]=0.5f*(O1[0]+O3[0]);
			point[1]=0.5f*(O1[1]+O3[1]);
			point[2]=0.5f*(O1[2]+O3[2]);
			}
		else{
			//l2_3;
			point[0]=0.5f*(O2[0]+O3[0]);
			point[1]=0.5f*(O2[1]+O3[1]);
			point[2]=0.5f*(O2[2]+O3[2]);
			}
	else
		if(l1_3<l2_4)
			{
			//l1_3;
			point[0]=0.5f*(O1[0]+O3[0]);
			point[1]=0.5f*(O1[1]+O3[1]);
			point[2]=0.5f*(O1[2]+O3[2]);
			}
		else{
			//l2_4;
			point[0]=0.5f*(O2[0]+O4[0]);
			point[1]=0.5f*(O2[1]+O4[1]);
			point[2]=0.5f*(O2[2]+O4[2]);
			}

else
	if(l2_3<l2_4)
		if(l1_4<l2_3)
			{
			//l1_4;
			point[0]=0.5f*(O1[0]+O4[0]);
			point[1]=0.5f*(O1[1]+O4[1]);
			point[2]=0.5f*(O1[2]+O4[2]);
			}
		else{
			//l2_3;
			point[0]=0.5f*(O2[0]+O3[0]);
			point[1]=0.5f*(O2[1]+O3[1]);
			point[2]=0.5f*(O2[2]+O3[2]);
			}
	else
		if(l1_4<l2_4)
			{
			//l1_4;
			point[0]=0.5f*(O1[0]+O4[0]);
			point[1]=0.5f*(O1[1]+O4[1]);
			point[2]=0.5f*(O1[2]+O4[2]);
			}
		else{
			//l2_4;
			point[0]=0.5f*(O2[0]+O4[0]);
			point[1]=0.5f*(O2[1]+O4[1]);
			point[2]=0.5f*(O2[2]+O4[2]);
			}

return true;
}




void lineClosestApproach (const float3 &pa, const float3 &ua,
				 const float3 &pb, const float3 &ub,
				 float *alpha, float *beta)
{
  float3 p;
  p[0] = pb[0] - pa[0];
  p[1] = pb[1] - pa[1];
  p[2] = pb[2] - pa[2];
  float uaub = (ua|ub);
  float q1 =   (ua|p);
  float q2 = - (ub|p);
  float d = 1-uaub*uaub;
  if (d <= 0) {
    // @@@ this needs to be made more robust
    *alpha = 0;
    *beta  = 0;
  }
  else {
    //d = dRecip(d);
	 d = 1.f/(d);
    *alpha = (q1 + uaub*q2)*d;
    *beta  = (uaub*q1 + q2)*d;
  }
}


// @@@ some stuff to optimize here, reuse code in contact point calculations.

void dCylBox ( on_contact& on_c,
			const float3& p1, const float4x4 &RR1,
			const float radius,const float lz, const float3 &p2,
			const float4x4 &RR2, const float3 &side2
			//float3 &normal, float *depth, int *code,
			//int maxc, int skip
			)
{
	
	float depth;
	int code;
	float3 normal;
	
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



  float3 p,pp,normalC;
  const float3 *normalR = 0;
  float B1,B2,B3,R11,R12,R13,R21,R22,R23,R31,R32,R33,
    Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l,sQ21,sQ22,sQ23;
  int i,invert_normal;

  // get _vector_ from centers of box 1 to box 2, relative to box 1
  //p[0] = p2[0] - p1[0];
  //p[1] = p2[1] - p1[1];
  //p[2] = p2[2] - p1[2];
	
  p = p2 - p1;
  const float4x4 invR1 = transpose ( R1 );//R1 has no translation

  //dMULTIPLY1_331 (pp,R1,p);		// get pp = p relative to body 1
  pp = p*invR1;
	
  // get side lengths / 2
  //A1 =radius; A2 = lz*REAL(0.5); A3 = radius;
  float hlz=lz;
  B1 = side2[0]; B2 = side2[1]; B3 = side2[2];

  // Rij is R1'*R2, i.e. the relative rotation between R1 and R2
  R11 = (R1.lines[0].xyz()|R2.lines[0].xyz()); R12 = (R1.lines[0].xyz()|R2.lines[1].xyz()); R13 = (R1.lines[0].xyz()|R2.lines[2].xyz());
  R21 = (R1.lines[1].xyz()|R2.lines[0].xyz()); R22 = (R1.lines[1].xyz()|R2.lines[1].xyz()); R23 = (R1.lines[1].xyz()|R2.lines[2].xyz());
  R31 = (R1.lines[2].xyz()|R2.lines[0].xyz()); R32 = (R1.lines[2].xyz()|R2.lines[1].xyz()); R33 = (R1.lines[2].xyz()|R2.lines[2].xyz());

  Q11 = abs(R11); Q12 = abs(R12); Q13 = abs(R13);
  Q21 = abs(R21); Q22 = abs(R22); Q23 = abs(R23);
  Q31 = abs(R31); Q32 = abs(R32); Q33 = abs(R33);

  
  //   * see if the axis separates the box with cylinder. if so, return 0.
  //   * find the depth of the penetration along the separating axis (s2)
  //   * if this is the largest depth so far, record it.
  // the normal vector3 will be set to the separating axis with the smallest
  // depth. note: normalR is set to point to a column of R1 or R2 if that is
  // the smallest depth normal so far. otherwise normalR is 0 and normalC is
  // set to a vector3 relative to body 1. invert_normal is 1 if the sign of
  // the normal should be flipped.

#define TEST(expr1,expr2,norm,cc) \
  s2 = abs(expr1) - (expr2); \
  if (s2 > 0) return ; \
  if (s2 > s) { \
    s = s2; \
    normalR = &norm; \
    invert_normal = ((expr1) < 0); \
    code = (cc); \
  }

  //s = -dInfinity;
  s = -math::infinity;//-dInfinity;
  invert_normal = 0;
  code = 0;

  // separating axis = cylinder ax u2
 //used when a box vertex touches a flat face of the cylinder
  TEST (pp[1],(hlz + B1*Q21 + B2*Q22 + B3*Q23),R1j,0);


  // separating axis = box axis v1,v2,v3
  //used when cylinder edge touches box face
  //there is two ways to compute sQ: sQ21=dSqrt(1.f-Q21*Q21); or sQ21=dSqrt(Q23*Q23+Q22*Q22); 
  //if we did not need Q23 and Q22 the first way might be used to quiken the routine but then it need to 
  //check if Q21<=1.f, becouse it may slightly exeed 1.f.

 
  sQ21=sqrt(Q23*Q23+Q22*Q22);
  TEST ((R2i|p),(radius*sQ21 + hlz*Q21 + B1),R2i,1);

  sQ22=sqrt(Q23*Q23+Q21*Q21);
  TEST ((R2j|p),(radius*sQ22 + hlz*Q22 + B2),R2j,2);

  sQ23=sqrt(Q22*Q22+Q21*Q21);
  TEST ((R2k|p),(radius*sQ23 + hlz*Q23 + B3),R2k,3);

 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = abs(expr1) - (expr2); \
  if (s2 > 0) return ; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      code = (cc); \
    } 
 


// separating axis is a normal to the cylinder axis passing across the nearest box vertex
//used when a box vertex touches the lateral surface of the cylinder

float proj,boxProj,_cos,_sin,cos1,cos3;
float3 tAx,Ax,pb;
{
//making Ax which is perpendicular to cyl ax to box position//
//proj=(p2|R1.lines[1])-(p1|R1.lines[1]);

proj = (p2-p1)|R1j;

//Ax[0]=p2[0]-p1[0]-R1.lines[1][0]*proj;
//Ax[1]=p2[1]-p1[1]-R1.lines[1][1]*proj;
//Ax[2]=p2[2]-p1[2]-R1.lines[1][2]*proj;

Ax = p2 - p1 - R1j * proj;

//Ax.normalize();//no need normalize sign determine!

//dNormalize3(Ax);
//using Ax find box vertex which is nearest to the cylinder axis
	float sign;
    
    //for (i=0; i<3; ++i) pb[i] = p2[i];
	pb = p2;
	sign = ((Ax|R2i) > 0) ? (-1.0f) : (1.0f);
	//for (i=0; i<3; ++i) pb[i] += sign * B1 * R2i[i];//[i*4];
	pb+=sign * B1 * R2i;
	sign = ((Ax|R2j) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) pb[i] += sign * B2 * R2j[i];//R2[i*4+1];
	pb += sign * B2 * R2j;
    sign = ((Ax|R2k) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) pb[i] += sign * B3 * R2k[i];//R2[i*4+2];
	pb += sign * B3 * R2k;

//building axis which is normal to cylinder ax to the nearest box vertex



//proj=(pb|R1.lines[1].xyz())-(p1|R1.lines[1].xyz());
proj = (pb-p1)|R1j;

//Ax[0]=pb[0]-p1[0]-R1[1]*proj;
//Ax[1]=pb[1]-p1[1]-R1[5]*proj;
//Ax[2]=pb[2]-p1[2]-R1[9]*proj;
Ax = pb - p1 - R1j * proj;
//dNormalize3(Ax);
Ax.normalize();
}

boxProj=abs((Ax|R2i)*B1)+
		abs((Ax|R2j)*B2)+
		abs((Ax|R2k)*B3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(radius+boxProj),Ax[0],Ax[1],Ax[2],4);


//next three test used to handle collisions between cylinder circles and box ages
//proj=(p1|R2.lines[0])-(p2|R2.lines[0]);
proj=(p1-p2)|R2i;

//tAx[0]=-p1[0]+p2[0]+R2[0]*proj;
//tAx[1]=-p1[1]+p2[1]+R2[4]*proj;
//tAx[2]=-p1[2]+p2[2]+R2[8]*proj;
tAx = -p1 + p2 + R2i * proj;
//dNormalize3(tAx);
tAx.normalize();

//now tAx is normal to first ax of the box to cylinder center
//making perpendicular to tAx lying in the plane which is normal to the cylinder axis
//it is tangent in the point where projection of tAx on cylinder's ring intersect edge circle

//_cos=dDOT14(tAx,R1+0);
//_sin=dDOT14(tAx,R1+2);

_cos=(tAx|R1i);
_sin=(tAx|R1k);

//tAx[0]=R1[2]*_cos-R1[0]*_sin;
//tAx[1]=R1[6]*_cos-R1[4]*_sin;
//tAx[2]=R1[10]*_cos-R1[8]*_sin;

tAx = R1k * _cos - R1i * _sin;

//use cross between tAx and first ax of the box as separating axix 

//dCROSS114(Ax,=,tAx,R2+0);
Ax = tAx ^ R2i;

float Axmag = Ax.magnitude();
if(Axmag>math::epsilon_7)
{
Ax *= 1.f/Axmag;

//dNormalize3(Ax);

boxProj=abs((Ax|R2j)*B2)+
		abs((Ax|R2i)*B1)+
		abs((Ax|R2k)*B3);

  _cos=abs((Ax|R1j));
  cos1=(Ax|R1i);
  cos3=(Ax|R1k);
  _sin=sqrt(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(_sin*radius+_cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],5);
}

//same thing with the second axis of the box
proj=(p1-p2)|R2j;

//tAx[0]=-p1[0]+p2[0]+R2[1]*proj;
//tAx[1]=-p1[1]+p2[1]+R2[5]*proj;
//tAx[2]=-p1[2]+p2[2]+R2[9]*proj;

tAx = -p1 + p2 + R2j * proj;
//dNormalize3(tAx);
float tAxmag = tAx.magnitude();
if( tAxmag > math::epsilon_7 )
{
	//tAx.normalize();
	tAx *= 1.f/tAxmag;

	_cos=(tAx|R1i);
	_sin=(tAx|R1k);
	//tAx[0]=R1[2]*_cos-R1[0]*_sin;
	//tAx[1]=R1[6]*_cos-R1[4]*_sin;
	//tAx[2]=R1[10]*_cos-R1[8]*_sin;
	tAx = R1k*_cos - R1i*_sin;

	//dCROSS114(Ax,=,tAx,R2+1);
	Ax = tAx^R2j;
	Ax.normalize();
	//dNormalize3(Ax);

	boxProj=abs((Ax|R2i)*B1)+
			abs((Ax|R2j)*B2)+
			abs((Ax|R2k)*B3);

	  _cos=abs((Ax|R1j));
	  cos1=(Ax|R1i);
	  cos3=(Ax|R1k);
	  _sin=sqrt(cos1*cos1+cos3*cos3);
	TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(_sin*radius+_cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],6);
}
//same thing with the third axis of the box
//proj=dDOT14(p1,R2+2)-dDOT14(p2,R2+2);
proj=(p1-p2)|R2k;

Ax = -p1 + p2 + R2k*proj;
//Ax[0]=-p1[0]+p2[0]+R2[2]*proj;
//Ax[1]=-p1[1]+p2[1]+R2[6]*proj;
//Ax[2]=-p1[2]+p2[2]+R2[10]*proj;
tAxmag = tAx.magnitude();
if( tAxmag > math::epsilon_7 )
{
 tAx *= 1.f/tAxmag;

//tAx.normalize();
//dNormalize3(tAx);

_cos=(tAx|R1i);
_sin=(tAx|R1k);
//tAx[0]=R1[2]*_cos-R1[0]*_sin;
//tAx[1]=R1[6]*_cos-R1[4]*_sin;
//tAx[2]=R1[10]*_cos-R1[8]*_sin;
tAx = R1k * _cos - R1i  * _sin;

//dCROSS114(Ax,=,tAx,R2+2);
 
Ax = tAx ^ R2k;
//dNormalize3(Ax);
Ax.normalize();

boxProj=abs((Ax|R2j)*B2)+
		abs((Ax|R2k)*B3)+
		abs((Ax|R2i)*B1);

  _cos=abs((Ax|R1j));
  cos1=(Ax|R1i);
  cos3=(Ax|R1k);
  _sin=sqrt(cos1*cos1+cos3*cos3);
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],(_sin*radius+_cos*hlz+boxProj),Ax[0],Ax[1],Ax[2],7);


#undef TEST
}
// note: cross product axes need to be scaled when s is computed.
// normal (n1,n2,n3) is relative to box 1.

#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = abs(expr1) - (expr2); \
  if (s2 > 0) return ; \
  l = sqrt ((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
  if (l > 0) { \
    s2 /= l; \
    if (s2 > s) { \
      s = s2; \
      normalR = 0; \
      normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
      invert_normal = ((expr1) < 0); \
      code = (cc); \
    } \
  }

//crosses between cylinder axis and box axes
  // separating axis = u2 x (v1,v2,v3)
  TEST(pp[0]*R31-pp[2]*R11,(radius+B2*Q23+B3*Q22),R31,0,-R11,8);
  TEST(pp[0]*R32-pp[2]*R12,(radius+B1*Q23+B3*Q21),R32,0,-R12,9);
  TEST(pp[0]*R33-pp[2]*R13,(radius+B1*Q22+B2*Q21),R33,0,-R13,10);


#undef TEST

  // if we get to this point, the boxes interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    //normal[0] = normalR[0];
    //normal[1] = normalR[4];
    //normal[2] = normalR[8];
	normal = *normalR;
  }
  else {
	  if(code>7) normal = normalC * R1; //dMULTIPLY0_331 (normal,R1,normalC);
	  else {normal[0] =normalC[0];normal[1] = normalC[1];normal[2] = normalC[2];}
  }

  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
  depth = -s;

  // compute contact point(s)

  if (code > 7) {
 //find point on the cylinder pa deepest along normal
    float3 pa;
    float sign, cos1,cos3,factor;


   // for (i=0; i<3; ++i) pa[i] = p1[i];
	pa = p1;

  	cos1 = (normal|R1i);
	cos3 = (normal|R1k) ;
	factor=sqrt(cos1*cos1+cos3*cos3);

	cos1/=factor;
	cos3/=factor;
	
    //for (i=0; i<3; ++i) pa[i] += cos1 * radius * R1[i*4];
	pa += cos1 * radius * R1i;

    sign = ((normal|R1j) > 0) ? (1.0f) : (-1.0f);


    //for (i=0; i<3; ++i) pa[i] += sign * hlz * R1[i*4+1];
	pa += sign * hlz * R1j;

  
    //for (i=0; i<3; ++i) pa[i] += cos3 * radius * R1[i*4+2];
	pa += cos3 * radius * R1k;

    // find vertex of the box  deepest along normal 
    float3 pb;
    //for (i=0; i<3; ++i) pb[i] = p2[i];
	pb = p2;
    sign = ((normal|R2i) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) pb[i] += sign * B1 * R2[i*4];
	pb +=sign * B1 *R2i;
    sign = ((normal|R2j) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) pb[i] += sign * B2 * R2[i*4+1];
	pb += sign * B2 * R2j;
    sign = ((normal|R2k) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) pb[i] += sign * B3 * R2[i*4+2];
	pb += sign * B3 * R2k;

    float alpha,beta;
    float3 ua,ub;
    //for (i=0; i<3; ++i) ua[i] = R1[1 + i*4];
	ua = R1j;
    //for (i=0; i<3; ++i) ub[i] = R2[*code-8 + i*4];
	ub = R2.lines[code-8].xyz();

    lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);

    //for (i=0; i<3; ++i) pa[i] += ua[i]*alpha;
	pa += ua * alpha;
    //for (i=0; i<3; ++i) pb[i] += ub[i]*beta;
	pb += ub * beta;

	
	contact_info ci;
	ci.depth	= depth;
	ci.normal	=  normal;//+-?//replacement/distance;
	ci.position	= (pa + pb) * 0.5f;
	on_c( ci );


  //  for (i=0; i<3; ++i) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
   // contact[0].depth = *depth;
   // return 1;
	return;
  }


  	if(code==4){
		contact_info ci;
		//for (i=0; i<3; ++i) contact[0].pos[i] = pb[i];
		ci.position	= pb;
		//contact[0].depth = *depth;
		ci.normal	=  normal;
		ci.depth = depth;
		on_c( ci );
		//return 1;
				}
  

  float3 vertex;
  if (code == 0) {
   
    float sign;
   // for (i=0; i<3; ++i) vertex[i] = p2[i];
	vertex  = p2;
    sign = ((normal|R2i) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) vertex[i] += sign * B1 * R2[i*4];
	vertex += sign * B1 * R2i;
    sign = ((normal|R2j) > 0) ? (-1.0f) : (1.0f);

    //for (i=0; i<3; ++i) vertex[i] += sign * B2 * R2[i*4+1];
	vertex += sign * B2 *  R2j;
    sign = ((normal|R2k) > 0) ? (-1.0f) : (1.0f);
    //for (i=0; i<3; ++i) vertex[i] += sign * B3 * R2[i*4+2];
	vertex += sign * B3 * R2k;
  }
  else {
   
    float sign,cos1,cos3,factor;
	float3 center;
    cos1 = (normal|R1i) ;
	cos3 = (normal|R1k);
	factor=sqrt(cos1*cos1+cos3*cos3);
	factor= factor ? factor : 1.f;
	cos1/=factor;
	cos3/=factor;
	sign = ((normal|R1j) > 0) ? (1.0f) : (-1.0f);

	//for (i=0; i<3; ++i) center[i] = p1[i]+sign * hlz * R1[i*4+1];
	center = p1 + sign * hlz * R1j;
	//for (i=0; i<3; ++i) vertex[i] = center[i]+cos1 * radius * R1[i*4];
	vertex = center +cos1 * radius * R1i;
	//for (i=0; i<3; ++i) vertex[i] += cos3 * radius * R1[i*4+2];
	vertex += cos3 * radius * R1k;
	if(code<4)
	{
			
			float A1,A3,centerDepth,Q1,Q3,sQ2;


			Q1=Q11;Q3=Q31;sQ2=sQ21;
			int ret=1;
			switch(code) {
			//case 1:
			//	centerDepth=*depth-radius*sQ21;
			//	Q1=Q11;Q3=Q31;
			//	break;
			case 2:
				sQ2=sQ22;
				Q1=Q12;Q3=Q32;
				break;
			case 3:
				sQ2=sQ23;
				Q1=Q13;Q3=Q33;
				break;
			
			}
			contact_info ci[3];

			if(sQ2<M_SQRT1_2)
			{
			
			centerDepth=depth-radius*sQ2;
			A1=(-cos1*M_COS_PI_3-cos3*M_SIN_PI_3)*radius;
			A3=(-cos3*M_COS_PI_3+cos1*M_SIN_PI_3)*radius;


			//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R1[0]+A3*R1[2];
			//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R1[4]+A3*R1[6];
			//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R1[8]+A3*R1[10];
			//CONTACT(contact,ret*skip)->depth=centerDepth+Q1*A1+Q3*A3;
			ci[ret].position = center + A1 * R1i + A3 * R1k;
			ci[ret].depth = centerDepth+Q1*A1+Q3*A3;

			//if(CONTACT(contact,ret*skip)->depth>0.f)++ret;
			if(ci[ret].depth>0.f)
								++ret;

			A1=(-cos1*M_COS_PI_3+cos3*M_SIN_PI_3)*radius;
			A3=(-cos3*M_COS_PI_3-cos1*M_SIN_PI_3)*radius;

			//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R1[0]+A3*R1[2];
			//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R1[4]+A3*R1[6];
			//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R1[8]+A3*R1[10];
			//CONTACT(contact,ret*skip)->depth=centerDepth+Q1*A1+Q3*A3;

			ci[ret].position = center + A1 * R1i + A3 * R1k;
			ci[ret].depth = centerDepth+Q1*A1+Q3*A3;
			if(ci[ret].depth>0.f)
								++ret;
			}


			//for (i=0; i<3; ++i) contact[0].pos[i] = vertex[i];
			//contact[0].depth = *depth;
			ci[0].position = vertex;
			ci[0].depth = depth;
		for(i=0;i<ret;++i)
		{
			ci[i].normal = normal;
			on_c(ci[i]);
		}
		//return ret;
		return;
	}

  }
  //for (i=0; i<3; ++i) contact[0].pos[i] = vertex[i];
  //contact[0].depth = *depth;
  contact_info ci;
  ci.position = vertex;
  ci.normal = normal;
  ci.depth = depth;
  on_c(ci);
  //return 1;
  return;
}

//****************************************************************************

void dCylCyl (on_contact& on_c,
			  const float3 &p1, const float4x4 &RR1,
			const float radius1,const float lz1, const float3 &p2,
			const float4x4 &RR2, const float radius2,const float lz2
			//dVector3 normal, float *depth, int *code,
			//int maxc, dContactGeom *contact, int skip
			)
{
   float3 normal ;
   float depth;
   int code;	

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

  float3 p,pp1,pp2,normalC;
  const float3 *normalR = 0;
  float hlz1,hlz2,s,s2;
  int invert_normal;

  // get vector3 from centers of box 1 to box 2, relative to box 1
  //p[0] = p2[0] - p1[0];
  //p[1] = p2[1] - p1[1];
  //p[2] = p2[2] - p1[2];
  p = p2 - p1;
  const float4x4 invR1 = transpose ( R1 );
  const float4x4 invR2 = transpose ( R2 );
  //dMULTIPLY1_331 (pp1,R1,p);		// get pp1 = p relative to body 1
  pp1 = p * invR1 ;
	
  pp2 = p * invR2 ;
  //dMULTIPLY1_331 (pp2,R2,p);
  // get side lengths / 2

  hlz1 = lz1;//*(0.5f);
  hlz2 = lz2;//*(0.5f); 

 float proj,cos1,cos3;



#define TEST(expr1,expr2,norm,cc) \
  s2 = fabs(expr1) - (expr2); \
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

  float c_cos=abs((R1j|R2j));
  float c_sin=sqrt(1.f-(c_cos>1.f ? 1.f : c_cos));

  TEST (pp1[1],(hlz1 + radius2*c_sin + hlz2*c_cos ),R1j,0);//pp

 /// TEST (pp2[1],(radius1*c_sin + hlz1*c_cos + hlz2),R2+1,1);



  // note: cross product axes need to be scaled when s is computed.
 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = abs(expr1) - (expr2); \
  if (s2 > 0) return ; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      code = (cc); \
    } 
 

float3 tAx,Ax,pa,pb;

//cross between cylinders' axes
//dCROSS144(Ax,=,R1+1,R2+1);
Ax = R1j ^ R2j;
//dNormalize3(Ax);

float Axmag = Ax.magnitude();
if(Axmag > math::epsilon_7)
{
	Ax *= 1.f/Axmag;
	TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius1+radius2,Ax[0],Ax[1],Ax[2],6);
}

{
 
    float sign, factor;

	//making ax which is perpendicular to cyl1 ax passing across cyl2 position//
		//(project p on cyl1 flat surface )
   // for (i=0; i<3; ++i) pb[i] = p2[i];
	pb = p2;

 
	//tAx[0]=pp1[0]*R1[0]+pp1[2]*R1[2];
	//tAx[1]=pp1[0]*R1[4]+pp1[2]*R1[6];
	//tAx[2]=pp1[0]*R1[8]+pp1[2]*R1[10];
	//dNormalize3(tAx);
	tAx = pp1[0] * R1i + pp1[2] * R1k;
	tAx.normalize();


//find deepest point pb of cyl2 on opposite direction of tAx
 	cos1 = (tAx|R2i);
	cos3 = (tAx|R2k) ;
	factor=sqrt(cos1*cos1+cos3*cos3);
	cos1/=factor;
	cos3/=factor;
    //for (i=0; i<3; ++i) pb[i] -= cos1 * radius2 * R2[i*4];
	pb -= cos1 * radius2 * R2i;

    sign = ((tAx|R2j) > 0) ? (1.0f) : (-1.0f);
    //for (i=0; i<3; ++i) pb[i] -= sign * hlz2 * R2[i*4+1];
	pb -= sign * hlz2 * R2j;

    //for (i=0; i<3; ++i) pb[i] -= cos3 * radius2 * R2[i*4+2];
	pb -= cos3 * radius2 * R2k;

//making perpendicular to cyl1 ax passing across pb
	//proj=dDOT14(pb,R1+1)-dDOT14(p1,R1+1);
	proj=(pb-p1)|R1j;
	
	Ax = pb - p1 - R1j * proj;
	//Ax[0]=pb[0]-p1[0]-R1[1]*proj;
	//Ax[1]=pb[1]-p1[1]-R1[5]*proj;
	//Ax[2]=pb[2]-p1[2]-R1[9]*proj;

}

//dNormalize3(Ax);
Ax.normalize();


  float _cos=abs((Ax|R2j));
  cos1=(Ax|R2i);
  cos3=(Ax|R2k);
  float _sin=sqrt(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius1+_cos*hlz2+_sin*radius2,Ax[0],Ax[1],Ax[2],3);



{
   
   float sign, factor;
   	
    //for (i=0; i<3; ++i) pa[i] = p1[i];
   pa = p1;

 	//making ax which is perpendicular to cyl2 ax passing across cyl1 position//
	//(project p on cyl2 flat surface )

	//tAx[0]=pp2[0]*R2[0]+pp2[2]*R2[2];
	//tAx[1]=pp2[0]*R2[4]+pp2[2]*R2[6];
	//tAx[2]=pp2[0]*R2[8]+pp2[2]*R2[10];
	//dNormalize3(tAx);
    tAx = pp2[0] * R2i + pp2[2] * R2k;
    tAx.normalize();
 	cos1 = (tAx|R1i);
	cos3 = (tAx|R1k) ;
	factor=sqrt(cos1*cos1+cos3*cos3);
	cos1/=factor;
	cos3/=factor;

//find deepest point pa of cyl2 on direction of tAx
    //for (i=0; i<3; ++i) pa[i] += cos1 * radius1 * R1[i*4];
	pa += cos1 * radius1 * R1i;

    sign = ((tAx|R1j) > 0) ? (1.0f) : (-1.0f);
    //for (i=0; i<3; ++i) pa[i] += sign * hlz1 * R1[i*4+1];
	pa += sign * hlz1 * R1j;

  
    //for (i=0; i<3; ++i) pa[i] += cos3 * radius1 * R1[i*4+2];
	pa += cos3 * radius1 * R1k;

	//proj=dDOT14(pa,R2+1)-dDOT14(p2,R2+1);
	proj=(pa-p2)|R2j;

	//Ax[0]=pa[0]-p2[0]-R2[1]*proj;
	//Ax[1]=pa[1]-p2[1]-R2[5]*proj;
	//Ax[2]=pa[2]-p2[2]-R2[9]*proj;
	Ax = pa - p2 - R2j * proj;

}
//dNormalize3(Ax);
  Ax.normalize();
  _cos=abs((Ax|R1j));
  cos1=(Ax|R1i);
  cos3=(Ax|R1k);
  _sin=sqrt(cos1*cos1+cos3*cos3);

TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],radius2+_cos*hlz1+_sin*radius1,Ax[0],Ax[1],Ax[2],4);


////test circl

//@ this needed to set right normal when cylinders edges intersect
//@ the most precise axis for this test may be found as a line between nearest points of two
//@ circles. But it needs comparatively a lot of computation.
//@ I use a trick which lets not to solve quadric equation. 
//@ In the case when cylinder eidges touches the test below rather accurate.
//@ I still not sure about problems with sepparation but they have not been revealed during testing.
float3 point;
{
 float3 ca,cb; 
 float sign;
 //for (i=0; i<3; ++i) ca[i] = p1[i];
 ca = p1;
 //for (i=0; i<3; ++i) cb[i] = p2[i];
 cb = p2;
//find two nearest flat rings
 sign = (pp1[1] > 0) ? (1.0f) : (-1.0f);
 //for (i=0; i<3; ++i) ca[i] += sign * hlz1 * R1[i*4+1];
 ca += sign * hlz1 * R1j;

 sign = (pp2[1] > 0) ? (1.0f) : (-1.0f);
 //for (i=0; i<3; ++i) cb[i] -= sign * hlz2 * R2[i*4+1];
 cb -= sign * hlz2 * R2j;

 float3 tAx,tAx1;
 if( circleIntersection(R1j,ca,radius1,R2j,cb,radius2,point) )
 {
	//Ax[0]=point[0]-ca[0];
	//Ax[1]=point[1]-ca[1];
	//Ax[2]=point[2]-ca[2];
	Ax = point - ca;

  	cos1 = (Ax|R1i);
	cos3 = (Ax|R1k) ;

	//tAx[0]=cos3*R1[0]-cos1*R1[2];
	//tAx[1]=cos3*R1[4]-cos1*R1[6];
	//tAx[2]=cos3*R1[8]-cos1*R1[10];
	tAx = cos3 * R1i - cos1 * R1k;

	//Ax[0]=point[0]-cb[0];
	//Ax[1]=point[1]-cb[1];
	//Ax[2]=point[2]-cb[2];
	Ax = point - cb;


 	cos1 = (Ax|R2i);
	cos3 = (Ax|R2k) ;

	//tAx1[0]=cos3*R2[0]-cos1*R2[2];
	//tAx1[1]=cos3*R2[4]-cos1*R2[6];
	//tAx1[2]=cos3*R2[8]-cos1*R2[10];
	tAx1 = cos3 * R2i - cos1*R2k;
	//dCROSS(Ax,=,tAx,tAx1);
	Ax = tAx ^ tAx1;
	

 

//dNormalize3(Ax);
	Ax.normalize();
float cyl1Pr,cyl2Pr;

 _cos=abs((Ax|R1j));
 cos1=(Ax|R1i);
 cos3=(Ax|R1k);
 _sin=sqrt(cos1*cos1+cos3*cos3);
 cyl1Pr=_cos*hlz1+_sin*radius1;

 _cos=abs((Ax|R2j));
 cos1=(Ax|R2i);
 cos3=(Ax|R2k);
 _sin=sqrt(cos1*cos1+cos3*cos3);
 cyl2Pr=_cos*hlz2+_sin*radius2;
TEST(p[0]*Ax[0]+p[1]*Ax[1]+p[2]*Ax[2],cyl1Pr+cyl2Pr,Ax[0],Ax[1],Ax[2],5);
 }

}


#undef TEST



  // if we get to this point, the cylinders interpenetrate. compute the normal
  // in global coordinates.
  if (normalR) {
    //normal[0] = normalR[0];
    //normal[1] = normalR[4];
    //normal[2] = normalR[8];
	 normal = *normalR;
  }
  else {
		normal[0] =normalC[0];normal[1] = normalC[1];normal[2] = normalC[2];
		}
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }

  depth = -s;

  // compute contact point(s)

	if(code==3){

		//for (i=0; i<3; ++i) contact[0].pos[i] = pb[i];
		//contact[0].depth = *depth;
		//return 1;
		contact_info ci;
		ci.position = pb;
		ci.normal = normal;
		ci.depth = depth;
		on_c(ci);
		return;
				}

	if(code==4){
		//for (i=0; i<3; ++i) contact[0].pos[i] = pa[i];
		//contact[0].depth = *depth;
		//return 1;
		contact_info ci;
		ci.position = pa;
		ci.normal = normal;
		ci.depth = depth;
		on_c(ci);
		return;
				}

	if(code==5){
		//for (i=0; i<3; ++i) contact[0].pos[i] = point[i];
		//contact[0].depth = *depth;
		//return 1;
		contact_info ci;
		ci.position = point;
		ci.normal = normal;
		ci.depth = depth;
		on_c(ci);
		return;
				}

if (code == 6) {
	    float3 pa;
    float sign, cos1,cos3,factor;


    //for (i=0; i<3; ++i) pa[i] = p1[i];
	pa = p1;

  	cos1 = (normal|R1i);
	cos3 = (normal|R1k) ;
	factor=sqrt(cos1*cos1+cos3*cos3);
	if(factor>0.f)
	{
		cos1/=factor;
		cos3/=factor;
	}
    //for (i=0; i<3; ++i) pa[i] += cos1 * radius1 * R1[i*4];
	pa += cos1 * radius1 * R1i;

    sign = ((normal|R1j) > 0) ? (1.0f) : (-1.0f);
    //for (i=0; i<3; ++i) pa[i] += sign * hlz1 * R1[i*4+1];
	pa += sign * hlz1 * R1j;

  
    //for (i=0; i<3; ++i) pa[i] += cos3 * radius1 * R1[i*4+2];
	pa += cos3 * radius1 * R1k;

    // find a point pb on the intersecting edge of cylinder 2
    float3 pb;
    //for (i=0; i<3; ++i) pb[i] = p2[i];
	pb = p2;
 	cos1 = (normal|R2i);
	cos3 = (normal|R2k) ;
	factor=sqrt(cos1*cos1+cos3*cos3);
	if(factor>0.f)
	{
		cos1/=factor;
		cos3/=factor;
	}
    //for (i=0; i<3; ++i) pb[i] -= cos1 * radius2 * R2[i*4];
	pb -= cos1 * radius2 * R2i;

    sign = ((normal|R2j) > 0) ? (1.0f) : (-1.0f);
    //for (i=0; i<3; ++i) pb[i] -= sign * hlz2 * R2[i*4+1];
	pb -= sign * hlz2 * R2j;

  
    //for (i=0; i<3; ++i) pb[i] -= cos3 * radius2 * R2[i*4+2];
	pb -= cos3 * radius2 * R2k;

	
	float alpha,beta;
	float3 ua,ub;
	//for (i=0; i<3; ++i) ua[i] = R1[1 + i*4];
	//for (i=0; i<3; ++i) ub[i] = R2[1 + i*4];
	ua = R1j;
	ub = R2j;

	lineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
	//for (i=0; i<3; ++i) pa[i] += ua[i]*alpha;
	pa += ua * alpha;
	//for (i=0; i<3; ++i) pb[i] += ub[i]*beta;
	pb += ub * beta;

    //for (i=0; i<3; ++i) contact[0].pos[i] = REAL(0.5)*(pa[i]+pb[i]);
    //contact[0].depth = *depth;
    //return 1;
	contact_info ci;
	ci.position = 0.5f*(pa+pb);
	ci.depth = depth;
	ci.normal = normal;
	on_c( ci );
	return;
  }

  // okay, we have a face-something intersection (because the separating
  // axis is perpendicular to a face).

  // @@@ temporary: make deepest point on the "other" cylinder the contact point.
  // @@@ this kind of works, but we need multiple contact points for stability,
  // @@@ especially for face-face contact.
  
  float3 vertex;
  int ret=1;
  contact_info ci[3];
  if (code == 0) {

    // flat face from cylinder 1 touches a edge/face from cylinder 2.
    float sign,cos1,cos3,factor;
   // for (i=0; i<3; ++i) vertex[i] = p2[i];
    cos1 = (normal|R2i) ;
	cos3 = (normal|R2k);
	factor=sqrt(cos1*cos1+cos3*cos3);
	if(factor>0.f)
	{
		cos1/=factor;
		cos3/=factor;
	}
	float3 center;

	sign = ((normal|R2j) > 0) ? (1.0f) : (-1.0f);
	//for (i=0; i<3; ++i) center[i] =p2[i]- sign * hlz2 * R2[i*4+1];
	center = p2 - sign * hlz2 * R2j;

    //for (i=0; i<3; ++i) vertex[i] =center[i]- cos1 * radius2 * R2[i*4];
	vertex = center - cos1 * radius2 * R2i;

    //for (i=0; i<3; ++i) vertex[i] -=cos3 * radius2 * R2[i*4+2];
	vertex -= cos3 * radius2 * R2k;

	float A1,A3,centerDepth,Q1,Q3;
	centerDepth=depth-radius2*(factor);
	Q1=-((normal|R2i));Q3=-((normal|R2k));

	A1=-(-cos1*M_COS_PI_3-cos3*M_SIN_PI_3)*radius2;
	A3=-(-cos3*M_COS_PI_3+cos1*M_SIN_PI_3)*radius2;

	

	//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R2[0]+A3*R2[2];
	//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R2[4]+A3*R2[6];
	//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R2[8]+A3*R2[10];
	//CONTACT(contact,ret*skip)->depth=centerDepth+(Q1*A1)+(Q3*A3);
	ci[ret].position = center +A1*R2i+A3*R2k;
	ci[ret].depth = centerDepth+(Q1*A1)+(Q3*A3);
	ci[ret].normal = normal;

	//if(CONTACT(contact,ret*skip)->depth>0.f)++ret;
	if( ci[ret].depth > 0.f ) 
		++ret;

	A1=-(-cos1*M_COS_PI_3+cos3*M_SIN_PI_3)*radius2;
	A3=-(-cos3*M_COS_PI_3-cos1*M_SIN_PI_3)*radius2;

	//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R2[0]+A3*R2[2];
	//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R2[4]+A3*R2[6];
	//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R2[8]+A3*R2[10];
	//CONTACT(contact,ret*skip)->depth=centerDepth+(Q1*A1)+(Q3*A3);

	ci[ret].position = center +A1*R2i+A3*R2k;
	ci[ret].depth = centerDepth+(Q1*A1)+(Q3*A3);
	ci[ret].normal = normal;

	if( ci[ret].depth > 0.f ) 
		++ret;
	//if(CONTACT(contact,ret*skip)->depth>0.f)++ret;

  }
  else {
     // flat face from cylinder 2 touches a edge/face from cylinder 1.
    float sign,cos1,cos3,factor;
   // for (i=0; i<3; ++i) vertex[i] = p1[i];
    cos1 = (normal|R1i) ;
	cos3 = (normal|R1k);
	factor=sqrt(cos1*cos1+cos3*cos3);
	if(factor>0.f)
	{
		cos1/=factor;
		cos3/=factor;
	}

	float3 center;

	sign = ((normal|R1j) > 0) ? (1.0f) : (-1.0f);
	//for (i=0; i<3; ++i) center[i] =p1[i]+sign * hlz1 * R1[i*4+1];
	center = p1 + sign * hlz1 * R1j;


    //for (i=0; i<3; ++i) vertex[i] =center[i]+cos1 * radius1 * R1[i*4];
	vertex = center+cos1 * radius1 * R1i;
    //for (i=0; i<3; ++i) vertex[i] += cos3 * radius1 * R1[i*4+2];
	vertex += cos3 * radius1 * R1k;

	float A1,A3,centerDepth,Q1,Q3;
	centerDepth=depth-radius1*(factor);

	//Q1=(dDOT(R2+1,R1+0));Q3=(dDOT(R2+1,R1+2));
	Q1=((invR2.lines[1].xyz()|invR1.lines[0].xyz()));Q3=((invR2.lines[1].xyz()|invR1.lines[2].xyz()));


	A1=(-cos1*M_COS_PI_3-cos3*M_SIN_PI_3)*radius1;
	A3=(-cos3*M_COS_PI_3+cos1*M_SIN_PI_3)*radius1;
	//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R1[0]+A3*R1[2];
	//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R1[4]+A3*R1[6];
	//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R1[8]+A3*R1[10];
	//CONTACT(contact,ret*skip)->depth=centerDepth+dFabs(Q1*A1)+dFabs(Q3*A3);
	ci[ret].position = center+A1*R1i+A3*R1k;
	ci[ret].depth	 = centerDepth+abs(Q1*A1)+abs(Q3*A3);

	//if(CONTACT(contact,ret*skip)->depth>0.f)++ret;
	if( ci[ret].depth > 0.f )
		++ret;

	A1=(-cos1*M_COS_PI_3+cos3*M_SIN_PI_3)*radius1;
	A3=(-cos3*M_COS_PI_3-cos1*M_SIN_PI_3)*radius1;
	//CONTACT(contact,ret*skip)->pos[0]=center[0]+A1*R1[0]+A3*R1[2];
	//CONTACT(contact,ret*skip)->pos[1]=center[1]+A1*R1[4]+A3*R1[6];
	//CONTACT(contact,ret*skip)->pos[2]=center[2]+A1*R1[8]+A3*R1[10];
	//CONTACT(contact,ret*skip)->depth=centerDepth+dFabs(Q1*A1)+dFabs(Q3*A3);
	ci[ret].position = center+A1*R1i+A3*R1k;
	ci[ret].depth	 = centerDepth+abs(Q1*A1)+abs(Q3*A3);

	//if(CONTACT(contact,ret*skip)->depth>0.f)++ret;
	if( ci[ret].depth > 0.f )
		++ret;

  }
  //for (i=0; i<3; ++i) contact[0].pos[i] = vertex[i];
  ci[0].position = vertex;
  ci[0].depth = depth;
  for(int i = 0; i < ret; ++i )
  {	  ci[i].normal = normal;
	  on_c(ci[i]);
  }
  //return ret;
  return;
}

//#pragma todo(optimize factor==0.f)
//****************************************************************************


void dCollideCylS (
		on_contact& on_c,
		const float3 &p1,
		const float4x4 &RR,
		const float hl,
		const float cylRadius,
		const float3 &p2,
		const float sphereRadius

		//dxGeom *o1, dxGeom *o2, int flags,
		//dContactGeom *contact, int skip
		)
{
 	
	float4x4 R = RR;
	R.c.xyz().set(0,0,0);

	
	const float3	&Ri = R.lines[0].xyz();
	const float3	&Rj = R.lines[1].xyz();
	const float3	&Rk = R.lines[2].xyz();



//  VERIFY (skip >= (int)sizeof(dContactGeom));
// VERIFY (dGeomGetClass(o2) == dSphereClass);
//  VERIFY (dGeomGetClass(o1) == dCylinderClassUser);
//  const float* p1=dGeomGetPosition(o1);
 // const float* p2=dGeomGetPosition(o2);
 // const float* R=dGeomGetRotation(o1);
  float3 p,normalC,normal;
  const float3 *normalR = 0;
//  float cylRadius;
//  float hl;
 // dGeomCylinderGetParams(o1,&cylRadius,&hl);
 // hl/=2.f;
 // float sphereRadius;
 // sphereRadius=dGeomSphereGetRadius(o2);
  
  int invert_normal;

  // get vector3 from centers of cyl to shere
  //p[0] = p2[0] - p1[0];
  //p[1] = p2[1] - p1[1];
  //p[2] = p2[2] - p1[2];
 p = p2 - p1;

float s,s2;
unsigned char code;
#define TEST(expr1,expr2,norm,cc) \
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

  // separating axis cyl ax 



  TEST ((p|Rj),sphereRadius+hl,Rj,2);
  // note: cross product axes need to be scaled when s is computed.
  // normal (n1,n2,n3) is relative to 
#undef TEST
#define TEST(expr1,expr2,n1,n2,n3,cc) \
  s2 = abs(expr1) - (expr2); \
  if (s2 > 0) return ; \
  if (s2 > s) { \
      s = s2; \
	  normalR = 0; \
      normalC[0] = (n1); normalC[1] = (n2); normalC[2] = (n3); \
      invert_normal = ((expr1) < 0); \
      code = (cc); \
    } 
 
//making ax which is perpendicular to cyl1 ax to sphere center//
 
float proj,_cos,_sin,cos1,cos3;
float3 Ax;
//	proj=dDOT14(p2,R+1)-dDOT14(p1,R+1);
proj = (p2-p1)| Rj;

	//Ax[0]=p2[0]-p1[0]-R[1]*proj;
	//Ax[1]=p2[1]-p1[1]-R[5]*proj;
	//Ax[2]=p2[2]-p1[2]-R[9]*proj;
Ax = p2 - p1 - Rj * proj;
float Axmag = Ax.magnitude();
if(Axmag>math::epsilon_7)
{
	//Ax.normalize();
	Ax *= 1.f/Axmag;
	//dNormalize3(Ax);
	TEST((p|Ax),sphereRadius+cylRadius,Ax[0],Ax[1],Ax[2],9);
}

//Ax[0]=p[0];
//Ax[1]=p[1];
//Ax[2]=p[2];
Ax = p;
//dNormalize3(Ax);
Ax.normalize();

	float3 pa;
    float sign, factor;
    //for (i=0; i<3; ++i) pa[i] = p1[i];
	pa = p1;

  	cos1 = (Ax|Ri);
	cos3 = (Ax|Rk) ;
	factor = cos1*cos1+cos3*cos3;
	if(factor > math::epsilon_7 )
	{
		factor=sqrt(factor);
		cos1/=factor;
		cos3/=factor;
	}
    //for (i=0; i<3; ++i) pa[i] += cos1 * cylRadius * R[i*4];
	pa += cos1 * cylRadius * Ri;
    sign = ((Ax|Rj) > 0) ? (1.0f) : (-1.0f);
    //for (i=0; i<3; ++i) pa[i] += sign * hl * R[i*4+1];
	pa += sign * hl * Rj;
    //for (i=0; i<3; ++i) pa[i] += cos3 * cylRadius  * R[i*4+2];
	pa += cos3 * cylRadius  * Rk;


//Ax[0]=p2[0]-pa[0];
//Ax[1]=p2[1]-pa[1];
//Ax[2]=p2[2]-pa[2];
	Ax = p2 - pa;
//dNormalize3(Ax);
	Ax.normalize();

 _cos=abs((Ax|Rj));
 cos1=(Ax|Ri);
 cos3=(Ax|Rk);
 _sin=sqrt(cos1*cos1+cos3*cos3);
TEST((p|Ax),sphereRadius+cylRadius*_sin+hl*_cos,Ax[0],Ax[1],Ax[2],14);


#undef TEST

  if (normalR) {
    //normal[0] = normalR[0];
    //normal[1] = normalR[4];
    //normal[2] = normalR[8];
	normal = *normalR;
  }
  else {

	normal[0] = normalC[0];
	normal[1] = normalC[1];
	normal[2] = normalC[2];
		}
  if (invert_normal) {
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
  }
   // compute contact point(s)

//contact->depth=-s;
//contact->normal[0]=-normal[0];
//contact->normal[1]=-normal[1];
//contact->normal[2]=-normal[2];
//contact->g1=const_cast<dxGeom*> (o1);
//contact->g2=const_cast<dxGeom*> (o2);
//contact->pos[0]=p2[0]-normal[0]*sphereRadius;
//contact->pos[1]=p2[1]-normal[1]*sphereRadius;
//contact->pos[2]=p2[2]-normal[2]*sphereRadius;
//return 1;
contact_info ci; 
ci.depth = -s;
ci.normal = normal;
ci.position = p2 - normal * sphereRadius;
on_c(ci);
return;
}

void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const sphere_geometry& og )			const
{
		
	dCollideCylS (
		c,
		self_transform.c.xyz(),
		self_transform,
		m_half_length,
		m_radius,
		transform.c.xyz(),
		og.radius()

		//dxGeom *o1, dxGeom *o2, int flags,
		//dContactGeom *contact, int skip
		);
}
void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )				const 
{
	dCylBox( c, self_transform.c.xyz(), self_transform, radius(), half_length(), transform.c.xyz(), transform, og.half_sides() );
}
void	cylinder_geometry::generate_contacts( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og )		const 
{
	dCylCyl( c, self_transform.c.xyz(), self_transform, radius(), half_length(), transform.c.xyz(), transform, og.radius(), og.half_length() );
}

math::float3 const* cylinder_geometry::vertices	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* cylinder_geometry::indices			( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER (triangle_id);
	NOT_IMPLEMENTED(return 0);
}

//int dCollideCylB (dxGeom *o1, dxGeom *o2, int flags,
//		dContactGeom *contact, int skip)
//{
//  dVector3 normal;
//  float depth;
//  int code;
//  float cylRadius,cylLength;
//  dVector3 boxSides;
//  dGeomCylinderGetParams(o1,&cylRadius,&cylLength);
//  dGeomBoxGetLengths(o2,boxSides);
//  int num = dCylBox(dGeomGetPosition(o1),dGeomGetRotation(o1),cylRadius,cylLength, 
//					dGeomGetPosition(o2),dGeomGetRotation(o2),boxSides,
//					normal,&depth,&code,flags & NUMC_MASK,contact,skip);
//  for (int i=0; i<num; ++i) {
//    CONTACT(contact,i*skip)->normal[0] = -normal[0];
//    CONTACT(contact,i*skip)->normal[1] = -normal[1];
//    CONTACT(contact,i*skip)->normal[2] = -normal[2];
//    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o1);
//    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o2);
//  }
//  return num;
//}

//int dCollideCylCyl (dxGeom *o1, dxGeom *o2, int flags,
//		dContactGeom *contact, int skip)
//{
//  dVector3 normal;
//  float depth;
//  int code;
//float cylRadius1,cylRadius2;
//float cylLength1,cylLength2;
//dGeomCylinderGetParams(o1,&cylRadius1,&cylLength1);
//dGeomCylinderGetParams(o2,&cylRadius2,&cylLength2);
//int num = dCylCyl (dGeomGetPosition(o1),dGeomGetRotation(o1),cylRadius1,cylLength1,
//				   dGeomGetPosition(o2),dGeomGetRotation(o2),cylRadius2,cylLength2,
//				     normal,&depth,&code,flags & NUMC_MASK,contact,skip);
//
//  for (int i=0; i<num; ++i) {
//    CONTACT(contact,i*skip)->normal[0] = -normal[0];
//    CONTACT(contact,i*skip)->normal[1] = -normal[1];
//    CONTACT(contact,i*skip)->normal[2] = -normal[2];
//    CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o1);
//    CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o2);
//  }
//  return num;
//}








//static  void dCylinderAABB (dxGeom *geom, float aabb[6])
//{
//  float radius,lz;
//  dGeomCylinderGetParams(geom,&radius,&lz);
//const float* R= dGeomGetRotation(geom);
//const float* pos= dGeomGetPosition(geom);
//
//	
//  float xrange = REAL(0.5) * dFabs (R[1] * lz) + (dSqrt(R[0]*R[0]+R[2]*R[2]) * radius);
//
//  float yrange = REAL(0.5) * dFabs (R[5] * lz) + (dSqrt(R[4]*R[4]+R[6]*R[6]) * radius);
//
//  float zrange = REAL(0.5) * dFabs (R[9] * lz) + (dSqrt(R[8]*R[8]+R[10]*R[10]) * radius);
//
//  aabb[0] = pos[0] - xrange;
//  aabb[1] = pos[0] + xrange;
//  aabb[2] = pos[1] - yrange;
//  aabb[3] = pos[1] + yrange;
//  aabb[4] = pos[2] - zrange;
//  aabb[5] = pos[2] + zrange;
//}


















//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace collision
} // namespace xray