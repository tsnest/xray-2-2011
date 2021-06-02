////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_cylinder_contact_tests.h"
#include "cylinder_geometry_instance.h"
namespace xray {
namespace collision {
struct on_contact;
namespace detail{





void accurate_normalize(float3 &v)
{

	ASSERT( v.squared_length() > 0 );
	v.normalize();
}
inline	bool  cylinderCrossesLine(const float3& p,const float3& R, float hlz,
						 const float3 v0,const float3 v1,const float3 l,float3 &pos){
	float _cos=(l|R);

	if(!(abs(_cos)<1.f)) return false;
	
	float sin2=1.f-_cos*_cos;

	float3 vp=v0-p;//{v0[0]-p[0],v0[1]-p[1],v0[2]-p[2]};
    float c1=(vp|l);
	float c2=(vp|R);

	float t=(c2*_cos-c1)/sin2;
	float q=(c2-c1*_cos)/sin2;

	if(abs(q)>hlz) return false;

	float3 v01=v1-v0;//{v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
	float sidelength2=(v01|v01);

	if(t*t>sidelength2) return false;
	
	pos[0]=v0[0]+l[0]*t;
	pos[1]=v0[1]+l[1]*t;
	pos[2]=v0[2]+l[2]*t;

	return true;
	
}

bool circleLineIntersection(const float3& cn,const float3& cp,float r,const float3& lv,const float3& lp,float sign,float3& point)
{

	float3 LC=lp-cp;//{lp[0]-cp[0],lp[1]-cp[1],lp[2]-cp[2]};

	float A,B,C,B_A,B_A_2,D;
	float t1,t2;
	A=(lv|lv);
	B=(LC|lv);
	C=(LC|LC)-r*r;
	B_A=B/A;
	B_A_2=B_A*B_A;
	D=B_A_2-C;
	if(D<0.f){
		
		point[0]=lp[0]-lv[0]*B;
		point[1]=lp[1]-lv[1]*B;
		point[2]=lp[2]-lv[2]*B;
		return false;
		
	}
	else{
	t1=-B_A-sqrt(D);
	t2=-B_A+sqrt(D);

	float3 O1=lp+lv*t1;//{lp[0]+lv[0]*t1,lp[1]+lv[1]*t1,lp[2]+lv[2]*t1};
	float3 O2=lp+lv*t2;//{lp[0]+lv[0]*t2,lp[1]+lv[1]*t2,lp[2]+lv[2]*t2};

	float cpPr=sign*(cn|cp);

	float dist1=(sign*(cn|O1)-cpPr);
	float dist2=(sign*(cn|O2)-cpPr);

	if(dist1<dist2){
					point[0]=O1[0];
					point[1]=O1[1];
					point[2]=O1[2];
					}
	else{			point[0]=O2[0];
					point[1]=O2[1];
					point[2]=O2[2];
					}
					

	return true;
	}
}


void	triangle_cylinder_contact_tests::test( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, float4x4 const& self_transform )
{
	/////////////////////////////////////////////////////////////////////////////////////	
	u32	local_triangle_id	= u32( res_vec.size() );
	triangle_cylinder_res r( local_triangle_id, ind );
	res_vec.push_back( r );
	triangle_cylinder_res &res = res_vec.back();
	const float3	v0 = verts[ res.v0 ] * self_transform, v1 = verts[ res.v1 ] * self_transform, v2 = verts[ res.v2 ] * self_transform;
	const float3	&p	= m_transform.c.xyz();

	const	float3	p_sub_v0	=p-v0;
	base_precalculation( res, v0, v1, v2, p_sub_v0 ); 
	
	triangle_mesh_primitive_contact_tests::test( backs, ind, verts, self_transform );

	if( res.b_in_plane )
	{
		backs.push_back( local_triangle_id );
		return;
	}
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
	 // VERIFY (skip >= (int)sizeof(dContactGeom));
 // VERIFY (dGeomGetClass(o1)== dCylinderClassUser);
  

  
  
  const float4x4 &r_p  = m_transform;

 // const float3 &p=r_p.c.xyz();
  float radius	= m_geometry.radius();
  float hlz		= m_geometry.half_length();
  //dGeomCylinderGetParams(o1,&radius,&hlz);
  //hlz/=2.f;

    // find number of contacts requested
 // int maxc = flags & NUMC_MASK;
 // if (maxc < 1) maxc = 1;
 // if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed

	int maxc = 3;

   float tri_ax_sq_mag = res.tri_ax.squared_length();
  ASSERT( tri_ax_sq_mag > 0.f );

  float tri_ax_mag = sqrt( tri_ax_sq_mag );
   ASSERT( tri_ax_mag > 0.f );
  float tri_ax_rp_mag = 1.f/tri_ax_mag;
  const float3 triAx = res.tri_ax*tri_ax_rp_mag;
  
 // dVector3 triSideAx0={T->side0[0],T->side0[1],T->side0[2]}; //{v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
 // dVector3 triSideAx1={T->side1[0],T->side1[1],T->side1[2]}; //{v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
 // dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};

  float3 triSideAx0=res.side0;//{v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
  float3 triSideAx1=res.side1;//{v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  //dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};
  //dCROSS(triAx,=,triSideAx0,triSideAx1);
  //const float3 side2 = v0 - v2;
  float3 triSideAx2 = v0 - v2;



  //dCROSS(triAx,=,triSideAx0,triSideAx1);
  int code=0;
  float signum, outDepth,cos0,cos1,cos2,sin1;
////////////////////////////////////////////////////////////////////////////
//sepparation along tri plane normal;///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//accurate_normalize(triAx);

//cos0=dDOT14(triAx,R+0);
  cos1=abs((triAx|r_p.lines[1].xyz()));
//cos2=dDOT14(triAx,R+2);

//sin1=_sqrt(cos0*cos0+cos2*cos2);

////////////////////////
//another way //////////
cos1=cos1<(1.f) ? cos1 : (1.f); //cos1 may slightly exeed 1.f
sin1=sqrt((1.f)-cos1*cos1);
//////////////////////////////

float sidePr=cos1*hlz+sin1*radius;

float dist =-res.ax_dist*tri_ax_rp_mag;//-T->dist; //dDOT(triAx,v0)-dDOT(triAx,p);
if(dist>0.f) 
	return;
float depth=sidePr-abs(dist);
outDepth=depth;
signum=dist>0.f ? 1.f : -1.f;

code=0;
if( depth<0.f ) 
	return;

float depth0,depth1,depth2,dist0,dist1,dist2;
bool isPdist0,isPdist1,isPdist2;
bool testV0,testV1,testV2;
bool sideTestV00,sideTestV01,sideTestV02;
bool sideTestV10,sideTestV11,sideTestV12;
bool sideTestV20,sideTestV21,sideTestV22;


//////////////////////////////////////////////////////////////////////////////
//cylinder axis - one of the triangle vertexes touches cylinder's flat surface
//////////////////////////////////////////////////////////////////////////////
dist0=(v0-p)|r_p.lines[1].xyz();
dist1=(v1-p)|r_p.lines[1].xyz();
dist2=(v2-p)|r_p.lines[1].xyz();

isPdist0=dist0>0.f;
isPdist1=dist1>0.f;
isPdist2=dist2>0.f;

depth0=hlz-abs(dist0);
depth1=hlz-abs(dist1);
depth2=hlz-abs(dist2);

testV0=depth0>0.f;
testV1=depth1>0.f;
testV2=depth2>0.f;

if(isPdist0==isPdist1 && isPdist1== isPdist2) //(here and lower) check the tryangle is on one side of the cylinder
   
{
if(depth0>depth1) 
		if(depth0>depth2) 	
			if(testV0){
				if(depth0<outDepth) 
					{
					signum= isPdist0 ? 1.f : -1.f;
					outDepth=depth0;			
					code=1;
					}
				}
			else 
				return;
		else
			if(testV2){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					signum= isPdist2 ? 1.f : -1.f;
					code=3;
					}
			}
			else 
				return;
else
		if(depth1>depth2)
			if(testV1){
				if(depth1<outDepth) 
					{
					outDepth=depth1;
					signum= isPdist1 ? 1.f : -1.f;
					code=2;
					}
			}
			else 
				return;

		else
			if(testV2){
				if(depth2<outDepth) 
					{
					outDepth=depth2;
					signum= isPdist2 ? 1.f : -1.f;
					code=2;
					}
			}
			else return;
}


float3 axis,outAx;
float posProj;
float pointDepth=0.f;


#define TEST(vx,ox1,ox2,c)	\
	{\
	posProj=((v##vx-p)|r_p.lines[1].xyz());\
\
	axis[0]=v##vx[0]-p[0]-r_p.lines[1][0]*posProj;\
	axis[1]=v##vx[1]-p[1]-r_p.lines[1][1]*posProj;\
	axis[2]=v##vx[2]-p[2]-r_p.lines[1][2]*posProj;\
\
	accurate_normalize(axis);\
\
\
	dist0=(v0-p)|axis;\
	dist1=(v1-p)|axis;\
	dist2=(v2-p)|axis;\
\
	isPdist0=dist0>0.f;\
	isPdist1=dist1>0.f;\
	isPdist2=dist2>0.f;\
\
	depth0=radius-abs(dist0);\
	depth1=radius-abs(dist1);\
	depth2=radius-abs(dist2);\
\
	sideTestV##vx##0=depth0>0.f;\
	sideTestV##vx##1=depth1>0.f;\
	sideTestV##vx##2=depth2>0.f;\
\
	if(isPdist0==isPdist1 && isPdist1== isPdist2)\
\
	{\
	if(sideTestV##vx##0||sideTestV##vx##1||sideTestV##vx##2){\
	if(!(depth##vx<depth##ox1 || depth##vx<depth##ox2))\
					{\
						if(depth##vx<outDepth && depth##vx > pointDepth)\
							{\
							pointDepth=depth##vx;\
							signum= isPdist##vx ? 1.f : -1.f;\
							outAx[0]=axis[0];\
							outAx[1]=axis[1];\
							outAx[2]=axis[2];\
							code=c;\
							}\
					}\
	}\
	else return;\
			\
\
\
	}\
}

if(testV0) 
	TEST(0,1,2,4)
if(testV1 )
	TEST(1,2,0,5)
//&& sideTestV01
if(testV2 )
	TEST(2,0,1,6)
//&& sideTestV02 && sideTestV12
#undef TEST

float3 tpos,pos;
if(code>3) 
	outDepth=pointDepth; //deepest vertex axis used if its depth less than outDepth
//else{
//bool outV0=!(testV0&&sideTestV00&&sideTestV10&&sideTestV20);
//bool outV1=!(testV1&&sideTestV01&&sideTestV11&&sideTestV21);
//bool outV2=!(testV2&&sideTestV02&&sideTestV12&&sideTestV22);
bool outV0=true;
bool outV1=true;
bool outV2=true;
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///crosses between triangle sides and cylinder axis//////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define TEST(ax,nx,ox,c)	if(cylinderCrossesLine(p,r_p.lines[1].xyz(),hlz,v##ax,v##nx,triSideAx##ax,tpos))	{\
	axis=triSideAx##ax^r_p.lines[1].xyz();\
	accurate_normalize(axis);\
	dist##ax=(v##ax|axis)-(p|axis);\
	dist##ox=(v##ox|axis)-(p|axis);\
\
	isPdist##ax=dist##ax>0.f;\
	isPdist##ox=dist##ox>0.f;\
\
	if(isPdist##ax == isPdist##ox)\
{\
depth##ax=radius-abs(dist##ax);\
depth##ox=radius-abs(dist##ox);\
	\
			if(depth##ax>0.f){\
				if(depth##ax<=outDepth && depth##ax>=depth##ox) \
					{\
						outDepth=depth##ax;\
						signum= isPdist##ax ? 1.f : -1.f;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						pos[0]=tpos[0];\
						pos[1]=tpos[1];\
						pos[2]=tpos[2];\
						code=c;\
					}\
				}\
			else if(depth##ox<0.f) return;\
\
}\
}

accurate_normalize(triSideAx0);
if(outV0&&outV1) 
TEST(0,1,2,7)

accurate_normalize(triSideAx1);
if(outV1&&outV2) 
TEST(1,2,0,8)

accurate_normalize(triSideAx2);
if(outV2&&outV0) 
TEST(2,0,1,9)
#undef TEST

////////////////////////////////////
//test cylinder rings on triangle sides////
////////////////////////////////////

float3 tAx,cen;
float sign;
bool cs;

#define TEST(ax,nx,ox,c)	\
{\
posProj=(p-v##ax)|triSideAx##ax;\
axis[0]=p[0]-v0[0]-triSideAx##ax[0]*posProj;\
axis[1]=p[1]-v0[1]-triSideAx##ax[1]*posProj;\
axis[2]=p[2]-v0[2]-triSideAx##ax[2]*posProj;\
	\
sign=(axis|(r_p.lines[1].xyz()))>0.f ? 1.f :-1.f;\
cen[0]=p[0]-sign*r_p.lines[1][0]*hlz;\
cen[1]=p[1]-sign*r_p.lines[1][1]*hlz;\
cen[2]=p[2]-sign*r_p.lines[1][2]*hlz;\
\
cs=circleLineIntersection((r_p.lines[1].xyz()),cen,radius,triSideAx##ax,v##ax,-sign,tpos);\
\
axis[0]=tpos[0]-cen[0];\
axis[1]=tpos[1]-cen[1];\
axis[2]=tpos[2]-cen[2];\
\
if(cs){ \
\
cos0=(axis|r_p.lines[0].xyz());\
cos2=(axis|r_p.lines[2].xyz());\
tAx[0]=r_p.lines[2][0]*cos0-r_p.lines[0][0]*cos2;\
tAx[1]=r_p.lines[2][1]*cos0-r_p.lines[0][1]*cos2;\
tAx[2]=r_p.lines[2][2]*cos0-r_p.lines[0][2]*cos2;\
\
axis=triSideAx##ax^tAx;\
\
}\
accurate_normalize(axis);\
dist##ax=(v##ax-p)|axis;\
if(dist##ax*(axis|triSideAx##nx)>0.f){\
\
cos0=(axis|r_p.lines[0].xyz());\
cos1=abs((axis|r_p.lines[1].xyz()));\
cos2=(axis|r_p.lines[2].xyz());\
\
\
sin1=sqrt(cos0*cos0+cos2*cos2);\
\
sidePr=cos1*hlz+sin1*radius;\
\
\
	dist##ox=(v##ox-p)|axis;\
\
	isPdist##ax=dist##ax>0.f;\
	isPdist##ox=dist##ox>0.f;\
\
	if(isPdist##ax == isPdist##ox) \
\
{\
depth##ax=sidePr-abs(dist##ax);\
depth##ox=sidePr-abs(dist##ox);\
	\
			if(depth##ax>0.f){\
				if(depth##ax<outDepth) \
					{\
						outDepth=depth##ax;\
						signum= isPdist##ax ? 1.f : -1.f;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						pos[0]=tpos[0];\
						pos[1]=tpos[1];\
						pos[2]=tpos[2];\
						code=c;\
					}\
				}\
			else if(depth##ox<0.f) return;\
\
\
}\
}\
}

if(7!=code)
	TEST(0,1,2,10)

if(8!=code)
	TEST(1,2,0,11)

if(9!=code)
	TEST(2,0,1,12)

#undef TEST

//}
//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches cylinder///////////////////////
/////////////////////////////////////////////////////////////////////
//VERIFY( g_pGameLevel );
//CDB::TRI*       T_array      = inl_ph_world().ObjectSpace().GetStaticTris();
float3 norm;
unsigned int ret;
//flags8& gl_state=gl_cl_tries_state[_I-_B];
  const float3	&x_ax  = r_p.lines[0].xyz();
  const float3	&y_ax  = r_p.lines[1].xyz();
  const float3	&z_ax  = r_p.lines[2].xyz();
  contact_info	cv[4];

if(code==0){
	norm[0]=triAx[0]*signum;
	norm[1]=triAx[1]*signum;
	norm[2]=triAx[2]*signum;


  float Q1 = (norm|r_p.lines[0].xyz());
  float Q2 = (norm|r_p.lines[1].xyz());
  float Q3 = (norm|r_p.lines[2].xyz());
  float factor =sqrt(Q1*Q1+Q3*Q3);
  float	C1,C3;
  float centerDepth;//depth in the cirle centre
  if( factor>0.f )
  {
  		C1=Q1/factor;
		C3=Q3/factor;
		
  }
  else
  {
		C1=1.f;
		C3=0.f;
	
  }
  
  float A1 = radius *		C1;//cosinus
  float A2 = hlz;//Q2
  float A3 = radius *		C3;//sinus 
	
  if( factor>0.f ) 
	  centerDepth=outDepth-A1*Q1-A3*Q3; 
  else 
	  centerDepth=outDepth;

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];
 
  pos[0]+= Q2>0 ? hlz*r_p.lines[1][0]:-hlz*r_p.lines[1][0];
  pos[1]+= Q2>0 ? hlz*r_p.lines[1][1]:-hlz*r_p.lines[1][1];
  pos[2]+= Q2>0 ? hlz*r_p.lines[1][2]:-hlz*r_p.lines[1][2];

  
  
  
  ret=0;
  float3 cross0, cross1, cross2;
  float ds0,ds1,ds2;
  
  cross0=triAx^triSideAx0;
  ds0=cross0|v0;

  cross1=triAx^triSideAx1;
  ds1=cross1|v1;

  cross2 =triAx^triSideAx2;
  ds2=cross2|v2;



 // contact_info	*contact_info = cv[0];
  cv[0].position[0] = pos[0]+A1*r_p.lines[0][0]+A3*r_p.lines[2][0];
  cv[0].position[1] = pos[1]+A1*r_p.lines[0][1]+A3*r_p.lines[2][1];
  cv[0].position[2] = pos[2]+A1*r_p.lines[0][2]+A3*r_p.lines[2][2];

  if((cross0|cv[0].position)-ds0>0.f && 
	 (cross1|cv[0].position)-ds1>0.f && 
	 (cross2|cv[0].position)-ds2>0.f){
							   cv[0].depth = outDepth;
							   ret=1;
  }
const float M_SQRT1_2 = 1.f/sqrt(2.f);
const float M_SIN_PI_3 = sqrt(3.f)/2.f;
const float M_COS_PI_3 = 1.f/2.f;
  if(abs(Q2)>M_SQRT1_2){
	
  A1=(-C1*M_COS_PI_3-C3*  M_SIN_PI_3)*radius;
  A3=(-C3*M_COS_PI_3+C1*M_SIN_PI_3)*radius;
  (cv[ret]).position[0]=pos[0]+A1*x_ax[0]+A3*z_ax[0];
  (cv[ret]).position[1]=pos[1]+A1*x_ax[1]+A3*z_ax[1];
  (cv[ret]).position[2]=pos[2]+A1*x_ax[2]+A3*z_ax[2];
  (cv[ret]).depth=centerDepth+Q1*A1+Q3*A3;

  if( (cv[ret]).depth>0.f)
    if(( cross0 | cv[ret].position )-ds0>0.f && 
	   ( cross1 | cv[ret].position )-ds1>0.f && 
	   ( cross2 | cv[ret].position )-ds2>0.f ) ++ret;
  
  A1=(-C1*M_COS_PI_3+C3*M_SIN_PI_3)*radius;
  A3=(-C3*M_COS_PI_3-C1*M_SIN_PI_3)*radius;
  cv[ret].position[0]=pos[0]+A1*x_ax[0]+A3*z_ax[0];
  cv[ret].position[1]=pos[1]+A1*x_ax[1]+A3*z_ax[1];
  cv[ret].position[2]=pos[2]+A1*x_ax[2]+A3*z_ax[2];
  cv[ret].depth=centerDepth+Q1*A1+Q3*A3;

  if(cv[ret].depth>0.f)
    if(( cross0 | cv[ret].position )-ds0>0.f && 
	   ( cross1 | cv[ret].position )-ds1>0.f && 
	   ( cross2 | cv[ret].position )-ds2>0.f) ++ret;
} else {

  cv[ret].position[0]=cv[0].position[0]-2.f*(Q2>0 ? hlz*y_ax[0]:-hlz*y_ax[0]);
  cv[ret].position[1]=cv[0].position[1]-2.f*(Q2>0 ? hlz*y_ax[1]:-hlz*y_ax[1]);
  cv[ret].position[2]=cv[0].position[2]-2.f*(Q2>0 ? hlz*y_ax[2]:-hlz*y_ax[2]);
  cv[ret].depth=outDepth-abs(Q2*2.f*A2);

  if(cv[ret].depth>0.f)
    if(( cross0 | cv[ret].position )-ds0>0.f && 
	   ( cross1 | cv[ret].position )-ds1>0.f && 
	   ( cross2 | cv[ret].position )-ds2>0.f) ++ret;
}
	}
else if(code<7)//1-6
	{
	ret=1;
	cv[0].depth = outDepth;
	res.r_vertex.use = true;
	switch((code-1)%3){
	case 0:
	//if(gl_state.test(fl_engaged_v0))RETURN0;
	//VxToGlClTriState(T->T->verts[0],T_array);
	//cv[0].position[0]=v0[0];
	//cv[0].position[1]=v0[1];
	//cv[0].position[2]=v0[2];
	cv[0].position = v0;
	res.r_vertex.vertex_id = res.v0;
	if( check_neibour_verts( res_vec, res.r_vertex.vertex_id ) )
		return;
	break;
	case 1:
	//if(gl_state.test(fl_engaged_v1))RETURN0;
	//VxToGlClTriState(T->T->verts[1],T_array);
	//cv[0].position[0]=v1[0];
	//cv[0].position[1]=v1[1];
	//cv[0].position[2]=v1[2];
	cv[0].position  = v1;
	res.r_vertex.vertex_id = res.v1;
	if( check_neibour_verts( res_vec, res.r_vertex.vertex_id ) )
		return;
	break;
	case 2:
	//if(gl_state.test(fl_engaged_v2))
	//								RETURN0;
	//VxToGlClTriState(T->T->verts[2],T_array);
	//cv[0].position[0]=v2[0];
	//cv[0].position[1]=v2[1];
	//cv[0].position[2]=v2[2];
	cv[0].position = v2;
	res.r_vertex.vertex_id = res.v2;
	if( check_neibour_verts( res_vec, res.r_vertex.vertex_id ) )
		return;
	break;
	}

if(code<4){//1-3
	//norm[0]=R[1]*signum;
	//norm[1]=R[5]*signum;
	//norm[2]=R[9]*signum;
	norm = y_ax * signum;
	}
else {
	//norm[0]=outAx[0]*signum;
	//norm[1]=outAx[1]*signum;
	//norm[2]=outAx[2]*signum;
	norm = outAx * signum;
	}
}

else {//7-12
	ret=1;
	res.r_side.use = true;
	const int liv0  = (code-7)%3;
	const int liv1  = (liv0+1)%3;
	res.r_side.vertex0_id=ind[liv0];
	res.r_side.vertex1_id=ind[liv1];
	if( !check_neibour_sides( res_vec,res.r_side.vertex0_id, res.r_side.vertex1_id ) )
		return;
	//int flag=fl_engaged_s0<<(iv0);
	//if(gl_state.test(u8(flag&0xff)))
	//					RETURN0;
	//SideToGlClTriState(T->T->verts[iv0],T->T->verts[iv1],T_array);

	cv[0].depth = outDepth;
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;
	cv[0].position[0] = pos[0];
	cv[0].position[1] = pos[1];
	cv[0].position[2] = pos[2];	
}

	if((int)ret>maxc) ret=(unsigned int)maxc;

 for (unsigned int i=0; i<ret; ++i) {
    //CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
    //CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);

	//CONTACT(contact,i*skip)->normal[0] = norm[0];
	//CONTACT(contact,i*skip)->normal[1] = norm[1];
	//CONTACT(contact,i*skip)->normal[2] = norm[2];
	cv[i].normal = -norm;
	cb( cv[i] );
	//SURFACE(contact,i*skip)->mode=T->T->material;
 }
 // if(ret&&dGeomGetUserData(o1)->callback)dGeomGetUserData(o1)->callback(T->T,contact);
 return ;  






}




}//detail {
}//collision{
}//xray{