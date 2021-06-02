////////////////////////////////////////////////////////////////////////////
//	Module 		: aabb3f_inline.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : 3D axes aligned bounding box class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_AABB3F_INLINE_H_INCLUDED
#define CS_CORE_AABB3F_INLINE_H_INCLUDED

inline aabb3f& aabb3f::invalidate	()
{
	min.set		(flt_max,flt_max,flt_max);
	max.set		(flt_min,flt_min,flt_min);

	return		*this;
}

inline aabb3f& aabb3f::modify		(vec3f const& vector)
{
	min.min 	(vector);
	max.max 	(vector);
	
	return		*this;
}

inline aabb3f& aabb3f::grow			(vec3f const& vector)
{
	min.sub 	(vector);
	max.add 	(vector);
	
	return		*this;
}

inline aabb3f& aabb3f::grow			(float const value)
{
	min.sub 	(value);
	max.add 	(value);
	
	return		*this;
}

inline aabb3f& aabb3f::merge		(aabb3f const& aabb)
{
	modify		(aabb.min);
	modify		(aabb.max);
	
	return		*this;
}

inline bool aabb3f::inside			(aabb3f const& aabb) const
{
	return		(
		(min.x >= aabb.min.x) &&
		(min.y >= aabb.min.y) &&
		(min.z >= aabb.min.z) &&
		(max.x <= aabb.max.x) &&
		(max.y <= aabb.max.y) &&
		(max.z <= aabb.max.z)
	);
}

inline bool aabb3f::inside			(vec3f const& center, vec3f const& extents) const
{
	return		(
		(min.x >= center.x - extents.x) &&
		(min.y >= center.y - extents.y) &&
		(min.z >= center.z - extents.z) &&
		(max.x <= center.x + extents.x) &&
		(max.y <= center.y + extents.y) &&
		(max.z <= center.z + extents.z)
	);
}

inline bool aabb3f::over			(aabb3f const& aabb) const
{
	return		(
		(aabb.min.x >= min.x) &&
		(aabb.min.y >= min.y) &&
		(aabb.min.z >= min.z) &&
		(aabb.max.x <= max.x) &&
		(aabb.max.y <= max.y) &&
		(aabb.max.z <= max.z)
	);
}

inline bool aabb3f::over			(vec3f const& center, vec3f const& extents) const
{
	return		(
		(center.x - extents.x >= min.x) &&
		(center.y - extents.y >= min.y) &&
		(center.z - extents.z >= min.z) &&
		(center.x + extents.x <= max.x) &&
		(center.y + extents.y <= max.y) &&
		(center.z + extents.z <= max.z)
	);
}

inline bool aabb3f::test			(aabb3f const& aabb) const
{
	if (max.x < aabb.min.x)
		return	false;

	if (max.y < aabb.min.y)
		return	false;

	if (max.z < aabb.min.z)
		return	false;

	if (min.x > aabb.max.x)
		return	false;

	if (min.y > aabb.max.y)
		return	false;

	if (min.z > aabb.max.z)
		return	false;

	return		true;
}

inline bool aabb3f::test			(vec3f const& center, vec3f const& extents) const
{
	if (max.x < center.x - extents.x)
		return	false;

	if (max.y < center.y - extents.y)
		return	false;

	if (max.z < center.z - extents.z)
		return	false;

	if (min.x > center.x + extents.x)
		return	false;

	if (min.y > center.y + extents.y)
		return	false;

	if (min.z > center.z + extents.z)
		return	false;

	return		true;
}

#endif // #ifndef CS_CORE_AABB3F_INLINE_H_INCLUDED