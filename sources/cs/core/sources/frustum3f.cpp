////////////////////////////////////////////////////////////////////////////
//	Module 		: frustum3f.cpp
//	Created 	: 19.01.2007
//  Modified 	: 19.01.2007
//	Author		: Dmitriy Iassenev
//	Description : 3D float frustum class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/frustum3f.h>

static u32 const min_x			= 0;
static u32 const min_y			= 1;
static u32 const min_z			= 2;
static u32 const max_x			= 3;
static u32 const max_y			= 4;
static u32 const max_z			= 5;
static u32 const aabb_lut[8][6]	= {
	{ max_x, max_y, max_z, min_x, min_y, min_z}, 
	{ max_x, max_y, min_z, min_x, min_y, max_z}, 
	{ max_x, min_y, max_z, min_x, max_y, min_z}, 
	{ max_x, min_y, min_z, min_x, max_y, max_z}, 
	{ min_x, max_y, max_z, max_x, min_y, min_z}, 
	{ min_x, max_y, min_z, max_x, min_y, max_z}, 
	{ min_x, min_y, max_z, max_x, max_y, min_z}, 
	{ min_x, min_y, min_z, max_x, max_y, max_z}
};

void frustum3f::aabb_plane::normalize					()
{
	float					denomimator = 1.f / plane.n().magnitude();
	plane.x					*= denomimator;
	plane.y					*= denomimator;
	plane.z					*= denomimator;
	plane.w					*= denomimator;
	
	u32						x = (*(u32*)&plane.x >> 31) << 2;
	u32						y = (*(u32*)&plane.y >> 31) << 1;
	u32						z = (*(u32*)&plane.z >> 31);

	m_lut_id				= x | y | z;
}

frustum3f::intersection_enum frustum3f::aabb_plane::test(aabb3f const& aabb) const
{
	typedef u32				lut_ids_type[6];
	lut_ids_type const&		ids = aabb_lut[m_lut_id];

	typedef float			values_type[6];
	values_type const&		values = (values_type const&)aabb.min.x;

	// test negative first
	if (plane.signed_distance(vec3f().set(values[ids[3]], values[ids[4]], values[ids[5]])) > 0.f)
		return				intersection_outside;

	// test positive
	if (plane.signed_distance(vec3f().set(values[ids[0]], values[ids[1]], values[ids[2]])) <= 0.f)
		return				intersection_inside;

	return					intersection_intersect;
}

frustum3f& frustum3f::construct							(matrix4f const& matrix)
{
	m_planes[0].plane.x		= -(matrix._14 + matrix._11);
	m_planes[0].plane.y		= -(matrix._24 + matrix._21);
	m_planes[0].plane.z		= -(matrix._34 + matrix._31);
	m_planes[0].plane.w		= -(matrix._44 + matrix._41);
	
	m_planes[1].plane.x		= -(matrix._14 - matrix._11);
	m_planes[1].plane.y		= -(matrix._24 - matrix._21);
	m_planes[1].plane.z		= -(matrix._34 - matrix._31);
	m_planes[1].plane.w		= -(matrix._44 - matrix._41);

	m_planes[2].plane.x		= -(matrix._14 - matrix._12);
	m_planes[2].plane.y		= -(matrix._24 - matrix._22);
	m_planes[2].plane.z		= -(matrix._34 - matrix._32);
	m_planes[2].plane.w		= -(matrix._44 - matrix._42);

	m_planes[3].plane.x		= -(matrix._14 + matrix._12);
	m_planes[3].plane.y		= -(matrix._24 + matrix._22);
	m_planes[3].plane.z		= -(matrix._34 + matrix._32);
	m_planes[3].plane.w		= -(matrix._44 + matrix._42);

	m_planes[4].plane.x		= -(matrix._14 - matrix._13);
	m_planes[4].plane.y		= -(matrix._24 - matrix._23);
	m_planes[4].plane.z		= -(matrix._34 - matrix._33);
	m_planes[4].plane.w		= -(matrix._44 - matrix._43);

	m_planes[5].plane.x		= -(matrix._14 + matrix._13);
	m_planes[5].plane.y		= -(matrix._24 + matrix._23);
	m_planes[5].plane.z		= -(matrix._34 + matrix._33);
	m_planes[5].plane.w		= -(matrix._44 + matrix._43);

	aabb_plane*				i = m_planes;
	aabb_plane*				e = m_planes + plane_count;
	for ( ; i != e; ++i)
		(*i).normalize		();

	return					*this;
}

frustum3f::intersection_enum frustum3f::test_inexact	(aabb3f const& aabb) const
{
	u32						inside_count = 0;

	aabb_plane const*		i = m_planes;
	aabb_plane const*		e = m_planes + plane_count;
	for ( ; i != e; ++i) {
		switch ((*i).test(aabb)) {
			case intersection_outside	:
				return		intersection_outside;
			case intersection_intersect :
				continue;
			case intersection_inside	: {
				++inside_count;
				continue;
			}
			default						: NODEFAULT;
		}
	}

	if (inside_count < plane_count)
		return				intersection_intersect;

	ASSERT					(inside_count == plane_count);
	return					intersection_inside;
}