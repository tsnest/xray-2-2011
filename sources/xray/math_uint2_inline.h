////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_UINT2_INLINE_H_INCLUDED
#define XRAY_MATH_UINT2_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline uint2::uint2						( ) 
{
#ifdef DEBUG
	x	=	xray::memory::uninitialized_value <int> ();
	y	=	xray::memory::uninitialized_value <int> ();
#endif // #ifdef DEBUG
}

inline uint2::uint2						( type const _x, type const _y )
{
	x	= _x;
	y	= _y;
}

inline uint2::uint2						( uint2 const& other )
{
	x			= other.x;
	y			= other.y;
}

inline uint2::uint2						( uint2_pod const& other )
{
	x			= other.x;
	y			= other.y;
}

inline uint2& uint2_pod::operator +=			( uint2_pod const& other )
{
	x			+= other.x;
	y			+= other.y;
	
	return		( *(uint2*)this );
}

inline uint2& uint2_pod::operator +=			( type const value )
{
	x			+= value;
	y			+= value;

	return		( *(uint2*)this );
}

inline uint2& uint2_pod::operator -=			( uint2_pod const& other )
{
	ASSERT( x >= other.x && y >= other.y  );

	x			-= other.x;
	y			-= other.y;

	return		( *(uint2*)this );
}

inline uint2& uint2_pod::operator -=			( type const value )
{
	ASSERT( x >= value && y >= value  );

	x			-= value;
	y			-= value;

	return		( *(uint2*)this );
}

inline uint2& uint2_pod::operator *=			( type const value )
{
	x			*= value;
	y			*= value;

	return		( *(uint2*)this );
}

inline uint2& uint2_pod::operator /=			( type const value )
{
	ASSERT		( value != 0 );

	x			/= value;
	y			/= value;

	return		( *(uint2*)this );
}

inline uint2::type& uint2_pod::operator [ ]	( int index )
{
	ASSERT		( index < 2 );
	return		( elements[ index ] );
}

inline uint2& uint2_pod::set			( type vx, type vy )
{
	x			= vx;
	y			= vy;
	return		( *(uint2*)this );
}

inline uint2 operator+						( uint2 const& left, uint2 const& right )
{
	return		( uint2 ( left.x + right.x, left.y + right.y ) );
}

inline uint2 operator+						( uint2 const& left, uint2::type const& value )
{
	return		( uint2 ( left.x + value, left.y + value ) );
}

inline uint2 operator-						( uint2 const& left, uint2 const& right )
{
	ASSERT( left.x >= right.x && left.y >= right.y);
	return		( uint2 ( left.x - right.x, left.y - right.y ) );
}

inline uint2 operator-						( uint2 const& left, uint2::type const& value )
{
	ASSERT( left.x >= value && left.y >= value);
	return		( uint2 ( left.x - value, left.y - value ) );
}

inline uint2 operator*						( uint2 const& left, uint2::type const& value )
{
	return		( uint2 ( left.x * value, left.y * value ) );
}

inline uint2 operator*						( uint2::type const& value, uint2 const& right )
{
	return		( uint2 ( value * right.x , value * right.y ) );
}

inline uint2 operator/						( uint2 const& left, uint2::type const& value )
{
	ASSERT		( value !=0 );
	return		( uint2 ( left.x / value, left.y / value ) );
}
inline bool	operator ==						( uint2 const& left, uint2 const& right )
{
	return ( left.x == right.x && left.y == right.y );
}
inline bool	operator !=		( uint2 const& left, uint2 const& right )
{
	return !(left == right);
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_UINT2_INLINE_H_INCLUDED