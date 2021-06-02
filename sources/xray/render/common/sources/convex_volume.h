////////////////////////////////////////////////////////////////////////////
//	Created		: 17.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONVEX_VOLUME_H_INCLUDED
#define CONVEX_VOLUME_H_INCLUDED

#define FRUSTUM_MAXPLANES	12
#define FRUSTUM_P_LEFT		(1<<0)
#define FRUSTUM_P_RIGHT		(1<<1)
#define FRUSTUM_P_TOP		(1<<2)
#define FRUSTUM_P_BOTTOM	(1<<3)
#define FRUSTUM_P_NEAR		(1<<4)
#define FRUSTUM_P_FAR		(1<<5)

#define FRUSTUM_P_LRTB		(FRUSTUM_P_LEFT|FRUSTUM_P_RIGHT|FRUSTUM_P_TOP|FRUSTUM_P_BOTTOM)
#define FRUSTUM_P_ALL		(FRUSTUM_P_LRTB|FRUSTUM_P_NEAR|FRUSTUM_P_FAR)

#define FRUSTUM_SAFE		(FRUSTUM_MAXPLANES*4)

namespace xray {
namespace render{


class convex_volume
{
public:
	//static const FRUSTUM_MAXPLANES = 12;
	
	math::plane	planes[FRUSTUM_MAXPLANES];
	u32			count;

public:
	void	clear()	{count=0;}

	void	add(math::plane &P);
	void	add(const float3& p1, const float3& p2, const float3& p3);

	//void			SimplifyPoly_AABB	(sPoly* P, math::plane& plane);

	//void			CreateOccluder		(float3* p,	int count,		float3& vBase, CFrustum& clip);
	//BOOL			CreateFromClipPoly	(float3* p,	int count,		float3& vBase, CFrustum& clip);	// returns 'false' if creation failed
	//void			CreateFromPoints	(float3* p,	int count,		float3& vBase );
	void			create_from_matrix(const float4x4 &mat/*, u32 mask = FRUSTUM_P_ALL*/);
	//void			CreateFromPortal	(sPoly* P,		float3& vPN,	float3& vBase, float4x4& mFullXFORM);
	void			create_from_planes(const math::plane* p, u32 count);

	//sPoly*			ClipPoly			(sPoly& src, sPoly& dest) const;

	//u32				getMask				() const { return (1<<p_count)-1; }

	math::intersection	test_sphere(const float3& c, float r) const;
	
	math::intersection	test(math::sphere const& sphere) const	{return test_sphere(sphere.center, sphere.radius);}
	//BOOL			testSphere_dirty	(float3& c, float r)									const;
	//EFC_Visible		testAABB			(const float* mM, u32& test_mask)						const;
	//EFC_Visible		testSAABB			(float3& c, float r, const float* mM, u32& test_mask)	const;
	//BOOL			testPolyInside_dirty(float3* p, int count)									const;

	//IC BOOL			testPolyInside		(sPoly& src)											const
 //   {
 //   	sPoly d;
 //       return !!ClipPoly(src,d);
 //   }
 //  	IC BOOL			testPolyInside		(float3* p, int count)									const
 //   {
 //   	sPoly src(p,count);
 //       return testPolyInside(src);
 //   }
}; // class convex_volume

} // namespace render
} // namespace xray


#endif // #ifndef CONVEX_VOLUME_H_INCLUDED