////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_AABB_INLINE_H_INCLUDED
#define XRAY_MATH_AABB_INLINE_H_INCLUDED

namespace xray {
namespace math {

//private
inline aabb::aabb( )
{}

inline aabb create_zero_aabb	( ) 
{
	aabb res;
	res.min		=	float3( 0.f, 0.f, 0.f );
	res.max		=	float3( 0.f, 0.f, 0.f );
	return res;
}

inline aabb create_identity_aabb( ) 
{
	aabb res;
	res.min		=	float3( -1.f, -1.f, -1.f );
	res.max		=	float3( +1.f, +1.f, +1.f );
	return res;
}

inline aabb create_invalid_aabb	( )
{
	aabb res;
	res.min		=	float3 ( +infinity, +infinity, +infinity );
	res.max		=	float3 ( -infinity, -infinity, -infinity );
	return res;
}

inline aabb create_aabb_min_max	( float3 const& min, float3 const& max ) 
{
	aabb res;
	res.min		=	( min );
	res.max		=	( max );
	return res;
}

inline aabb create_aabb_center_radius	( float3 const& center, float3 const& radius )
{
	R_ASSERT_CMP ( radius.x, >=, 0, "components of radius must be positive" );
	R_ASSERT_CMP ( radius.y, >=, 0, "components of radius must be positive" );
	R_ASSERT_CMP ( radius.z, >=, 0, "components of radius must be positive" );

	aabb res;
	res.min = center - radius ;
	res.max	= center + radius ;
	return res;
}

inline aabb& aabb::zero			( )
{
	min			= max = float3 ( 0.f, 0.f, 0.f );
	return		( *this );
}

inline aabb& aabb::identity		( )
{
	min			= float3 ( -.5f, -.5f, -.5f );
	max			= float3 ( +.5f, +.5f, +.5f );

	return		( *this );
}

inline aabb& aabb::invalidate	( )
{
	min			= float3 ( +infinity, +infinity, +infinity );
	max			= float3 ( -infinity, -infinity, -infinity );

	return		( *this );
}

inline aabb& aabb::operator +=	( type value )
{
	min			-= value;
	max			+= value;

	return		( *this );
}

inline aabb& aabb::operator +=	( float3 const& value )
{
	min			-= value;
	max			+= value;

	return		( *this );
}

inline aabb& aabb::operator -=	( type value )
{
	min			+= value;
	max			-= value;

	return		( *this );
}

inline aabb& aabb::operator -=	( float3 const& value )
{
	min			+= value;
	max			-= value;

	return		( *this );
}

inline aabb& aabb::operator *=	( type value )
{
	float3		extents, center = this->center( extents );
	extents		*= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

inline aabb& aabb::operator *=	( float3 const& value )
{
	float3		extents, center = this->center( extents );
	extents		*= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

inline aabb& aabb::operator /=	( type value )
{
	float3		extents, center = this->center( extents );

	ASSERT		( !is_relatively_zero( math::max(math::abs(extents.x), math::abs(extents.y), math::abs(extents.z)), value ) );
	ASSERT		( !is_relatively_zero( math::max(math::abs(center.x), math::abs(center.y), math::abs(center.z)), value ) );

	extents		/= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

inline aabb& aabb::operator /=	( float3 const& value )
{
	float3		extents, center = this->center( extents );
	extents		/= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

inline aabb& aabb::move			( float3 const& value )
{
	min			+= value;
	max			+= value;

	return		( *this );
}

inline aabb& aabb::modify		( float3 const& position )
{
	min			= math::min( min, position );
	max			= math::max( max, position );

	return		( *this );
}

inline aabb& aabb::modify		( aabb const& other )
{
	modify		( other.min );
	modify		( other.max );

	return		( *this );
}

inline float3 aabb::extents		( ) const
{
	return		( (max - min)*.5f );
}

inline aabb::type aabb::volume	( ) const
{
	float3 const double_extents = max - min;
	return		( double_extents.x * double_extents.y * double_extents.z );
}

inline float3 aabb::center		( ) const
{
	return		( ( min + max )*.5f );
}

inline float3 aabb::center		( float3& size ) const
{
	size		= ( max - min )*.5f;
	return		( min + size );
}

inline sphere aabb::sphere		( ) const
{
	float3 const center = this->center( );
	return		( math::sphere( center, (center - min).length( ) ) );
}

inline bool	aabb::intersect		( aabb const& other ) const
{
	return		( ( max >= other.min ) && ( min <= other.max ) );
}

inline bool	aabb::contains		( float3 const& position ) const
{
	return		( ( min <= position ) && ( max >= position ) );
}

inline bool	aabb::contains		( aabb const& other ) const
{
	return		( ( min <= other.min ) && ( max >= other.max  ) );
}

inline bool	aabb::similar		( aabb const& other ) const
{
	return		( min.is_similar( other.min ) && max.is_similar( other.max ) );
}

inline float3 aabb::vertex		( u32 index ) const
{
	ASSERT		( index < 8 );

	switch ( index ) {
		case 0 : return( float3 ( min.x, min.y, min.z ) );
		case 1 : return( float3 ( min.x, min.y, max.z ) );
		case 2 : return( float3 ( min.x, max.y, min.z ) );
		case 3 : return( float3 ( min.x, max.y, max.z ) );
		case 4 : return( float3 ( max.x, min.y, min.z ) );
		case 5 : return( float3 ( max.x, min.y, max.z ) );
		case 6 : return( float3 ( max.x, max.y, min.z ) );
		case 7 : return( float3 ( max.x, max.y, max.z ) );
		default: NODEFAULT(return float3());
	}
}

inline void	aabb::vertices		( float3 ( &result )[ 8 ] ) const
{
	result[ 0 ]	= float3 ( min.x, min.y, min.z );
	result[ 1 ]	= float3 ( min.x, min.y, max.z );
	result[ 2 ]	= float3 ( min.x, max.y, min.z );
	result[ 3 ]	= float3 ( min.x, max.y, max.z );
	result[ 4 ]	= float3 ( max.x, min.y, min.z );
	result[ 5 ]	= float3 ( max.x, min.y, max.z );
	result[ 6 ]	= float3 ( max.x, max.y, min.z );
	result[ 7 ]	= float3 ( max.x, max.y, max.z );
}

inline bool	aabb::valid			( ) const
{
	return		( min.valid( ) && max.valid( ) );
}

inline aabb	operator +			( aabb const& aabb, aabb::type const value )
{
	math::aabb	result;
	result.min	= aabb.min - value;
	result.max	= aabb.max + value;
	return		( result );
}

inline aabb	operator +			( aabb::type const value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min - value;
	result.max	= aabb.max + value;
	return		( result );
}

inline aabb	operator +			( aabb const& aabb, float3 const& value )
{
	math::aabb	result;
	result.min	= aabb.min - value;
	result.max	= aabb.max + value;
	return		( result );
}

inline aabb	operator +			( float3 const& value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min - value;
	result.max	= aabb.max + value;
	return		( result );
}

inline aabb	operator -			( aabb const& aabb, aabb::type value )
{
	math::aabb	result;
	result.min	= aabb.min + value;
	result.max	= aabb.max - value;
	return		( result );
}

inline aabb	operator -			( aabb::type value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min + value;
	result.max	= aabb.max - value;
	return		( result );
}

inline aabb	operator -			( aabb const& aabb, float3 const& value )
{
	math::aabb	result;
	result.min	= aabb.min + value;
	result.max	= aabb.max - value;
	return		( result );
}

inline aabb	operator -			( float3 const& value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min + value;
	result.max	= aabb.max - value;
	return		( result );
}

inline aabb	operator *			( aabb const& aabb, aabb::type value )
{
	math::aabb	result;
	result.min	= aabb.min * value;
	result.max	= aabb.max * value;
	return		( result );
}

inline aabb	operator *			( aabb::type value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min * value;
	result.max	= aabb.max * value;
	return		( result );
}

inline aabb	operator *			( aabb const& aabb, float3 const& value )
{
	math::aabb	result;
	result.min	= aabb.min * value;
	result.max	= aabb.max * value;
	return		( result );
}

inline aabb	operator *			( float3 const& value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min * value;
	result.max	= aabb.max * value;
	return		( result );
}

inline aabb	operator /			( aabb const& aabb, aabb::type value )
{
	math::aabb	result;
	result.min	= aabb.min / value;
	result.max	= aabb.max / value;
	return		( result );
}

inline aabb	operator /			( aabb::type value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min / value;
	result.max	= aabb.max / value;
	return		( result );
}

inline aabb	operator /			( aabb const& aabb, float3 const& value )
{
	math::aabb	result;
	result.min	= aabb.min / value;
	result.max	= aabb.max / value;
	return		( result );
}

inline aabb	operator /			( float3 const& value, aabb const& aabb )
{
	math::aabb	result;
	result.min	= aabb.min / value;
	result.max	= aabb.max / value;
	return		( result );
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_AABB_INLINE_H_INCLUDED