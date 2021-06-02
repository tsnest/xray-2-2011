////////////////////////////////////////////////////////////////////////////
//	Module 		: vec4f_inline.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : float vector with 4 components inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_VEC4F_INLINE_H_INCLUDED
#define CS_CORE_VEC4F_INLINE_H_INCLUDED

#define	vec	cs::core::math::vec4f

inline float& vec::operator[]	(unsigned int const i) const
{
	ASSERT	(i < 4);
	return	((float*)&x)[i];
}

inline vec3f& vec::vec3			()
{
	return	*(vec3f*)&x;
}

inline vec3f const& vec::vec3		() const
{
	return	*(vec3f const*)&x;
}

inline vec& vec::set			(float const _x, float const _y, float const _z, float const _w)
{
	x		= _x;
	y		= _y;
	z		= _z;
	w		= _w;

	return	*this;
}

inline vec& vec::set			(vec3f const& xyz, float const _w)
{
	vec3()	= xyz;
	w		= _w;

	return	*this;
}

#undef vec

#endif // #ifndef CS_CORE_VEC4F_INLINE_H_INCLUDED
