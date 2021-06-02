////////////////////////////////////////////////////////////////////////////
//	Module 		: random32.h
//	Created 	: 28.05.2007
//	Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : 32-bit pseudo random number generator
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_RANDOM32_H_INCLUDED
#define CS_GTL_RANDOM32_H_INCLUDED

namespace gtl {

class random32 {
private:
	u32					m_seed;

private:
	inline void			update_seed	();

public:
	inline u32 			seed		();
	inline void			seed		(u32 const seed);
	inline u32			random		(u32 const range);
	inline u32			random		();
	inline float		random_f	();
	inline float		random_f	(float const max);
	inline float		random_f	(float const min, float const max);
};

}

#include <cs/gtl/random32_inline.h>

#endif // #ifndef CS_GTL_RANDOM32_H_INCLUDED