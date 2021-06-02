////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
// it is nearly not refactored xray old tests



#include "pch.h"
#include "triangle_box_contact_tests.h"

namespace xray {
namespace collision {
namespace detail{

inline bool	 CrossProjLine14(const float3& pt1,const float3&  vc1,const float3& pt2,const float3&  vc2,float hside, float3&  proj){
	//dVector3 ac={pt1[0]-pt2[0],pt1[1]-pt2[1],pt1[2]-pt2[2]};
	float3 ac =pt1-pt2;

	//dReal vc1_vc2=dDOT41(vc2,vc1);
	float vc1_vc2=(vc2|vc1);
	//dReal vc1_2=dDOT(vc1,vc1);
	float vc1_2=(vc1|vc1);

	float factor=/*vc2_2*/vc1_2-vc1_vc2*vc1_vc2;
	if( factor==0.f ){
		//proj[0]=dInfinity;
		//proj[1]=dInfinity;
		//proj[2]=dInfinity;
		return false;
	}
	//dReal ac_vc1=dDOT(vc1,ac);
	float ac_vc1=(vc1|ac);
	//dReal ac_vc2=dDOT14(ac,vc2);
	float ac_vc2=(ac|vc2);

	float t1=(ac_vc2*vc1_vc2-ac_vc1/*vc2_2*/)
		/
		factor;

	if(t1<0.f) 
		return false;
	if(t1>1.f) 
		return false;

	float t2=(ac_vc1*vc1_vc2-ac_vc2*vc1_2)
		/factor;

	float nt2=t2;//*_sqrt(vc2_2);
	if( nt2 > hside || nt2 < -hside) return false;

	proj[0]=pt1[0]+vc1[0]*t1;
	proj[1]=pt1[1]+vc1[1]*t1;
	proj[2]=pt1[2]+vc1[2]*t1;

	return true;
}
inline bool normalize_if_possible( float3 &v )
{
	float sqr_length = (v|v);
	if( sqr_length < math::epsilon_7 )
		return false;
	float	l	=	1.f/sqrt(sqr_length); //dRecipSqrt(sqr_length);
		v[0]		*=	l;
		v[1]		*=	l;
		v[2]		*=	l;
	return true;
}
void	triangle_box_contact_tests::test( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, float4x4 const& self_transform )
{
/////////////////////////////////////////////////////////////////////////////////////	
	u32	local_triangle_id	= u32( res_vec.size() );
	triangle_box_res r( local_triangle_id, ind );
	res_vec.push_back( r );
	triangle_box_res &res = res_vec.back();
	//const float3	&v0 = verts[ res.v0 ], &v1 = verts[ res.v1 ], &v2 = verts[ res.v2 ];
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
/////////////////NOT REFACTORED//////////////////
/////////////////NOT REFACTORED//////////////////
/////////////////NOT REFACTORED//////////////////
  const float3 &hside = m_geometry.get_matrix().get_scale();
  //dGeomBoxGetLengths(o1,hside);
  //hside[0]/=2.f;hside[1]/=2.f;hside[2]/=2.f;

    // find number of contacts requested
 // int maxc = flags & NUMC_MASK;
 // if (maxc < 1) maxc = 1;
 // if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed

  //dVector3 triAx;
  const float3& triSideAx0=res.side0;//{v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
  const float3& triSideAx1=res.side1;//{v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
  //dVector3 triSideAx2={v0[0]-v2[0],v0[1]-v2[1],v0[2]-v2[2]};
  //dCROSS(triAx,=,triSideAx0,triSideAx1);



  //const float3 side2 = v0 - v2;
  const float3 triSideAx2 = v0 - v2;

  int code=0;
  float outDepth;
  float signum;
  //sepparation along tri plane normal;
  //const dReal *triAx	=T->norm;
  float tri_ax_sq_mag = res.tri_ax.squared_length();
  ASSERT( tri_ax_sq_mag > 0.f );

  float tri_ax_mag = sqrt( tri_ax_sq_mag );
   ASSERT( tri_ax_mag > 0.f );
  float tri_ax_rp_mag = 1.f/tri_ax_mag;
  const float3 triAx = res.tri_ax*tri_ax_rp_mag;

  const float4x4&	r_p = m_transform;
 // const float3&		p	= r_p.c;
 //accurate_normalize(triAx);

 //dReal sidePr=
	//dFabs(dDOT14(triAx,R+0)*hside[0])+
	//dFabs(dDOT14(triAx,R+1)*hside[1])+
	//dFabs(dDOT14(triAx,R+2)*hside[2]);

  float sidePr =	abs( triAx|r_p.i.xyz() ) * hside.x +
					abs( triAx|r_p.j.xyz() ) * hside.y +
					abs( triAx|r_p.k.xyz() ) * hside.z;

 float dist =-res.ax_dist*tri_ax_rp_mag; //-T->dist;
//dist=dDOT(triAx,v0)-dDOT(triAx,p);
float depth=sidePr-abs(dist);
outDepth=depth;

signum=dist<0.f ? -1.f : 1.f;
//signum = -1.f;
code=0;
if( depth<0.f ) 
		return;


bool isPdist0,isPdist1,isPdist2;
bool test0=true,test1=true,test2=true;
bool test00,test01,test02;
bool test10,test11,test12;
bool test20,test21,test22;

float depth0,depth1,depth2;
float dist0,dist1,dist2;



#define CMP(sd,c)	\
if(depth0>depth1)\
		if(depth0>depth2) \
			if(test0##sd){\
			  if(test0)\
				if(depth0<outDepth)\
					{\
					outDepth=depth0;\
					signum=dist0<0.f ? -1.f : 1.f;\
					code=c;\
					}\
			}\
			else return ;\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=dist2<0.f ? -1.f : 1.f;\
					code=c+2;\
					}\
			}\
			else return ;\
else\
		if(depth1>depth2)\
			if(test1##sd){\
			  if(test1)\
				if(depth1<outDepth) \
					{\
					outDepth=depth1;\
					signum=dist1<0.f ? -1.f : 1.f;\
					code=c+1;\
					}\
			}\
			else return ;\
\
		else\
			if(test2##sd){\
			  if(test2)\
				if(depth2<outDepth) \
					{\
					outDepth=depth2;\
					signum=dist2<0.f ? -1.f : 1.f;\
					code=c+2;\
					}\
			}\
			else return ;


#define TEST(sd, c) \
\
dist0=(v0-p)|r_p.lines[sd].xyz();\
dist1=(v1-p)|r_p.lines[sd].xyz();\
dist2=(v2-p)|r_p.lines[sd].xyz();\
\
isPdist0=dist0>0.f;\
isPdist1=dist1>0.f;\
isPdist2=dist2>0.f;\
\
depth0=hside[sd]-abs(dist0);\
depth1=hside[sd]-abs(dist1);\
depth2=hside[sd]-abs(dist2);\
test0##sd = depth0>0.f;\
test1##sd = depth1>0.f;\
test2##sd = depth2>0.f;\
\
test0 =test0 && test0##sd;\
test1 =test1 && test1##sd;\
test2 =test2 && test2##sd;\
\
if(isPdist0==isPdist1 && isPdist1==isPdist2)\
{\
CMP(sd,c)\
}

TEST(0,1)
TEST(1,4)
TEST(2,7)

if( !test0 && !test1 && !test2 )
{
	code = 0;
	outDepth = depth;
}

#undef CMP
#undef TEST

unsigned int i;

float3 axis,outAx;

/*
#define TEST(ax,ox,c) \
for(i=0;i<3;++i){\
	dCROSS114(axis,=,triSideAx##ax,R+i);\
	accurate_normalize(axis);\
	int ix1=(i+1)%3;\
	int ix2=(i+2)%3;\
	sidePr=\
		dFabs(dDOT14(axis,R+ix1)*hside[ix1])+\
		dFabs(dDOT14(axis,R+ix2)*hside[ix2]);\
\
	dist##ax=(dDOT(v##ax,axis)-dDOT(p,axis));\
	dist##ox=(dDOT(v##ox,axis)-dDOT(p,axis));\
			signum=dist##ox<0.f ? -1.f : 1.f;\
\
depth##ax=sidePr-signum*dist##ax;\
depth##ox=sidePr-signum*dist##ox;\
	if(depth##ax<depth##ox){\
			if(depth##ax>0.f){\
			if(depth##ax<outDepth){ \
				if(dDOT(axis,triAx)*signum<0.f)	{\
						outDepth=depth##ax;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						code=c+i;\
					}\
			}\
				}\
			else return 0;\
	}\
}
*/
float3 pos;

#define TEST(ax,ox,c) \
for(i=0;i<3;++i){\
	(axis = triSideAx##ax^r_p.lines[i].xyz());\
	if(!normalize_if_possible(axis)) continue;\
	int ix1=(i+1)%3;\
	int ix2=(i+2)%3;\
	sidePr=\
		abs((axis|r_p.lines[ix1].xyz())*hside[ix1])+\
		abs((axis|r_p.lines[ix2].xyz())*hside[ix2]);\
\
	dist##ax=((v##ax-p)|axis);\
	dist##ox=((v##ox-p)|axis);\
\
isPdist##ax=dist##ax>0.f;\
isPdist##ox=dist##ox>0.f;\
if(isPdist##ax != isPdist##ox) continue;\
\
depth##ax=sidePr-abs(dist##ax);\
depth##ox=sidePr-abs(dist##ox);\
	if(depth##ax>depth##ox){\
			if(depth##ax>0.f){\
				if(depth##ax*1.05f<outDepth) \
					{\
						float sgn=dist##ax<0.f ? -1.f : 1.f;\
						float sgn1=sgn*(axis|r_p.lines[ix1].xyz())<0.f ? -1.f : 1.f;\
						float sgn2=sgn*(axis|r_p.lines[ix2].xyz())<0.f ? -1.f : 1.f;\
						for(int ii=0;ii<3;++ii) crpos[ii]=p[ii]+r_p.lines[ix1][ii]*hside[ix1]*sgn1+r_p.lines[ix2][ii]*hside[ix2]*sgn2;\
						if(CrossProjLine14(v##ax,triSideAx##ax,crpos,r_p.lines[i].xyz(),hside[i],pos))\
						{\
						outDepth=depth##ax;\
						signum=sgn;\
						outAx[0]=axis[0];\
						outAx[1]=axis[1];\
						outAx[2]=axis[2];\
						code=c+i;\
						}\
					}\
				}\
			else return ;\
	}\
}

float3 crpos;
//#define TEST(ax,ox,c) 
//test_cross_side( dReal* outAx, dReal& outDepth, dReal *pos, dReal& outSignum, u8 &code, u8 c,  const dReal* R, const dReal* hside, const dReal* p,  const dReal* triSideAx, const dReal* vax,   const dReal* vox  )
//if( test_cross_side (outAx,outDepth,pos,signum,code,10,R,hside,p,triSideAx0,v0,v2) )
  // return 0;
TEST(0,2,10)
TEST(1,0,13)
TEST(2,1,16)

#undef TEST

//////////////////////////////////////////////////////////////////////
///if we get to this poit tri touches box
float3 norm;
unsigned int ret=1;
contact_info	cv[4];
if(code==0){
	norm[0]=triAx[0]*signum;
	norm[1]=triAx[1]*signum;
	norm[2]=triAx[2]*signum;

/////////////////////////////////////////// from geom.cpp dCollideBP
  float Q1 = -signum*(triAx|r_p.lines[0].xyz());
  float Q2 = -signum*(triAx|r_p.lines[1].xyz());
  float Q3 = -signum*(triAx|r_p.lines[2].xyz());
  float A1 = 2.f*hside[0] * Q1;
  float A2 = 2.f*hside[1] * Q2;
  float A3 = 2.f*hside[2] * Q3;
  float B1 = abs(A1);
  float B2 = abs(A2);
  float B3 = abs(A3);

  pos[0]=p[0];
  pos[1]=p[1];
  pos[2]=p[2];

#define FOO(i,op) \
  pos[0] op hside[i] * r_p.lines[i][0]; \
  pos[1] op hside[i] * r_p.lines[i][1]; \
  pos[2] op hside[i] * r_p.lines[i][2];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
  BAR(0,1);
  BAR(1,2);
  BAR(2,3);
#undef FOO
#undef BAR

///////////////////////////////////////////////////////////


#define TRI_CONTAIN_POINT(pos)	{\
 float3 cross0, cross1, cross2;\
 float ds0,ds1,ds2;\
 \
  cross0=triAx^triSideAx0;\
  ds0=(cross0|v0);\
\
  cross1=triAx^triSideAx1;\
  ds1=(cross1|v1);\
\
  cross2=(triAx^triSideAx2);\
  ds2=(cross2|v2);\
\
  if((cross0|pos)-ds0>0.f && \
	 (cross1|pos)-ds1>0.f && \
	 (cross2|pos)-ds2>0.f) ++ret;\
}
///////////////////////////////////////////////////////////


  // get the second and third contact points by starting from `p' and going
  // along the two sides with the smallest projected length.

float* pdepth;
//dContactGeom* prc,*c=CONTACT(contact,ret*skip);

contact_info	*c = &cv[ret];
contact_info	*prc = &cv[ret];
//prc=c;
#define FOO(j,op,spoint) \
	c->position[0] = spoint##[0] op 2.f*hside[j] * r_p.lines[j][0]; \
	c->position[1] = spoint##[1] op 2.f*hside[j] * r_p.lines[j][1]; \
	c->position[2] = spoint##[2] op 2.f*hside[j] * r_p.lines[j][2];
#define BAR(side,sideinc,spos,sdepth) \
  {\
  pdepth=&(c->depth);\
  *pdepth =sdepth-B ## sideinc; \
  if (A ## sideinc > 0) { FOO(side,+,spos) } else { FOO(side,-,spos) } \
  prc=c;\
  if (!(*pdepth < 0)) \
	{\
	++ret;\
	c=&cv[ret];\
	}\
  }
  //TRI_CONTAIN_POINT(CONTACT(contact,ret*skip)->pos)

   if(B1<B2)
  {
	  BAR(0,1,pos,depth);
	  if(B2<B3) 
	  {
		  BAR(1,2,pos,depth);
		  BAR(0,1,prc->position,prc->depth);
		  
	  }
	  else
	  {
		  BAR(2,3,pos,depth);
		  BAR(0,1,prc->position,prc->depth);
	  }
  }
  else
  {
	  BAR(1,2,pos,depth);
	  if(B1<B3)
	  {
		  BAR(0,1,pos,depth);
		  BAR(1,2,prc->position,prc->depth);
	  }
	  else
	  {		
		  BAR(2,3,pos,depth);
		  BAR(1,2,prc->position,prc->depth);
		}
  }
 
#undef FOO
#undef FOO1
#undef BAR
#undef TRI_CONTAIN_POINT
 //done: ;

////////////////////////////////////////////////////////////// end (from geom.cpp dCollideBP)

}
else 
{
	//signum  = -signum;
	if(code<=9)
{
	switch((code-1)%3){
	case 0:
	pos[0]=v0[0];
	pos[1]=v0[1];
	pos[2]=v0[2];
	break;
	case 1:
	pos[0]=v1[0];
	pos[1]=v1[1];
	pos[2]=v1[2];
	break;
	case 2:
	pos[0]=v2[0];
	pos[1]=v2[1];
	pos[2]=v2[2];
	break;
	}
switch((code-1)/3){
	case 0:
		{
		norm = r_p.lines[0].xyz()*signum;
		//norm[0]=R[0]*signum;
		//norm[1]=R[4]*signum;
		//norm[2]=R[8]*signum;
		}
	break;

	case 1:	
		{
		norm = r_p.lines[1].xyz()*signum;
		//norm[0]=R[1]*signum;
		//norm[1]=R[5]*signum;
		//norm[2]=R[9]*signum;
		}
	break;
	case 2:
		{
		norm = r_p.lines[2].xyz()*signum;
		//norm[0]=R[2]*signum;
		//norm[1]=R[6]*signum;
		//norm[2]=R[10]*signum;
		}
	break;
	}
}
else {
	norm[0]=outAx[0]*signum;
	norm[1]=outAx[1]*signum;
	norm[2]=outAx[2]*signum;

}
}
if((norm|triAx)>0.f)	
		return;



cv[0].position[0] = pos[0];
cv[0].position[1] = pos[1];
cv[0].position[2] = pos[2];




cv[0].depth = outDepth;


 for (u32 i=0; i<ret; ++i) {
   // CONTACT(contact,i*skip)->g1 = const_cast<dxGeom*> (o2);
   //CONTACT(contact,i*skip)->g2 = const_cast<dxGeom*> (o1);
	//cv[i].normal[0] = norm[0];
	//cv[i].normal[1] = norm[1];
	cv[i].normal = -norm;

	//SURFACE(contact,i*skip)->mode=T->T->material;
	cb(cv[i]);
 }
 //if(ret&&dGeomGetUserData(o1)->callback)dGeomGetUserData(o1)->callback(T->T,contact);
 return;

/////////////////////////////////////////////////////////////////////////////////////
}





}//xray{
}//collision{
}//detail{