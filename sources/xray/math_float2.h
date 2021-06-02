////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT2_H_INCLUDED
#define XRAY_MATH_FLOAT2_H_INCLUDED

namespace xray {
namespace math {

class float2;

class float2_pod {
public:
	typedef float		type;

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
		float			elements[2];
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline	float2&		operator -		( );

	inline	float2&		operator +=		( float2_pod const& other );
	inline	float2&		operator +=		( type value );

	inline	float2&		operator -=		( float2_pod const& other );
	inline	float2&		operator -=		( type value );

	inline	float2&		operator *=		( type value );
	inline	float2&		operator /=		( type value );
	inline	float2&		operator ^=		( float2_pod const& other );	/// cross_product

	inline	type&		operator [ ]	( int index );
	inline	const type&	operator [ ]	( int index ) const;
	inline	float2&		set				( type vx, type vy);

	inline	type		dot_product		( float2_pod const& other ) const;
	inline	float2&		cross_product	( float2_pod const& other );

	inline	float2&		normalize		( );
	inline	type		normalize_r		( ); // returns old length

	inline	float2&		normalize_safe	( float2_pod const& result_in_case_of_zero );
	inline	type		normalize_safe_r( float2_pod const& result_in_case_of_zero ); // returns old length

	inline	float2&		abs				( );

	inline	float2&		min				( float2_pod const& other );
	inline	float2&		max				( float2_pod const& other );

	inline	type		length			( ) const;
	inline	type		squared_length	( ) const;

	inline	void		set_length		( float length );

	inline	bool		is_similar		( float2_pod const& other, float epsilon = epsilon_5 ) const;

	#ifndef MASTER_GOLD
	inline	bool		valid			( ) const;
	#endif
}; // struct float2_pod

class float2 : public float2_pod {
public:
	typedef float2_pod		type_pod;

public:
	inline				float2			( );
	inline				float2			( type x, type y );
	inline				float2			( float2 const& other );
	inline				float2			( float2_pod const& other );
}; // class float2

inline float2			operator +		( float2_pod const& left, float2_pod const& right );
inline float2			operator +		( float2_pod const& left, float2_pod::type const& right );
inline float2			operator -		( float2_pod const& left, float2_pod const& right );
inline float2			operator -		( float2_pod const& left, float2_pod::type const& right );
inline float2			operator *		( float2_pod const& left, float2_pod::type const& right );
inline float2			operator *		( float2_pod::type const& left, float2_pod const& right );
inline float2			operator /		( float2_pod const& left, float2_pod::type const& right );
inline float2			operator /		( float2_pod::type const& left, float2_pod const& right );
inline float2::type		operator |		( float2_pod const& left, float2_pod::type const& right );
inline float2			operator ^		( float2_pod const& left, float2_pod::type const& right );
inline bool				operator <		( float2_pod const& left, float2_pod const& right );
inline bool				operator <=		( float2_pod const& left, float2_pod const& right );
inline bool				operator >		( float2_pod const& left, float2_pod const& right );
inline bool				operator >=		( float2_pod const& left, float2_pod const& right );
inline bool				operator !=		( float2_pod const& left, float2_pod const& right );
inline bool				operator ==		( float2_pod const& left, float2_pod const& right );
inline float2::type		dot_product		( float2_pod const& left, float2_pod const& right );
inline float2			cross_product	( float2_pod const& object );
inline float2			normalize		( float2_pod const& object );
inline float2			normalize_safe	( float2_pod const& object, float2_pod const& result_in_case_of_zero = float2( 0.f, 1.f ) );
inline float2			pow				( float2_pod const& object, float2_pod::type power );
inline float2			pow				( float2_pod const& object, float2_pod const& power );

} // namespace math
} // namespace xray

#include <xray/math_float2_inline.h>

#endif // #ifndef XRAY_MATH_FLOAT2_H_INCLUDED