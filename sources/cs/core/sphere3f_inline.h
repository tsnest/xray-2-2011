////////////////////////////////////////////////////////////////////////////
//	Module 		: sphere3f_inline.h
//	Created 	: 28.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : sphere class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SPHERE3F_INLINE_H_INCLUDED
#define CS_CORE_SPHERE3F_INLINE_H_INCLUDED

#define SPHERE3F	cs::core::math::sphere3f

inline SPHERE3F& SPHERE3F::set	(float const x, float const y, float const z, float const radius)
{
	m_center.set	(x, y, z);
	m_radius		= radius;
	return			*this;
}

inline SPHERE3F& SPHERE3F::set	(vec3f const& center, float const radius)
{
	m_center		= center;
	m_radius		= radius;
	return			*this;
}

inline bool SPHERE3F::intersect	(SPHERE3F const& sphere) const
{
	float			max_distance = m_radius + sphere.m_radius;
	return			m_center.distance_to_sqr(sphere.m_center) <= _sqr(max_distance);
}

#undef SPHERE3F

#endif // #ifndef CS_CORE_SPHERE3F_INLINE_H_INCLUDED