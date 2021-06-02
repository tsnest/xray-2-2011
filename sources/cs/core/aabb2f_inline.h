////////////////////////////////////////////////////////////////////////////
//	Module 		: aabb2f_inline.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : 2D axes aligned bounding box class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_AABB2F_INLINE_H_INCLUDED
#define CS_CORE_AABB2F_INLINE_H_INCLUDED

inline aabb2f& aabb2f::grow	(vec2f const& position)
{
	min.sub			(position);
	max.add			(position);

	return			*this;
}

inline bool aabb2f::pick	(vec2f const& position, vec2f const& direction, float const epsilon) const
{
	vec2f			position2min = vec2f().sub(min,position);
	vec2f			position2max = vec2f().sub(max,position);

	if (!fp_zero(direction.x)) {
		float		alpha = position2min.x/direction.x;
		if (alpha >= 0.f) {
			float		yt = alpha*direction.y;
			if ((yt >= (position2min.y - epsilon)) && (yt <= (position2max.y + epsilon)))
				return	true;
		}

		alpha		= position2max.x/direction.x;
		if (alpha >= 0.f) {
			float		yt = alpha*direction.y;
			if ((yt >= (position2min.y - epsilon)) && (yt <= (position2max.y + epsilon)))
				return	true;
		}
	}

	if (!fp_zero(direction.y)) {
		float		alpha = position2min.y/direction.y;
		if (alpha >= 0.f) {
			float		xt = alpha*direction.x;
			if ((xt >= (position2min.x - epsilon)) && (xt <= (position2max.x + epsilon)))
				return	true;
		}

		alpha			= position2max.y/direction.y;
		if (alpha >= 0.f) {
			float		xt = alpha*direction.x;
			if ((xt >= (position2min.x - epsilon)) && (xt <= (position2max.x + epsilon)))
				return	true;
		}
	}

	return			false;
}

#endif // #ifndef CS_CORE_AABB2F_INLINE_H_INCLUDED