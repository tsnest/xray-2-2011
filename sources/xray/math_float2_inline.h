////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT2_INLINE_H_INCLUDED
#define XRAY_MATH_FLOAT2_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline float2& float2_pod::operator -				( )
{
	x			= -x;
	y			= -y;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator +=			( float2_pod const& other )
{
	x			+= other.x;
	y			+= other.y;
	
	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator +=			( type const value )
{
	x			+= value;
	y			+= value;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator -=			( float2_pod const& other )
{
	x			-= other.x;
	y			-= other.y;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator -=			( type const value )
{
	x			-= value;
	y			-= value;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator *=			( type const value )
{
	x			*= value;
	y			*= value;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator /=			( type const value )
{
	ASSERT		( !is_relatively_zero( math::max(math::abs(x), math::abs(y)), value ) );

	x			/= value;
	y			/= value;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::operator ^=			( float2_pod const& other )
{
	return		( cross_product( other ) );
}

inline float2::type& float2_pod::operator [ ]	( int index )
{
	ASSERT		( index < 2 );
	return		( elements[ index ] );
}

inline const float2::type& float2_pod::operator [ ]	( int index ) const
{
	ASSERT		( index < 2 );
	return		( elements[ index ] );
}

inline float2& float2_pod::set					( float2_pod::type vx, float2_pod::type vy )
{
	x			= vx;
	y			= vy;
	return		( static_cast<float2&>(*this) );
}

inline float2::type float2_pod::dot_product		( float2_pod const& other ) const
{
	return		( x*other.x + y*other.y );
}

inline float2& float2_pod::cross_product		( float2_pod const& other )
{
	x			= other.y;
	y			= -other.x;

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::normalize			( )
{
	return		( *this /= length( ) );
}

inline float2::type float2_pod::normalize_r		( )
{
	type const	length = this->length( );
	*this		/= length;
	return		( length );
}

inline float2& float2_pod::normalize_safe		( float2_pod const& result_in_case_of_zero )
{
	type const	length = this->length( );
	if ( !is_relatively_zero( math::max(math::abs(x), math::abs(y)), length ) )
		return	( *this	/= length );

	*this		= result_in_case_of_zero;
	return		( static_cast<float2&>(*this) );
}

inline float2::type float2_pod::normalize_safe_r( float2_pod const& result_in_case_of_zero )
{
	type const	length = this->length( );
	if ( !is_relatively_zero( math::max(math::abs(x), math::abs(y)), length ) ) {
		*this	/= length;
		return	( length );
	}

	*this		= result_in_case_of_zero;
	return		( length );
}

inline float2& float2_pod::abs					( )
{
	x			= math::abs( x );
	y			= math::abs( y );

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::min					( float2_pod const& other )
{
	x			= math::min( x, other.x );
	y			= math::min( y, other.y );

	return		( static_cast<float2&>(*this) );
}

inline float2& float2_pod::max					( float2_pod const& other )
{
	x			= math::max( x, other.x );
	y			= math::max( y, other.y );

	return		( static_cast<float2&>(*this) );
}

inline float2::type float2_pod::length		( ) const
{
	return		( sqrt( squared_length( ) ) );
}

inline float2::type float2_pod::squared_length( ) const
{
	return		( sqr( x ) + sqr( y ) );
}

inline void float2_pod::set_length			( float length )
{
	*this		*= length/this->length();
}

inline bool float2_pod::is_similar					( float2_pod const& other, float epsilon ) const
{
	if ( !math::is_similar( x, other.x, epsilon ) )
		return	( false );

	if ( !math::is_similar( y, other.y, epsilon ) )
		return	( false );

	return		( true );
}

#ifndef MASTER_GOLD

inline bool float2_pod::valid					( ) const
{
	return		( math::valid( x ) && math::valid( y ) );
}

#endif

inline float2::float2							( ) 
{
	x			= SNaN;
	y			= SNaN;
}

inline float2::float2							( type const other_x, type const other_y )
{
	x			= other_x;
	y			= other_y;
}

inline float2::float2							( float2 const& other )
{
	x			= other.x;
	y			= other.y;
}

inline float2::float2							( float2_pod const& other )
{
	x			= other.x;
	y			= other.y;
}

inline float2 operator+							( float2_pod const& left, float2_pod const& right )
{
	return		( float2 ( left.x + right.x, left.y + right.y ) );
}

inline float2 operator+							( float2_pod const& left, float2_pod::type const& value )
{
	return		( float2 ( left.x + value, left.y + value ) );
}

inline float2 operator-							( float2_pod const& left, float2_pod const& right )
{
	return		( float2 ( left.x - right.x, left.y - right.y ) );
}

inline float2 operator-							( float2_pod const& left, float2_pod::type const& value )
{
	return		( float2 ( left.x - value, left.y - value ) );
}

inline float2 operator*							( float2_pod const& left, float2_pod::type const& value )
{
	return		( float2 ( left.x * value, left.y * value ) );
}

inline float2 operator*							( float2_pod::type const& value, float2_pod const& right )
{
	return		( float2 ( value * right.x , value * right.y ) );
}

inline float2 operator/							( float2_pod const& left, float2_pod::type const& value )
{
	ASSERT		( !is_relatively_zero( max(abs(left.x), abs(left.y)), value ) );
	return		( float2 ( left.x / value, left.y / value ) );
}

inline float2 operator /						( float2_pod::type const& left, float2_pod const& right )
{
	ASSERT		( !is_relatively_zero( left, right.x ) );
	ASSERT		( !is_relatively_zero( left, right.y ) );

	return		( float2 ( left / right.x, left / right.y ) );
}

inline bool operator <							( float2_pod const& left, float2_pod const& right )
{
	return		( ( left.x < right.x ) && ( left.y < right.y ) );
}

inline bool operator <=							( float2_pod const& left, float2_pod const& right )
{
	return		( ( left.x <= right.x ) && ( left.y <= right.y ) );
}

inline bool operator >							( float2_pod const& left, float2_pod const& right )
{
	return		( ( left.x > right.x ) && ( left.y > right.y ) );
}

inline bool operator >=							( float2_pod const& left, float2_pod const& right )
{
	return		( ( left.x >= right.x ) && ( left.y >= right.y ) );
}

inline bool	operator !=							( float2_pod const& left, float2_pod const& right )
{
	return		!(left == right);
}

inline bool	operator ==							( float2_pod const& left, float2_pod const& right )
{
	return		( ( left.x == right.x ) && ( left.y == right.y ) );
}

inline float2::type dot_product					( float2_pod const& left, float2_pod const& right )
{
	return		( left.dot_product( right ) );
}

inline float2::type operator |					( float2_pod const& left, float2_pod const& right )
{
	return		( dot_product( left, right ) );
}

inline float2 cross_product						( float2_pod const& object )
{
	return		( float2( object.y, -object.x )  );
}

inline float2 normalize							( float2_pod const& object )
{
	float2::type const length = object.length( );
	ASSERT		( !is_relatively_zero( math::max(math::abs(object.x), math::abs(object.y)), length ) );
	return		( float2( object.x/length, object.y/length ) );
}

inline float2 normalize_safe					( float2_pod const& object, float2_pod const& result_in_case_of_zero )
{
	float2::type const length = object.length( );
	if ( !is_relatively_zero( math::max(math::abs(object.x), math::abs(object.y)), length ) )
		return	( float2( object.x/length, object.y/length ) );

	return		( float2( result_in_case_of_zero ) );
}

inline float2 pow		( float2_pod const& object, float2_pod::type power )
{
	return			float2( pow(object.x, power), pow(object.y, power) );
}

inline float2 pow		( float2_pod const& object, float2_pod const& power )
{
	return			float2( pow(object.x, power.x), pow(object.y, power.y) );
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_FLOAT2_INLINE_H_INCLUDED