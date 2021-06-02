////////////////////////////////////////////////////////////////////////////
//	Module 		: frustum3f.h
//	Created 	: 19.01.2007
//  Modified 	: 19.01.2007
//	Author		: Dmitriy Iassenev
//	Description : 3D float frustum class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_FRUSTUM3F_H_INCLUDED
#define CS_CORE_FRUSTUM3F_H_INCLUDED

class CS_CORE_API frustum3f {
public:
	enum intersection_enum {
		intersection_outside	= u32(0),
		intersection_intersect,
		intersection_inside,
	}; // enum intersection_enum

public:
	frustum3f&				construct	(matrix4f const& matrix);
	intersection_enum		test_inexact(aabb3f const& aabb) const;

private:
	class CS_CORE_API aabb_plane {
	public:
		void				normalize	();
		intersection_enum	test		(aabb3f const& aabb) const;

	public:
		plane3f				plane;

	private:
		u32					m_lut_id;
	}; // struct aabb_plane

private:
	enum {
		plane_count			= 6,
	}; // enum

private:
	aabb_plane				m_planes[plane_count];
}; // class frustum3f

#endif // #ifndef CS_CORE_FRUSTUM3F_H_INCLUDED