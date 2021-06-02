////////////////////////////////////////////////////////////////////////////
//	Created		: 17.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "convex_volume.h"

namespace xray {
namespace render{


////////////////////////////////////////////////////////////////////////
//void			CFrustum::fplane::cache	()	{
//	if(positive(n.x)) {
//		if(positive(n.y)) {
//			if(positive(n.z))	aabb_overlap_id	= 0;
//			else				aabb_overlap_id	= 1;
//		} else {
//			if(positive(n.z))	aabb_overlap_id	= 2;
//			else				aabb_overlap_id = 3;
//		}
//	} else {
//		if(positive(n.y)) {
//			if(positive(n.z))	aabb_overlap_id = 4;
//			else				aabb_overlap_id = 5;
//		} else {
//			if(positive(n.z))	aabb_overlap_id = 6;
//			else				aabb_overlap_id = 7;
//		}
//	}
//}
//void			CFrustum::_add			(Fplane &P) 
//{ 
//	VERIFY(count<FRUSTUM_MAXPLANES); 
//	planes[count].set		(P);
//	planes[count].cache	();
//	count					++;
//}
//void			CFrustum::_add			(Fvector& P1, Fvector& P2, Fvector&P3)
//{
//	VERIFY(count<FRUSTUM_MAXPLANES);
//	planes[count].build_precise	(P1,P2,P3);
//	planes[count].cache			();
//	count							++;
//}
// 
//#define			mx			0
//#define			my			1
//#define			mz			2
//#define			Mx			3
//#define			My			4
//#define			Mz			5
//u32				frustum_aabb_remap [8][6]	=
//{
//	{ Mx,My,Mz,mx,my,mz}, 
//	{ Mx,My,mz,mx,my,Mz}, 
//	{ Mx,my,Mz,mx,My,mz}, 
//	{ Mx,my,mz,mx,My,Mz}, 
//	{ mx,My,Mz,Mx,my,mz}, 
//	{ mx,My,mz,Mx,my,Mz}, 
//	{ mx,my,Mz,Mx,My,mz}, 
//	{ mx,my,mz,Mx,My,Mz}
//};
//
//////////////////////////////////////////////////////////////////////
math::intersection convex_volume::test_sphere(const float3& c, float r) const
{
	bool outside = false;
	bool inside = true;

	for (u32 i=0; i<count; ++i)
	{
		const math::plane& p = planes[i];

		outside |= p.classify(c) > r+math::epsilon_5;

		if (outside)
			break;

		inside  &= p.classify(c) < -r-math::epsilon_5;
	}

	if (outside)
		return math::intersection_outside;

	if (inside)
		return math::intersection_inside;

	return math::intersection_intersect;
}

//BOOL	CFrustum::testSphere_dirty		(Fvector& c, float r) const
//{
//	switch (count) {
//		case 12:if (planes[11].classify(c)>r)	return FALSE;
//		case 11:if (planes[10].classify(c)>r)	return FALSE;
//		case 10:if (planes[9].classify(c)>r)	return FALSE;
//		case 9:	if (planes[8].classify(c)>r)	return FALSE;
//		case 8:	if (planes[7].classify(c)>r)	return FALSE;
//		case 7:	if (planes[6].classify(c)>r)	return FALSE;
//		case 6:	if (planes[5].classify(c)>r)	return FALSE;
//		case 5:	if (planes[4].classify(c)>r)	return FALSE;
//		case 4:	if (planes[3].classify(c)>r)	return FALSE;
//		case 3:	if (planes[2].classify(c)>r)	return FALSE;
//		case 2:	if (planes[1].classify(c)>r)	return FALSE;
//		case 1:	if (planes[0].classify(c)>r)	return FALSE;
//		case 0:	break;
//		default:	NODEFAULT();
//	}
//	return TRUE;
//}
//
//EFC_Visible	CFrustum::testAABB			(const float* mM, u32& test_mask) const
//{
//	// go for trivial rejection or acceptance using "faster overlap test"
//	u32		bit = 1;
//
//	for (int i=0; i<count; i++, bit<<=1)
//	{
//		if (test_mask&bit) {
//			EFC_Visible	r	= AABB_OverlapPlane(planes[i],mM);
//			if (fcvFully==r)	test_mask&=~bit;					// fully - no need to test this plane
//			else if (fcvNone==r){ test_mask=0; return fcvNone;	}	// none - return
//		}
//	}
//	return test_mask ? fcvPartial:fcvFully;
//}
//
//EFC_Visible	CFrustum::testSAABB			(Fvector& c, float r, const float* mM, u32& test_mask) const
//{
//	u32	bit = 1;
//	for (int i=0; i<count; i++, bit<<=1)
//	{
//		if (test_mask&bit) {
//			float cls = planes[i].classify(c);
//			if (cls>r) { test_mask=0; return fcvNone;}	// none  - return
//			if (_abs(cls)>=r) test_mask&=~bit;			// fully - no need to test this plane
//			else {
//				EFC_Visible	r	= AABB_OverlapPlane(planes[i],mM);
//				if (fcvFully==r)	test_mask&=~bit;					// fully - no need to test this plane
//				else if (fcvNone==r){ test_mask=0; return fcvNone;	}	// none - return
//			}
//		}
//	}
//	return test_mask ? fcvPartial:fcvFully;
//}
//
//BOOL		CFrustum::testPolyInside_dirty(Fvector* p, int count) const
//{
//	Fvector* e = p+count;
//	for (int i=0; i<count; i++)
//	{
//		const fplane &P = planes[i];
//		for (Fvector* I=p; I!=e; I++)
//			if (P.classify(*I)>0) return false;
//	}
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////
//void CFrustum::CreateFromPoints(Fvector* p, int count, Fvector& COP)
//{
//	VERIFY(count<FRUSTUM_MAXPLANES);
//	VERIFY(count>=3);
//
//	_clear();
//	for (int i=1; i<count; i++)
//		_add(COP,p[i-1],p[i]);
//	_add(COP,p[count-1],p[0]);
//}
//
void convex_volume::create_from_planes(const math::plane* p, u32 plane_count)
{
	for (u32 k=0; k<plane_count; ++k)
	{
		planes[k] = p[k];
	}

	for (u32 i=0;i<plane_count;i++)
	{
		planes[i].normalize();
		//planes[i].cache	();
	}

	count = plane_count;
}
//
//void CFrustum::CreateFromPortal(sPoly* poly, Fvector& vPN, Fvector& vBase, Fmatrix& mFullXFORM)
//{
//	Fplane	P;
//	P.build_precise	((*poly)[0],(*poly)[1],(*poly)[2]);
//
//	if (poly->size()>6) {
//		SimplifyPoly_AABB(poly,P);
//		P.build_precise	((*poly)[0],(*poly)[1],(*poly)[2]);
//	}
//
//	// Check plane orientation relative to viewer
//	// and reverse if needed
//	if (P.classify(vBase)<0)
//	{
//		std::reverse(poly->begin(),poly->end());
//		P.build_precise	((*poly)[0],(*poly)[1],(*poly)[2]);
//	}
//
//	// Base creation
//	CreateFromPoints(poly->begin(),poly->size(),vBase);
//
//	// Near clipping plane
//	_add		(P);
//
//	// Far clipping plane
//	Fmatrix &M	= mFullXFORM;
//	P.normal.x		= -(M.e03 - M.e02);
//	P.normal.y		= -(M.e13 - M.e12);
//	P.normal.z		= -(M.e23 - M.e22);
//	P.d			= -(M.e33 - M.e22);
//	float denom = 1.0f / P.normal.magnitude();
//	P.normal.x		*= denom;
//	P.normal.y		*= denom;
//	P.normal.z		*= denom;
//	P.d			*= denom;
//	_add		(P);
//}
//
//void CFrustum::SimplifyPoly_AABB(sPoly* poly, Fplane& plane)
//{
//	Fmatrix		mView,mInv;
//	Fvector		from,up,right,y;
//	from.set	((*poly)[0]);
//	y.set		(0,1,0);
//	if (_abs(plane.normal.y)>0.99f) y.set(1,0,0);
//	right.crossproduct		(y,plane.normal);
//	up.crossproduct			(plane.normal,right);
//	mView.build_camera_dir	(from,plane.normal,up);
//
//	// Project and find extents
//	Fvector2	min,max;
//	min.set		(flt_max,flt_max);
//	max.set		(flt_min,flt_min);
//	for (u32 i=0; i<poly->size(); i++)
//	{
//		Fvector2 tmp;
//		mView.transform_tiny32(tmp,(*poly)[i]);
//		min.min(tmp.x,tmp.y);
//		max.max(tmp.x,tmp.y);
//	}
//
//	// Build other 2 points and inverse project
//	Fvector2	p1,p2;
//	p1.set		(min.x,max.y);
//	p2.set		(max.x,min.y);
//	mInv.invert	(mView);
//	poly->clear	();
//
//	mInv.transform_tiny23(poly->last(),min);	poly->inc();
//	mInv.transform_tiny23(poly->last(),p1);		poly->inc();
//	mInv.transform_tiny23(poly->last(),max);	poly->inc();
//	mInv.transform_tiny23(poly->last(),p2);		poly->inc();
//}
//
//void CFrustum::CreateOccluder(Fvector* p, int count, Fvector& vBase, CFrustum& clip)
//{
//	VERIFY(count<FRUSTUM_SAFE);
//	VERIFY(count>=3);
//
//	BOOL	edge[FRUSTUM_SAFE];
//	float	cls	[FRUSTUM_SAFE];
//	ZeroMemory	(edge,sizeof(edge));
//	for (int i=0; i<clip.count; i++)
//	{
//		// classify all points relative to plane #i
//		fplane &P = clip.planes[i];
//		for (int j=0; j<count; j++) cls[j]=_abs(P.classify(p[j]));
//
//		// test edges to see which lies directly on plane
//		for (j=0; j<count; j++) {
//			if (cls[j]<EPS_L)
//			{
//				int next = j+1; if (next>=count) next=0;
//				if (cls[next]<EPS_L) {
//					// both points lies on plane - mark as 'open'
//					edge[j] = true;
//				}
//			}
//		}
//	}
//
//	// here we have all edges marked accordenly to 'open' / 'closed' classification
//	_clear	();
//	_add	(p[0],p[1],p[2]);		// main plane
//	for (i=0; i<count; i++)
//	{
//		if (!edge[i]) {
//			int next = i+1; if (next>=count) next=0;
//			_add(vBase,p[i],p[next]);
//		}
//	}
//}
//
//sPoly*	CFrustum::ClipPoly(sPoly& S, sPoly& D) const
//{
//	sPoly*	src		= &D;
//	sPoly*	dest	= &S;
//	for (int i=0; i<count; i++)
//	{
//		// cache plane and swap lists
//		const fplane &P = planes[i];
//		std::swap		(src,dest);
//		dest->clear		();
//
//		// classify all points relative to plane #i
//		float	cls	[FRUSTUM_SAFE];
//		for (u32 j=0; j<src->size(); j++) cls[j]=P.classify((*src)[j]);
//
//		// clip everything to this plane
//		cls[src->size()] = cls[0];
//		src->push_back((*src)[0]);
//		Fvector D; float denum,t;
//		for (j=0; j<src->size()-1; j++)
//		{
//			if ((*src)[j].similar((*src)[j+1],EPS_S)) continue;
//
//			if (negative(cls[j]))
//			{
//				dest->push_back((*src)[j]);
//				if (positive(cls[j+1]))
//				{
//					// segment intersects plane
//					D.sub((*src)[j+1],(*src)[j]);
//					denum = P.normal.dotproduct(D);
//					if (denum!=0) {
//						t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
//						dest->last().mad((*src)[j],D,t);
//						dest->inc();
//					}
//				}
//			} else {
//				// J - outside
//				if (negative(cls[j+1]))
//				{
//					// J+1  - inside
//					// segment intersects plane
//					D.sub((*src)[j+1],(*src)[j]);
//					denum = P.normal.dotproduct(D);
//					if (denum!=0) {
//						t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
//						dest->last().mad((*src)[j],D,t);
//						dest->inc();
//					}
//				}
//			}
//		}
//
//		// here we end up with complete polygon in 'dest' which is inside plane #i
//		if (dest->size()<3) return 0;
//	}
//	return dest;
//}
//
//BOOL CFrustum::CreateFromClipPoly(Fvector* p, int count, Fvector& vBase, CFrustum& clip)
//{
//	VERIFY(count<FRUSTUM_MAXPLANES);
//	VERIFY(count>=3);
//
//	sPoly	poly1	(p,count);
//	sPoly	poly2;
//	sPoly*	dest	= clip.ClipPoly(poly1,poly2);
//
//	// here we end up with complete frustum-polygon in 'dest'
//	if (0==dest)	return false;
//
//	CreateFromPoints(dest->begin(),dest->size(),vBase);
//	return	true;
//}
//
void convex_volume::create_from_matrix(const float4x4 &mat/*, u32 mask*/)
{
	planes[ 0 ].vector.x	= -( mat.e03 + mat.e00 );
	planes[ 0 ].vector.y	= -( mat.e13 + mat.e10 );
	planes[ 0 ].vector.z	= -( mat.e23 + mat.e20 );
	planes[ 0 ].vector.w	= -( mat.e33 + mat.e30 );
															   
	planes[ 1 ].vector.x	= -( mat.e03 - mat.e00 );
	planes[ 1 ].vector.y	= -( mat.e13 - mat.e10 );
	planes[ 1 ].vector.z	= -( mat.e23 - mat.e20 );
	planes[ 1 ].vector.w	= -( mat.e33 - mat.e30 );
															   
	planes[ 2 ].vector.x	= -( mat.e03 - mat.e01 );
	planes[ 2 ].vector.y	= -( mat.e13 - mat.e11 );
	planes[ 2 ].vector.z	= -( mat.e23 - mat.e21 );
	planes[ 2 ].vector.w	= -( mat.e33 - mat.e31 );
															   
	planes[ 3 ].vector.x	= -( mat.e03 + mat.e01 );
	planes[ 3 ].vector.y	= -( mat.e13 + mat.e11 );
	planes[ 3 ].vector.z	= -( mat.e23 + mat.e21 );
	planes[ 3 ].vector.w	= -( mat.e33 + mat.e31 );
															   
	planes[ 4 ].vector.x	= -( mat.e03 - mat.e02 );
	planes[ 4 ].vector.y	= -( mat.e13 - mat.e12 );
	planes[ 4 ].vector.z	= -( mat.e23 - mat.e22 );
	planes[ 4 ].vector.w	= -( mat.e33 - mat.e32 );
														   
	planes[ 5 ].vector.x	= -( mat.e03 + mat.e02 );
	planes[ 5 ].vector.y	= -( mat.e13 + mat.e12 );
	planes[ 5 ].vector.z	= -( mat.e23 + mat.e22 );
	planes[ 5 ].vector.w	= -( mat.e33 + mat.e32 );

	count = 6;

	for (u32 i=0; i<count; ++i)
	{
		planes[i].normalize();
		//planes[i].cache	();
	}
}

}
}
