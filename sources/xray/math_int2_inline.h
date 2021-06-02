////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_INT2_INLINE_H_INCLUDED
#define MATH_INT2_INLINE_H_INCLUDED

#include <xray/memory_debug.h>

namespace xray {
namespace math {

inline  int2::int2						( )
{
#ifdef DEBUG
	x			= xray::memory::uninitialized_value <int> ();
	y			= xray::memory::uninitialized_value <int> ();
#endif // #ifdef DEBUG
}

inline int2::int2						( type const other_x, type const other_y )
{
	x			= other_x;
	y			= other_y;
}

inline int2::int2						( int2 const& other )
{
	x			= other.x;
	y			= other.y;
}

inline int2::int2						( int2_pod const& other )
{
	x			= other.x;
	y			= other.y;
}

inline int2& int2_pod::operator -			( )
{
	x			= -x;
	y			= -y;

	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator +=			( int2_pod const& other )
{
	x			+= other.x;
	y			+= other.y;
	
	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator +=			( type const value )
{
	x			+= value;
	y			+= value;

	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator -=			( int2_pod const& other )
{
	x			-= other.x;
	y			-= other.y;

	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator -=			( type const value )
{
	x			-= value;
	y			-= value;

	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator *=			( type const value )
{
	x			*= value;
	y			*= value;

	return		( static_cast<int2&>(*this) );
}

inline int2& int2_pod::operator /=			( type const value )
{
	ASSERT		( value != 0 );

	x			/= value;
	y			/= value;

	return		( static_cast<int2&>(*this) );
}

inline int2::type& int2_pod::operator [ ]	( int index )
{
	ASSERT		( index < 2 );
	return		( elements[ index ] );
}

inline int2& int2_pod::set			( int2_pod::type vx, int2_pod::type vy )
{
	x			= vx;
	y			= vy;
	return		( static_cast<int2&>(*this) );
}

inline	bool int2_pod::similar			( int2_pod const& other ) const
{
	return (x==other.x) && (y==other.y);
}

inline int2 operator+						( int2_pod const& left, int2_pod const& right )
{
	return		( int2 ( left.x + right.x, left.y + right.y ) );
}

inline int2 operator+						( int2_pod const& left, int2_pod::type const& value )
{
	return		( int2 ( left.x + value, left.y + value ) );
}

inline int2 operator-						( int2_pod const& left, int2_pod const& right )
{
	return		( int2 ( left.x - right.x, left.y - right.y ) );
}

inline int2 operator-						( int2_pod const& left, int2_pod::type const& value )
{
	return		( int2 ( left.x - value, left.y - value ) );
}

inline int2 operator*						( int2_pod const& left, int2_pod::type const& value )
{
	return		( int2 ( left.x * value, left.y * value ) );
}

inline int2 operator*						( int2_pod::type const& value, int2_pod const& right )
{
	return		( int2 ( value * right.x , value * right.y ) );
}

inline int2 operator/						( int2_pod const& left, int2_pod::type const& value )
{
	ASSERT		( value !=0 );
	return		( int2 ( left.x / value, left.y / value ) );
}
inline bool	operator ==						( int2_pod const& left, int2_pod const& right )
{
	return ( left.x == right.x && left.y == right.y );
}
inline bool	operator !=						( int2_pod const& left, int2_pod const& right )
{
	return !(left == right);
}

}// namespace math
}// namespace xray


#endif // #ifndef MATH_INT2_INLINE_H_INCLUDED