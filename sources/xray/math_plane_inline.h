////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_PLANE_INLINE_H_INCLUDED
#define XRAY_MATH_PLANE_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline plane::plane									( )
#ifdef DEBUG
:	vector			( float4() )
#endif // #ifdef DEBUG
{
}

inline plane::plane									( float3_pod const& normal, type const distance ) :
	normal			( normal ),
	d				( distance )
{
}

inline bool plane::similar							( plane const& other ) const
{
	return			( vector.is_similar( other.vector ) );
}

inline float3 plane::project						( float3 const& vertex ) const
{
	return			( vertex - normal*classify( vertex ) );
}

inline plane::type plane::classify					( float3 const& vertex ) const
{
	return			( ( normal | vertex ) + d );
}

inline plane::type plane::distance					( float3 const& vertex ) const
{
	return			( abs( classify( vertex) ) );
}

inline plane& plane::normalize						( )
{
	vector			/= normal.length( );
	return			( * this );
}

inline bool plane::intersect_ray					( float3 const& position, float3 const& direction, float& distance ) const
{
	float const		numerator = classify( position );
	float const		denominator = normal | direction;

	if ( abs( denominator ) < epsilon_7 )  // normal is orthogonal to vector3, cant intersect
		return	( false );

	distance		= -(numerator / denominator);
	return			( ( distance > 0.f ) || is_zero( distance ) );
}

inline bool plane::intersect_ray					( float3 const& position, float3 const& direction, float3& intersection ) const
{
    R_ASSERT                            ( xray::math::valid( normal ) && xray::math::valid( d ) );
    R_ASSERT                            ( xray::math::valid( position ) );
    R_ASSERT                            ( xray::math::valid( direction ) );

    float const signed_distance         = classify( position );
    float const cosine_alpha            = normal | direction;
    const float distance                = cosine_alpha != 0.f ? signed_distance/-cosine_alpha : -xray::math::abs( signed_distance );
	intersection						= position + direction * distance;
    return                              distance >= 0.f;

}

inline bool plane::intersect_segment				( xray::math::float3 const& first, xray::math::float3 const& second, xray::math::float3& intersection_position ) const
{
    xray::math::float3 const& second_minus_first    = second - first;
    float const signed_projection					= normal | second_minus_first;
    float const first_signed_distance				= classify( first );
    float const distance							= signed_projection != 0.f ? first_signed_distance/-signed_projection : -xray::math::abs(first_signed_distance);
    intersection_position							= first + second_minus_first * distance;
	return (distance >= 0.f) && (distance <= 1.f);
}

inline plane& plane::transform						( float4x4 const& right )
{
	vector				= right.transform( vector );
	normalize			( );
	return				*this;
}

#ifndef MASTER_GOLD

inline bool plane::valid							( ) const
{
	return			( vector.valid( ) );
}

#endif

inline plane transform								( plane const& left, float4x4 const& right )
{
	return			( plane( left ).transform( right ) );
}

inline plane create_plane_normalized				( float3 const& normalized_normal, float3 const& point_on_plane )
{
	plane			result;
	result.normal	= normalized_normal;
	result.d		= -(normalized_normal | point_on_plane);
	return			result;
}

inline float3 get_plane_normal_non_normalized		( float3 const& first, float3 const& second, float3 const& third )
{
	return			(second - first) ^ (third - first);
}

inline float3 get_plane_normal						( float3 const& first, float3 const& second, float3 const& third )
{
	return			normalize( get_plane_normal_non_normalized(first, second, third) );
}

inline plane create_plane							( float3 const& first, float3 const& second, float3 const& third )
{
	return			create_plane_normalized( normalize( get_plane_normal_non_normalized( first, second, third ) ), first );
}

inline plane create_plane_precise					( float3 const& first, float3 const& second, float3 const& third )
{
	return			create_plane_normalized( normalize_precise( get_plane_normal_non_normalized( first, second, third ) ), first );
}

inline plane create_plane							( float3 const ( &vertices )[ 3 ] )
{
	return			create_plane( vertices[0], vertices[1], vertices[2] );
}

inline plane create_plane_precise					( float3 const ( &vertices )[ 3 ] )
{
	return			create_plane_precise( vertices[0], vertices[1], vertices[2] );
}




} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_PLANE_INLINE_H_INCLUDED