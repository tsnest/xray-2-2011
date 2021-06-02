////////////////////////////////////////////////////////////////////////////
//	Module 		: sphere3f.h
//	Created 	: 28.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : sphere class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SPHERE3F_H_INCLUDED
#define CS_CORE_SPHERE3F_H_INCLUDED

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

namespace cs {
namespace core {
namespace math {

struct sphere3f {
	vec3f	m_center;
	float	m_radius;

	inline	sphere3f&	set		(float const x, float const y, float const z, float const radius);
	inline	sphere3f&	set		(vec3f const& center, float const radius);
	inline	bool		intersect	(sphere3f const& sphere) const;

#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifdef CS_CORE_BUILDING
}; // struct sphere3f

} // namespace math
} // namespace core
} // namespace cs

#ifdef CS_CORE_BUILDING
	typedef cs::core::math::sphere3f	cs__core__math__sphere3f;
	CS_TYPE_LIST_PUSH_FRONT				(cs__core__math__sphere3f)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST		CS_SAVE_TYPE_LIST(cs__core__math__sphere3f)
#endif // #ifdef CS_CORE_BUILDING

#include <cs/core/sphere3f_inline.h>

#endif // #ifndef CS_CORE_SPHERE3F_H_INCLUDED
