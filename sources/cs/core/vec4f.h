////////////////////////////////////////////////////////////////////////////
//	Module 		: vec4f.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : float vector with 4 components
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_VEC4F_H_INCLUDED
#define CS_CORE_VEC4F_H_INCLUDED

#include <cs/config.h>

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

#ifndef CS_CORE_API
#	define CS_CORE_API	CS_API
#	define CS_CORE_API_DEFINED_IN_VEC4F_H_INCLUDED
#endif // #ifndef CS_CORE_API

namespace cs {
namespace core {
namespace math {

struct CS_CORE_API vec4f {
public:
	float	x;
	float	y;
	float	z;
	float	w;

public:
    inline	float&		operator[]	(u32 const i) const;
	inline	vec3f&		vec3		();
	inline	vec3f const&	vec3		() const;
	inline	vec4f&		set		(float const _x, float const _y, float const _z, float const _w);
	inline	vec4f&		set		(vec3f const& _x, float const _w);

#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifndef CS_CORE_BUILDING
}; // struct vec4f

} // namespace math
} // namespace core
} // namespace cs

#ifdef CS_CORE_API_DEFINED_IN_VEC4F_H_INCLUDED
#	undef CS_CORE_API_DEFINED_IN_VEC4F_H_INCLUDED
#	undef CS_CORE_API
#endif // #ifndef CS_CORE_API_DEFINED_IN_VEC4F_H_INCLUDED

#ifdef CS_CORE_BUILDING
	typedef cs::core::math::vec4f	cs__core__math__vec4f;
	CS_TYPE_LIST_PUSH_FRONT			(cs__core__math__vec4f)
#	undef							CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST	CS_SAVE_TYPE_LIST(cs__core__math__vec4f)
#endif // #ifdef CS_CORE_BUILDING

#include <cs/core/vec4f_inline.h>

#endif // #ifndef CS_CORE_VEC4F_H_INCLUDED