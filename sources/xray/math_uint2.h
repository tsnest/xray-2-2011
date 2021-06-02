////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_UINT2_H_INCLUDED
#define XRAY_MATH_UINT2_H_INCLUDED

namespace xray {
namespace math {

class uint2;

class uint2_pod {
public:
	typedef unsigned int	type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			type		x;
			type		y;
		};
		struct {
			type		width;
			type		height;
		};
		type			elements[2];
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline	uint2&		operator +=		( uint2_pod const& other );
	inline	uint2&		operator +=		( type value );

	inline	uint2&		operator -=		( uint2_pod const& other );
	inline	uint2&		operator -=		( type value );

	inline	uint2&		operator *=		( type value );
	inline	uint2&		operator /=		( type value );

	inline	type&		operator [ ]	( int index );

	inline	uint2&		set				( type vx, type vy);

}; // class uint2_pod

class uint2 : public uint2_pod
{
public:
	typedef uint2_pod		type_pod;

public:
	inline				uint2			( );
	inline				uint2			( type const x, type const y );
	inline				uint2			( uint2 const& other );
	inline				uint2			( uint2_pod const& other );

}; // uint2 

inline uint2			operator +		( uint2_pod const& left, uint2_pod const& right );
inline uint2			operator +		( uint2_pod const& left, uint2_pod::type const& right );
inline uint2			operator -		( uint2_pod const& left, uint2_pod const& right );
inline uint2			operator -		( uint2_pod const& left, uint2_pod::type const& right );
inline uint2			operator *		( uint2_pod const& left, uint2_pod::type const& right );
inline uint2			operator *		( uint2_pod::type const& left, uint2_pod const& right );
inline uint2			operator /		( uint2_pod const& left, uint2_pod::type const& right );
inline bool				operator ==		( uint2_pod const& left, uint2_pod const& right );
inline bool				operator !=		( uint2_pod const& left, uint2_pod const& right );


} // namespace math
} // namespace xray

#include <xray/math_uint2_inline.h>

#endif // #ifndef XRAY_MATH_UINT2_H_INCLUDED