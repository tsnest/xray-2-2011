////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_SPHERE_INLINE_H_INCLUDED
#define XRAY_MATH_SPHERE_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline sphere::sphere				( )
#ifdef DEBUG
:	vector			( float4() )
#endif // #ifdef DEBUG
{
}

inline sphere::sphere				( float3 const& center, type radius ) :
	center			( center ),
	radius			( radius )
{
}

inline bool sphere::intersect_ray	( float3 const& position, float3 const& direction ) const
{
	float3 const	position2center = center - position;
	return			( sqr( radius ) + sqr( position2center | direction ) > sqr( position2center.length( ) ) );
}

inline bool sphere::intersect		( sphere const& other ) const
{
	return			( ( center - other.center ).squared_length( ) > sqr( radius + other.radius ) );
}

inline bool sphere::contains		( float3 const& position )
{
	return			( ( center - position ).squared_length( ) < sqr( radius ) );
}

inline bool sphere::contains		( sphere const& other )
{
	type const		radius_difference = radius - other.radius;
	if ( radius_difference < 0.f )
		return		( false );

	return			( ( center - other.center ).squared_length( ) < sqr( radius_difference ) );
}

inline bool sphere::similar			( sphere const& other ) const
{
	return			( vector.is_similar( other.vector ) );
}

#ifndef MASTER_GOLD

inline bool sphere::valid			( ) const
{
	return			( vector.valid( ) );
}

#endif

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_SPHERE_INLINE_H_INCLUDED