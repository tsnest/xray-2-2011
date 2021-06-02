////////////////////////////////////////////////////////////////////////////
//	Module 		: aabb2f.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : 2D axes aligned bounding box class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_AABB2F_H_INCLUDED
#define CS_CORE_AABB2F_H_INCLUDED

struct aabb2f {
public:
	vec2f	min;
	vec2f	max;

public:
	inline	aabb2f& grow	(vec2f const& position);
	inline	bool	pick	(vec2f const& position, vec2f const& direction, float const epsilon = 0.f) const;
};

#include <cs/core/aabb2f_inline.h>

#endif // #ifndef CS_CORE_AABB2F_H_INCLUDED