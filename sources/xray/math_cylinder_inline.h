////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_CYLINDER_INLINE_H_INCLUDED
#define XRAY_MATH_CYLINDER_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline cylinder::cylinder				( )
#ifdef DEBUG
:	radius		( SNaN ),
	height		( SNaN )
#endif // #ifdef DEBUG
{
}

#ifndef MASTER_GOLD

inline bool cylinder::valid		( ) const
{
	return		( center.valid( ) && direction.valid( ) && math::valid( height ) && math::valid( radius ) );
}

#endif

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_CYLINDER_INLINE_H_INCLUDED