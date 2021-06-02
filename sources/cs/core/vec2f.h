////////////////////////////////////////////////////////////////////////////
//	Module 		: vec2f.h
//	Created 	: 25.08.2006
//  Modified 	: 25.08.2006
//	Author		: Dmitriy Iassenev
//	Description : float vector with 2 components
////////////////////////////////////////////////////////////////////////////

#ifndef VEC2F_H_INCLUDED
#define VEC2F_H_INCLUDED

#include <cs/core/math_functions.h>

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

struct vec2f {
public:
	float x;
	float y;

public:
    inline	float&	operator[]		(u32 const i) const;
	inline	vec2f&	set			(float const _x, float const _y);

	inline	vec2f&	add			(float const value);
	inline	vec2f&	add			(vec2f const& vector);
	inline	vec2f&	add			(vec2f const& _0, vec2f const& _1);
	inline	vec2f&	add			(vec2f const& _0, float const _1);

	inline	vec2f&	sub			(float const value);
	inline	vec2f&	sub			(vec2f const& vector);
	inline	vec2f&	sub			(vec2f const& _0, vec2f const& _1);
	inline	vec2f&	sub			(vec2f const& _0, float const _1);

	inline	vec2f&	mul			(float const value);
	inline	vec2f&	mul			(vec2f const& vector);
	inline	vec2f&	mul			(vec2f const& _0, vec2f const& _1);
	inline	vec2f&	mul			(vec2f const& _0, float const _1);

	inline	vec2f&	div			(float const value);
	inline	vec2f&	div			(vec2f const& vector);
	inline	vec2f&	div			(vec2f const& _0, vec2f const& _1);
	inline	vec2f&	div			(vec2f const& _0, float const _1);

	inline	vec2f&	mad			(vec2f const& point, vec2f const& direction, float const multiplier);

	inline	vec2f&	normalize		();

	inline	bool	similar			(vec2f const& vector, float const epsilon = EPS_7) const;
	inline	float	dot_product		(vec2f const& vector) const;
	inline	float	cross_product	(vec2f const& vector) const;

	inline	float	magnitude_sqr	() const;
	inline	float	magnitude		() const;

	inline	float	distance_to_sqr	(vec2f const& vector) const;
	inline	float	distance_to		(vec2f const& vector) const;

	inline	float	geth			() const;

#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifdef CS_CORE_BUILDING
};

#ifdef CS_CORE_BUILDING
//	typedef cs::core::math::vec2f	cs__core__math__vec2f;
//	CS_TYPE_LIST_PUSH_FRONT			(cs__core__math__vec2f)
//#	undef CS_SCRIPT_EXPORT_TYPE_LIST
//#	define CS_SCRIPT_EXPORT_TYPE_LIST	CS_SAVE_TYPE_LIST(cs__core__math__vec2f)
	CS_TYPE_LIST_PUSH_FRONT			(vec2f)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST	CS_SAVE_TYPE_LIST(vec2f)
#endif // #ifdef CS_CORE_BUILDING

#ifdef CS_BUILDING
inline bool valid	(vec2f const& vector)
{
	return		valid(vector.x) && valid(vector.y);
}
#endif // #ifdef CS_BUILDING

#include <cs/core/vec2f_inline.h>

#endif // #ifndef VEC2F_H_INCLUDED