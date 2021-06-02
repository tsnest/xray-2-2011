////////////////////////////////////////////////////////////////////////////
//	Module 		: aabb3f.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : 3D axes aligned bounding box class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_AABB3F_H_INCLUDED
#define CS_CORE_AABB3F_H_INCLUDED

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

struct CS_CORE_API aabb3f {
public:
	vec3f	min;
	vec3f	max;

public:
	inline	aabb3f&	invalidate	();
	inline	aabb3f&	modify		(vec3f const& vector);
	inline	aabb3f&	grow		(vec3f const& vector);
	inline	aabb3f&	grow		(float const value);
	inline	aabb3f&	merge		(aabb3f const& aabb);
	inline	bool	inside		(aabb3f const& aabb) const;
	inline	bool	inside		(vec3f const& center, vec3f const& extents) const;
	inline	bool	over		(aabb3f const& aabb) const;
	inline	bool	over		(vec3f const& center, vec3f const& extents) const;
	inline	bool	test		(aabb3f const& aabb) const;
	inline	bool	test		(vec3f const& center, vec3f const& extents) const;

#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifdef CS_CORE_BUILDING
};

#ifdef CS_CORE_BUILDING
	CS_TYPE_LIST_PUSH_FRONT(aabb3f)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST CS_SAVE_TYPE_LIST(aabb3f)
#endif // #ifdef CS_CORE_BUILDING

#include <cs/core/aabb3f_inline.h>

#endif // #ifndef CS_CORE_AABB3F_H_INCLUDED