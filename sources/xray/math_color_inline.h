////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_COLOR_INLINE_H_INCLUDED
#define XRAY_MATH_COLOR_INLINE_H_INCLUDED

namespace xray {
namespace math {

//	inline color& color_pod::operator -			( )
//	{
//		r			= -r;
//		g			= -g;
//		b			= -b;
//		a			= -a;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator +=			( color_pod const& other )
//	{
//		r			+= other.r;
//		g			+= other.g;
//		b			+= other.b;
//		a			+= other.a;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator +=			( type const value )
//	{
//		r			+= value;
//		g			+= value;
//		b			+= value;
//		a			+= value;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator -=			( color_pod const& other )
//	{
//		r			-= other.r;
//		g			-= other.g;
//		b			-= other.b;
//		a			-= other.a;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator -=			( type const value )
//	{
//		r			-= value;
//		g			-= value;
//		b			-= value;
//		a			-= value;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator *=			( type const value )
//	{
//		r			*= value;
//		g			*= value;
//		b			*= value;
//		a			*= value;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::operator /=			( type const value )
//	{
//		ASSERT		( !is_zero( value ) );
//
//		r			/= value;
//		g			/= value;
//		b			/= value;
//		a			/= value;
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline float3& color_pod::rgb					( )
//	{
//		return		( ( float3& )( *this ) );
//	}
//
//	inline float3 const& color_pod::rgb			( ) const
//	{
//		return		( ( float3& )( *this ) );
//	}
//
//	inline color_pod::type& color_pod::operator [ ]	( int index )
//	{
//		ASSERT		( index < 4 );
//		return		( elements[ index ] );
//	}
//
//	inline color_pod::type const& color_pod::operator [ ]	( int index )const
//	{
//		ASSERT		( index < 4 );//4 ?
//		return		( elements[ index ] );
//	}
//
//	inline color& color_pod::set					( color_pod::type vr, color_pod::type vg, color_pod::type vb, color_pod::type va )
//	{
//		r			= vr;
//		g			= vg;
//		b			= vb;
//		a			= va;
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::normalize			( )
//	{
//		return		( *this /= length( ) );
//	}
//
//	inline color_pod::type color_pod::normalize_r		( )
//	{
//		type const	length = this->length( );
//		*this		/= length;
//		return		( length );
//	}
//
//	inline color& color_pod::normalize_safe		( color_pod const& result_in_case_of_zero )
//	{
//		type const	length = this->length( );
//		if ( !is_zero( length ) )
//			return	( *this	/= length );
//
//		return		( static_cast<color&>(*this = result_in_case_of_zero) );
//	}
//
//	inline color::type color_pod::normalize_safe_r( color_pod const& result_in_case_of_zero )
//	{
//		type const	length = this->length( );
//		if ( !is_zero( length ) ) {
//			*this	/= length;
//			return	( length );
//		}
//
//		*this		= result_in_case_of_zero;
//		return		( length );
//	}
//
//	inline color& color_pod::abs					( )
//	{
//		r			= math::abs( r );
//		g			= math::abs( g );
//		b			= math::abs( b );
//		a			= math::abs( a );
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::min					( color_pod const& other )
//	{
//		r			= math::min( r, other.r );
//		g			= math::min( g, other.g );
//		b			= math::min( b, other.b );
//		a			= math::min( a, other.a );
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color& color_pod::max					( color_pod const& other )
//	{
//		r			= math::max( r, other.r );
//		g			= math::max( g, other.g );
//		b			= math::max( b, other.b );
//		a			= math::max( a, other.a );
//
//		return		( static_cast<color&>(*this) );
//	}
//
//	inline color_pod::type color_pod::length		( ) const
//	{
//		return		( sqrt( squared_length( ) ) );
//	}
//
//	inline color_pod::type color_pod::squared_length( ) const
//	{
//		return		( sqr( r ) + sqr( g ) + sqr( b ) + sqr( a ) );
//	}
//
//	inline bool color_pod::similar					( color_pod const& other, float epsilon ) const
//	{
//		if ( !math::is_similar( r, other.r, epsilon ) )
//			return	( false );
//
//		if ( !math::is_similar( g, other.g, epsilon ) )
//			return	( false );
//
//		if ( !math::is_similar( b, other.b, epsilon ) )
//			return	( false );
//
//		if ( !math::is_similar( a, other.a, epsilon ) )
//			return	( false );
//
//		return		( true );
//	}
//
//	inline bool color_pod::valid					( ) const
//	{
//		return		( math::valid( r ) && math::valid( g ) && math::valid( b ) && math::valid( a ) );
//	}
//
//
//	inline float dot_product					( const color_pod &left, const color_pod &right )
//	{
//		return left.r * right.r + left.g * right.g + left.b * right.b + left.a * right.a ;
//	}
//
//
//	inline color operator +					( color_pod const& left, color_pod const& right )
//	{
//		return		( color ( left.r + right.r, left.g + right.g, left.b + right.b, left.a + right.a ) );
//	}
//
//	inline color operator +					( color_pod const& left, color_pod::type const& value )
//	{
//		return		( color ( left.r + value, left.g + value, left.b + value, left.a + value ) );
//	}
//
//	inline color operator -					( color_pod const& left, color_pod const& right )
//	{
//		return		( color ( left.r - right.r, left.g - right.g, left.b - right.b, left.a - right.a ) );
//	}
//
//	inline color operator -					( color_pod const& left, color_pod::type const& value )
//	{
//		return		( color ( left.r - value, left.g - value, left.b - value, left.a - value ) );
//	}
//
//	inline color operator *					( color_pod const& left, color_pod::type const& value )
//	{
//		return		( color ( left.r * value, left.g * value, left.b * value, left.a * value ) );
//	}
//
//	inline color operator *					( color_pod::type const& value, color_pod const& right )
//	{
//		return		( color ( value * right.r, value * right.g, value * right.b, value * right.a ) );
//	}
//
//	inline color operator /					( color_pod const& left, color_pod::type const& value )
//	{
//		ASSERT		( !is_zero( value ) );
//		return		( color ( left.r/value, left.g/value, left.b/value, left.a/value ) );
//	}
//
//	inline bool operator <						( color_pod const& left, color_pod const& right )
//	{
//		return		( ( left.r < right.r ) && ( left.g < right.g ) && ( left.b < right.b )&& ( left.a < right.a ) );
//	}
//
//	inline bool operator <=						( color_pod const& left, color_pod const& right )
//	{
//		return		( ( left.r <= right.r ) && ( left.g <= right.g ) && ( left.b <= right.b )&& ( left.a <= right.a ) );
//	}
//
//	inline bool operator >						( color_pod const& left, color_pod const& right )
//	{
//		return		( ( left.r > right.r ) && ( left.g > right.g ) && ( left.b > right.b )&& ( left.a > right.a ) );
//	}
//
//	inline bool operator >=						( color_pod const& left, color_pod const& right )
//	{
//		return		( ( left.r >= right.r ) && ( left.g >= right.g ) && ( left.b >= right.b )&& ( left.a >= right.a ) );
//	}
//
//	inline bool	operator !=						( color_pod const& left, color_pod const& right )
//	{
//		return		!(left == right);
//	}
//
//	inline bool	operator ==						( color_pod const& left, color_pod const& right )
//	{
//		return		( ( left.r == right.r ) && ( left.g == right.g ) && ( left.b == right.b ) && ( left.a == right.a ) );
//	}
//
//	inline color normalize		( color_pod const& object )
//	{
//		color_pod::type const length = object.length( );
//		ASSERT		( !is_zero( length ) );
//		return		( color( object.r/length, object.g/length, object.b/length, object.a/length ) );
//	}
//
//	inline color normalize_safe( color_pod const& object, color_pod const& result_in_case_of_zero )
//	{
//		color_pod::type const length = object.length( );
//		if ( !is_zero( length ) )
//			return	( color( object.r/length, object.g/length, object.b/length, object.a/length ) );
//
//		return		( result_in_case_of_zero );
//	}
//
//
//
//	inline color::color								( ) 
//	{
//#ifdef DEBUG
//		r			= SNaN;
//		g			= SNaN;
//		b			= SNaN;
//		a			= SNaN;
//#endif // #ifdef DEBUG
//	}
//
//	inline color::color								( type const other_r, type const other_g, type const other_b, type const other_a )
//	{
//		r			= other_r;
//		g			= other_g;
//		b			= other_b;
//		a			= other_a;
//	}
//
//	inline color::color								( float3 const& rgb, type const other_a )
//	{
//		r			= rgb.x;
//		g			= rgb.y;
//		b			= rgb.z;
//		a			= other_a;
//	}
//
//	inline color::color								( color_pod const& other )
//	{
//		r			= other.r;
//		g			= other.g;
//		b			= other.b;
//		a			= other.a;
//	}
//
//	inline color::color		( u32 r, u32 g, u32 b, u32 a )
//	{
//		type t = 1.0f / 255.0f;
//		set(r*t, g*t, b*t, a*t);
//	}
//
//	inline color::color		( u32 argb )
//	{
//		type t = 1.0f / 255.0f;
//		a = t * type((argb >> 24)& 0xff);
//		r = t * type((argb >> 16)& 0xff);
//		g = t * type((argb >>  8)& 0xff);
//		b = t * type((argb >>  0)& 0xff);
//
//		set(r, g, b, a);
//	}
//
//	inline color::color			( float4 const& other )
//	{
//		r			= other.x;
//		g			= other.y;
//		b			= other.z;
//		a			= other.w;
//	}
//
//	inline u32 color::get_d3dcolor() const
//	{
//		return color_rgba( u8(math::floor(r*255) & 0xff), u8(math::floor(g*255) & 0xff), u8(math::floor(b*255) & 0xff), u8(math::floor(a*255) & 0xff) );
//	}
} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_COLOR_INLINE_H_INCLUDED