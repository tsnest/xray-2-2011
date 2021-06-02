////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef AABB_INDICES_H_INCLUDED
#define AABB_INDICES_H_INCLUDED

namespace xray {
namespace render {

u16 aabb_indices[12*2] =  
{
	0, 1,
	0, 2,
	0, 3,

	7, 6,
	7, 5,
	7, 4,

	1, 4,
	1, 5,

	2, 4,
	2, 6,

	3, 5,
	3, 6

}; // class aabb_indices

} // namespace render
} // namespace xray

#endif // #ifndef AABB_INDICES_H_INCLUDED