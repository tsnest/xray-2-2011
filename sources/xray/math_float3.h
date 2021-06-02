////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT3_H_INCLUDED
#define XRAY_MATH_FLOAT3_H_INCLUDED

namespace xray {
namespace math {

class float3;

class float3_pod {
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
			type		z;
		};
		float			elements[3];
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline	float3		operator -		( ) const;

	inline	float3&		operator +=		( float3_pod const& other );
	inline	float3&		operator +=		( type value );

	inline	float3&		operator -=		( float3_pod const& other );
	inline	float3&		operator -=		( type value );

	inline	float3&		operator *=		( type value );
	inline	float3&		operator *=		( float3_pod const& value );

	inline	float3&		operator /=		( type value );
	inline	float3&		operator /=		( float3_pod const& value );

	inline	float3&		operator ^=		( float3_pod const& other );

	inline	type&		operator [ ]	( int index );
	inline	const type&	operator [ ]	( int index )const;
	inline	float3&		set				( type vx, type vy, type vz );

	inline	type		dot_product		( float3_pod const& other ) const;
	inline	float3&		cross_product	( float3_pod const& other );

	inline	float3&		normalize		( );
	inline	type		normalize_r		( ); // returns old length

	inline	float3&		normalize_safe	( float3_pod const& result_in_case_of_zero );
	inline	type		normalize_safe_r( float3_pod const& result_in_case_of_zero ); // returns old length

	inline	float3&		abs				( );

	inline	float3&		min				( float3_pod const& other );
	inline	float3&		max				( float3_pod const& other );

	inline	type		length			( ) const;
	inline	type		squared_length	( ) const;

	inline	void		set_length		( float length );

	inline	bool		is_similar			( float3_pod const& other, float epsilon = epsilon_5 ) const;
	inline	bool		valid			( ) const;

	static	float3		decompress		( u16 value );
			u16			compress		( ) const;
}; // struct float3_pod

class XRAY_CORE_API float3 : public float3_pod {
public:
	inline				float3			( );
	inline				float3			( type x, type y, type z );
	inline				float3			( float3_pod const& other );
};

inline float3			operator +		( float3_pod const& left, float3_pod const& right );
inline float3			operator +		( float3_pod const& left, float3_pod::type const& right );
inline float3			operator -		( float3_pod const& left, float3_pod const& right );
inline float3			operator -		( float3_pod const& left, float3_pod::type const& right );
inline float3			operator *		( float3_pod const& left, float3_pod::type const& right );
inline float3			operator *		( float3_pod::type const& left, float3_pod const& right );
inline float3			operator *		( float3_pod const& left, float3_pod const& right );
inline float3			operator /		( float3_pod const& left, float3_pod::type const& right );
inline float3			operator /		( float3_pod::type const& left, float3_pod const& right );
inline float3			operator /		( float3_pod const& left, float3_pod const& right );
inline float3::type		operator |		( float3_pod const& left, float3_pod const& right );
inline float3			operator ^		( float3_pod const& left, float3_pod const& right );
inline bool				operator <		( float3_pod const& left, float3_pod const& right );
inline bool				operator <=		( float3_pod const& left, float3_pod const& right );
inline bool				operator >		( float3_pod const& left, float3_pod const& right );
inline bool				operator >=		( float3_pod const& left, float3_pod const& right );
inline bool				operator !=		( float3_pod const& left, float3_pod const& right );
inline bool				operator ==		( float3_pod const& left, float3_pod const& right );

inline bool				less_by_order	( float3_pod const& left, float3_pod const& right );
inline bool			less_equal_by_order	( float3_pod const& left, float3_pod const& right );
inline bool				greater_by_order( float3_pod const& left, float3_pod const& right );
inline bool		greater_equal_by_order	( float3_pod const& left, float3_pod const& right );

inline float3::type		dot_product		( float3_pod const& left, float3_pod const& right );
inline float3			cross_product	( float3_pod const& left, float3_pod const& right );
inline float3			normalize		( float3_pod const& object );
inline float3			normalize_safe	( float3_pod const& object, float3_pod const& result_in_case_of_zero = float3( 0.f, 0.f, 1.f ) );

inline float3			abs				( float3_pod const& object );
inline float3			min				( float3_pod const& left, float3_pod const& right );
inline float3			max				( float3_pod const& left, float3_pod const& right );
inline float			length			( float3_pod const& object );
inline float			squared_length	( float3_pod const& object);
inline bool				is_similar		( float3_pod const& left, float3_pod const& right, float epsilon = epsilon_5 );
inline bool				valid			( float3_pod const& object );
inline float3			pow				( float3_pod const& object, float3_pod::type power );
inline float3			pow				( float3_pod const& object, float3_pod const& power );

inline float3			abs				( float3 const& object );
inline float3			min				( float3 const& left, float3 const& right );
inline float3			max				( float3 const& left, float3 const& right );
inline float			length			( float3 const& object );
inline float			squared_length	( float3 const& object);
inline bool				is_similar		( float3 const& left, float3 const& right, float epsilon = epsilon_5 );
inline bool		is_relatively_similar	( float3 const& left, float3 const& right, float epsilon = epsilon_6 );

#ifndef MASTER_GOLD
	inline bool			valid			( float3 const& object );
#endif // #ifndef MASTER_GOLD

XRAY_CORE_API void	generate_orthonormal_basis	( float3 const& direction, float3& up, float3& right );
XRAY_CORE_API float3 normalize_precise			( float3 const& object );

} // namespace math
} // namespace xray

#include <xray/math_float3_inline.h>

#endif // #ifndef XRAY_MATH_FLOAT2_H_INCLUDED