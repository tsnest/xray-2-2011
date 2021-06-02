////////////////////////////////////////////////////////////////////////////
//	Module 		: random32_inline.h
//	Created 	: 28.05.2007
//	Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : 32-bit pseudo random number generator inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_RANDOM32_INLINE_H_INCLUDED
#define CS_GTL_RANDOM32_INLINE_H_INCLUDED

#define the_random	gtl::random32

inline void the_random::update_seed	()
{
	m_seed		= 0x08088405*m_seed + 1;
}

inline u32  the_random::seed	()
{
	return		m_seed;
}

inline void the_random::seed		(u32 const seed)
{
	m_seed		= seed;
}

inline u32 the_random::random		(u32 const range)
{
	update_seed	();
	return		u32(u64(m_seed)*u64(range) >> 32);
}

inline u32 the_random::random		()
{
	update_seed	();
	return		u32(u64(m_seed)*u64(u64(1) << 32) >> 32);
}

inline float the_random::random_f	()
{
	update_seed	();
	return		float(m_seed)/float(u32(-1));
}

inline float the_random::random_f	(float const max)
{
	return		random_f()*max;
}

inline float the_random::random_f	(float const min, float const max)
{
	return		min + random_f(max - min);
}

#undef the_random

#endif // #ifndef CS_GTL_RANDOM32_INLINE_H_INCLUDED