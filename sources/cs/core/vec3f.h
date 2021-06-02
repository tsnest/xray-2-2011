////////////////////////////////////////////////////////////////////////////
//	Module 		: vec3f.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : float vector with 3 components
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_VEC3F_H_INCLUDED
#define CS_CORE_VEC3F_H_INCLUDED

#include <cs/core/math_functions.h>

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

namespace cs {
namespace core {
namespace math {

struct vec3f {
public:
	float	x;
	float	y;
	float	z;

public:
    inline	float&		operator[]		(u32 const i) const;

	inline	vec3f&		set			(float const _x, float const _y, float const _z);

	inline	vec3f&		add			(float const value);
	inline	vec3f&		add			(vec3f const& vector);
	inline	vec3f&		add			(vec3f const& _0, vec3f const& _1);
	inline	vec3f&		add			(vec3f const& _0, float const _1);

	inline	vec3f&		sub			(float const value);
	inline	vec3f&		sub			(vec3f const& vector);
	inline	vec3f&		sub			(vec3f const& _0, vec3f const& _1);
	inline	vec3f&		sub			(vec3f const& _0, float const _1);

	inline	vec3f&		mul			(float const value);
	inline	vec3f&		mul			(vec3f const& vector);
	inline	vec3f&		mul			(vec3f const& _0, vec3f const& _1);
	inline	vec3f&		mul			(vec3f const& _0, float const _1);

	inline	vec3f&		div			(float const divisor);
	inline	vec3f&		div			(vec3f const& vector);
	inline	vec3f&		div			(vec3f const& _0, vec3f const& _1);
	inline	vec3f&		div			(vec3f const& _0, float const _1);

	inline	vec3f&		mad			(vec3f const& point, vec3f const& direction, float const multiplier);
	inline	vec3f&		abs			(vec3f const& vector);
	inline	vec3f&		min			(vec3f const& vector);
	inline	vec3f&		max			(vec3f const& vector);
	inline	vec3f&		clamp			(vec3f const& low, vec3f const& high);

	inline	vec3f&		negate			();

	inline	vec3f&		set_angles_hp	(float const heading, float const pitch);

	inline	void		get_angles_hp	(float& heading, float& pitch) const;
	inline	float		get_angles_h	() const;
	inline	float		get_angles_p	() const;

	inline	vec3f&		normalize		();
	inline	vec3f&		normalize_safe	();

	inline	float		magnitude_sqr	() const;
	inline	float		magnitude		() const;

	inline	float		distance_to_sqr	(vec3f const& vector) const;
	inline	float		distance_to		(vec3f const& vector) const;

	inline	vec3f&		cross_product	(vec3f const& _0, vec3f const& _1);
	inline	float		dot_product		(vec3f const& vector) const;
	inline	vec3f&		create_normal	(vec3f const& _0, vec3f const& _1, vec3f const& _2);

	inline	bool		similar			(vec3f const& vector, float const epsilon = EPS_7) const;
	inline	bool		similar			(vec3f const& vector, vec3f const& epsilon ) const;

#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifdef CS_CORE_BUILDING
}; // struct vec3f

} // namespace math
} // namespace core
} // namespace cs

#ifdef CS_CORE_BUILDING
	typedef cs::core::math::vec3f		cs__core__math__vec3f;
	CS_TYPE_LIST_PUSH_FRONT				(cs__core__math__vec3f)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST		CS_SAVE_TYPE_LIST(cs__core__math__vec3f)
#endif // #ifdef CS_CORE_BUILDING

#ifdef CS_BUILDING
inline bool 			valid			(cs::core::math::vec3f const& vector)
{
	return	valid(vector.x) && valid(vector.y) && valid(vector.z);
}

#	define push_xyz(s) (s).x,(s).y,(s).z

#endif // #ifdef CS_BUILDING

#include <cs/core/vec3f_inline.h>

#endif // #ifndef CS_CORE_VEC3F_H_INCLUDED
