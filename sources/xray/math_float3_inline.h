////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT3_INLINE_H_INCLUDED
#define XRAY_MATH_FLOAT3_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline float3 float3_pod::operator -			( ) const
{
	return		( float3( -x, -y, -z ) );
}

inline float3& float3_pod::operator +=			( float3_pod const& other )
{
	x			+= other.x;
	y			+= other.y;
	z			+= other.z;
	
	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator +=			( type const value )
{
	x			+= value;
	y			+= value;
	z			+= value;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator -=			( float3_pod const& other )
{
	x			-= other.x;
	y			-= other.y;
	z			-= other.z;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator -=			( type const value )
{
	x			-= value;
	y			-= value;
	z			-= value;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator *=			( type const value )
{
	x			*= value;
	y			*= value;
	z			*= value;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator *=			( float3_pod const& value )
{
	x			*= value.x;
	y			*= value.y;
	z			*= value.z;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator /=			( type const value )
{
	ASSERT		( !is_relatively_zero( math::max(math::abs(x), math::abs(y), math::abs(z)), value ) );

	float const	invert_value = 1.f / value;
	*this		*= invert_value;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator /=			( float3_pod const& value )
{
	ASSERT		( !is_relatively_zero( x, value.x ) );
	ASSERT		( !is_relatively_zero( y, value.y ) );
	ASSERT		( !is_relatively_zero( z, value.z ) );

	x			/= value.x;
	y			/= value.y;
	z			/= value.z;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::operator ^=			( float3_pod const& other )
{
	return		( cross_product( other ) );
}

inline float3_pod::type& float3_pod::operator [ ]	( int index )
{
	ASSERT		( index < 3 );
	return		( elements[ index ] );
}
inline float3_pod::type const& float3_pod::operator [ ]	( int index )const
{
	ASSERT		( index < 3 );
	return		( elements[ index ] );
}
inline float3& float3_pod::set					( float3_pod::type vx, float3_pod::type vy, float3_pod::type vz )
{
	x			= vx;
	y			= vy;
	z			= vz;
	return		( static_cast<float3&>(*this) );
}

inline float3::type float3_pod::dot_product		( float3_pod const& other ) const
{
	return		( x*other.x + y*other.y + z*other.z );
}

inline float3& float3_pod::cross_product		( float3_pod const& other )
{
	float3_pod	self = *this;

	x			= self.y*other.z - self.z*other.y;
	y			= self.z*other.x - self.x*other.z;
	z			= self.x*other.y - self.y*other.x;

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::normalize			( )
{
	return		( *this /= length( ) );
}

inline float3_pod::type float3_pod::normalize_r		( )
{
	type const	length = this->length( );
	*this		/= length;
	return		( length );
}

inline float3& float3_pod::normalize_safe		( float3_pod const& result_in_case_of_zero )
{
	type const	length = this->length( );
	if ( !is_relatively_zero( math::max( math::abs(x), math::abs(y), math::abs(z)), length ) )
		return	( *this	/= length );

	return		( 	static_cast<float3&>( *this = result_in_case_of_zero) );
}

inline float3_pod::type float3_pod::normalize_safe_r( float3_pod const& result_in_case_of_zero )
{
	type const	length = this->length( );
	if ( !is_relatively_zero( math::max( math::abs(x), math::abs(y), math::abs(z)), length ) ) {
		*this	/= length;
		return	( length );
	}

	*this		= result_in_case_of_zero;
	return		( length );
}

inline float3& float3_pod::abs					( )
{
	x			= math::abs( x );
	y			= math::abs( y );
	z			= math::abs( z );

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::min					( float3_pod const& other )
{
	x			= math::min( x, other.x );
	y			= math::min( y, other.y );
	z			= math::min( z, other.z );

	return		( static_cast<float3&>(*this) );
}

inline float3& float3_pod::max					( float3_pod const& other )
{
	x			= math::max( x, other.x );
	y			= math::max( y, other.y );
	z			= math::max( z, other.z );

	return		( static_cast<float3&>(*this) );
}

inline float3_pod::type float3_pod::length		( ) const
{
	return		( sqrt( squared_length( ) ) );
}

inline float3_pod::type float3_pod::squared_length( ) const
{
	return		( sqr( x ) + sqr( y )+ sqr( z ) );
}


inline void float3_pod::set_length			( float length )
{
	*this		*= length/this->length();
}

inline bool float3_pod::is_similar					( float3_pod const& other, float epsilon ) const
{
	if ( !math::is_similar( x, other.x, epsilon ) )
		return	( false );

	if ( !math::is_similar( y, other.y, epsilon ) )
		return	( false );

	if ( !math::is_similar( z, other.z, epsilon ) )
		return	( false );

	return		( true );
}

#ifndef MASTER_GOLD
inline bool float3_pod::valid					( ) const
{
	return		( math::valid( x ) && math::valid( y ) && math::valid( z ) );
}
#endif // #ifndef MASTER_GOLD

inline float3 operator +					( float3_pod const& left, float3_pod const& right )
{
	return		( float3( left.x + right.x, left.y + right.y, left.z + right.z ) );
}

inline float3 operator +					( float3_pod const& left, float3_pod::type const& value )
{
	return		( float3( left.x + value, left.y + value, left.z + value ) );
}

inline float3 operator -					( float3_pod const& left, float3_pod const& right )
{
	return		( float3( left.x - right.x, left.y - right.y, left.z - right.z ) );
}

inline float3 operator -					( float3_pod const& left, float3_pod::type const& value )
{
	return		( float3( left.x - value, left.y - value, left.z - value ) );
}

inline float3 operator *					( float3_pod const& left, float3_pod::type const& value )
{
	return		( float3( left.x * value, left.y * value, left.z * value ) );
}

inline float3 operator *					( float3_pod::type const& value, float3_pod const& right )
{
	return		( float3( value * right.x, value * right.y, value * right.z ) );
}

inline float3 operator *					( float3_pod const& left, float3_pod const& right )
{
	return		( float3( left.x * right.x, left.y * right.y, left.z * right.z ) );
}

inline float3 operator /					( float3_pod const& left, float3_pod::type const& value )
{
	ASSERT		( value != 0.f );
	return		( float3( left.x/value, left.y/value, left.z/value ) );
}

inline float3 operator /					( float3_pod::type const& left, float3_pod const& right )
{
	ASSERT		( right.x != 0.f );
	ASSERT		( right.y != 0.f );
	ASSERT		( right.z != 0.f );

	return		( float3( left / right.x, left / right.y, left / right.z ) );
}

inline float3 operator /					( float3_pod const& left, float3_pod const& right )
{
	ASSERT		( right.x != 0.f );
	ASSERT		( right.y != 0.f );
	ASSERT		( right.z != 0.f );

	return		( float3( left.x / right.x, left.y / right.y, left.z / right.z ) );
}

inline float3::type dot_product				( float3_pod const& left, float3_pod const& right )
{
	return		( left.dot_product( right ) );
}

inline float3::type operator |				( float3_pod const& left, float3_pod const& right )
{
	return		( left.dot_product( right ) );
}

inline bool operator <						( float3_pod const& left, float3_pod const& right )
{
	return		( ( left.x < right.x ) && ( left.y < right.y ) && ( left.z < right.z ) );
}

inline bool operator <=						( float3_pod const& left, float3_pod const& right )
{
	return		( ( left.x <= right.x ) && ( left.y <= right.y ) && ( left.z <= right.z ) );
}

inline bool operator >						( float3_pod const& left, float3_pod const& right )
{
	return		( ( left.x > right.x ) && ( left.y > right.y ) && ( left.z > right.z ) );
}

inline bool operator >=						( float3_pod const& left, float3_pod const& right )
{
	return		( ( left.x >= right.x ) && ( left.y >= right.y ) && ( left.z >= right.z ) );
}

inline bool	operator !=						( float3_pod const& left, float3_pod const& right )
{
	return		!(left == right);
}

inline bool	operator ==						( float3_pod const& left, float3_pod const& right )
{
	return		( ( left.x == right.x ) && ( left.y == right.y ) && ( left.z == right.z ) );
}


inline bool less_by_order					( float3_pod const& left, float3_pod const& right )
{
	if ( left.x < right.x )
		return	true;

	if ( left.x > right.x )
		return	false;

	if ( left.y < right.y )
		return	true;

	if ( left.y > right.y )
		return	false;

	return		left.z < right.z;
}

inline bool less_equal_by_order				( float3_pod const& left, float3_pod const& right )
{
	if ( left.x < right.x )
		return	true;

	if ( left.x > right.x )
		return	false;

	if ( left.y < right.y )
		return	true;

	if ( left.y > right.y )
		return	false;

	return		left.z <= right.z;
}

inline bool greater_by_order				( float3_pod const& left, float3_pod const& right )
{
	if ( left.x > right.x )
		return	true;

	if ( left.x < right.x )
		return	false;

	if ( left.y > right.y )
		return	true;

	if ( left.y < right.y )
		return	false;

	return		left.z > right.z;
}

inline bool greater_equal_by_order			( float3_pod const& left, float3_pod const& right )
{
	if ( left.x > right.x )
		return	true;

	if ( left.x < right.x )
		return	false;

	if ( left.y > right.y )
		return	true;

	if ( left.y < right.y )
		return	false;

	return		left.z >= right.z;
}

inline float3 cross_product					( float3_pod const& left, float3_pod const& right )
{
	float3		result;
	result.x	= left.y*right.z - left.z*right.y;
	result.y	= left.z*right.x - left.x*right.z;
	result.z	= left.x*right.y - left.y*right.x;
	return		( result );
}

inline float3 operator ^					( float3_pod const& left, float3_pod const& right )
{
	return		( cross_product( left, right ) );
}

inline float3 normalize		( float3_pod const& object )
{
	float3_pod::type const length = object.length( );
	R_ASSERT	( !is_relatively_zero( max( abs(object.x), abs(object.y), abs(object.z)), length ) );
	return		( float3( object.x/length, object.y/length, object.z/length ) );
}

inline float3 normalize_safe( float3_pod const& object, float3_pod const& result_in_case_of_zero )
{
	float3_pod::type const length = object.length( );
	if ( !is_relatively_zero( max( abs(object.x), abs(object.y), abs(object.z)), length ) )
		return	( float3( object.x/length, object.y/length, object.z/length ) );

	return		( result_in_case_of_zero );
}

inline float3 min		( float3_pod const& left, float3_pod const& right )
{
	float3		result;
	result.x	= min( left.x, right.x );
	result.y	= min( left.y, right.y );
	result.z	= min( left.z, right.z );
	return		( result );
}

inline float3 max		( float3_pod const& left, float3_pod const& right )
{
	float3		result;
	result.x	= max( left.x, right.x );
	result.y	= max( left.y, right.y );
	result.z	= max( left.z, right.z );
	return		( result );
}

inline float3 abs		( float3_pod const& object )
{
	return			( float3( abs(object.x), abs(object.y), abs(object.z) ) );
}

inline float length	( float3_pod const& object )
{
	return			( object.length( ) );
}

inline float squared_length	( float3_pod const& object )
{
	return			( object.squared_length( ) );
}

inline bool is_similar		( float3_pod const& left, float3_pod const& right, float epsilon )
{
	return			( left.is_similar( right, epsilon ) );
}

inline bool	is_relatively_similar	( float3 const& left, float3 const& right, float epsilon )
{
	return			
		is_relatively_similar(left.x, right.x, epsilon) &&
		is_relatively_similar(left.y, right.y, epsilon) &&
		is_relatively_similar(left.z, right.z, epsilon);
}

#ifndef MASTER_GOLD
inline bool valid		( float3_pod const& object )
{
	return			( object.valid( ) );
}
#endif // #ifndef MASTER_GOLD

inline float3 pow		( float3_pod const& object, float3_pod::type power )
{
	return			float3( pow(object.x, power), pow(object.y, power), pow(object.z, power) );
}

inline float3 pow		( float3_pod const& object, float3_pod const& power )
{
	return			float3( pow(object.x, power.x), pow(object.y, power.y), pow(object.z, power.z) );
}

inline float3::float3						( ) 
{
#ifdef DEBUG
	x			= SNaN;
	y			= SNaN;
	z			= SNaN;
#endif // #ifdef DEBUG
}

inline float3::float3						( type const other_x, type const other_y, type const other_z )
{
	x			= other_x;
	y			= other_y;
	z			= other_z;
}

inline float3::float3						( float3_pod const& other ) 
{
	x			= other.x;
	y			= other.y;
	z			= other.z;
}

inline float3 min		( float3 const& left, float3 const& right )
{
	float3		result;
	result.x	= min( left.x, right.x );
	result.y	= min( left.y, right.y );
	result.z	= min( left.z, right.z );
	return		( result );
}

inline float3 max		( float3 const& left, float3 const& right )
{
	float3		result;
	result.x	= max( left.x, right.x );
	result.y	= max( left.y, right.y );
	result.z	= max( left.z, right.z );
	return		( result );
}

inline float3 abs		( float3 const& object )
{
	return			( float3( abs(object.x), abs(object.y), abs(object.z) ) );
}

inline float length	( float3 const& object )
{
	return			( object.length( ) );
}

inline float squared_length	( float3 const& object )
{
	return			( object.squared_length( ) );
}

inline bool is_similar		( float3 const& left, float3 const& right, float epsilon )
{
	return			( left.is_similar( right, epsilon ) );
}

inline bool valid		( float3 const& object )
{
	return			( object.valid( ) );
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_FLOAT3_INLINE_H_INCLUDED
