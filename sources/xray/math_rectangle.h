////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_RECTANGLE_H_INCLUDED
#define XRAY_MATH_RECTANGLE_H_INCLUDED

namespace xray {
namespace math {

template <typename vector_type>
class rectangle;

template < typename vector_type >
class rectangle_pod {
public:
	typedef typename vector_type::type_pod	vector_type_pod;
	typedef typename vector_type::type		type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			vector_type_pod	min;
			vector_type_pod	max;
		};
		struct {
			type		left;
			type		top;
			type		right;
			type		bottom;
		};
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:

	inline	rectangle<vector_type>&	zero		( );
	inline	rectangle<vector_type>&	identity	( );
	inline	rectangle<vector_type>&	invalidate	( );

	inline	rectangle<vector_type>&	operator +=	( type value );
	inline	rectangle<vector_type>&	operator +=	( vector_type const& value );
	inline	rectangle<vector_type>&	operator -=	( type value );
	inline	rectangle<vector_type>&	operator -=	( vector_type const& value );
	inline	rectangle<vector_type>&	operator *=	( type value );
	inline	rectangle<vector_type>&	operator *=	( vector_type const& value );
	inline	rectangle<vector_type>&	operator /=	( type value );
	inline	rectangle<vector_type>&	operator /=	( vector_type const& value );

	inline	rectangle<vector_type>&	move		( vector_type const& value );

	inline	rectangle<vector_type>&	modify		( vector_type const& position );
	inline	rectangle<vector_type>&	modify		( rectangle_pod const& other );
			rectangle<vector_type>&	modify		( float4x4 const& transform );

	inline	vector_type	size		( ) const;
	inline	type		radius		( ) const;
	inline	type		volume		( ) const;

	inline	vector_type	center		( ) const;
	inline	vector_type	center		( vector_type& size ) const;

	inline	bool		intersect	( rectangle_pod const& other ) const;

	inline	bool		contains	( vector_type const& position ) const;
	inline	bool		contains	( rectangle_pod const& other ) const;

	inline	bool		similar		( rectangle_pod const& other ) const;

	inline	vector_type	vertex		( u32 index ) const;
	inline	void		vertices	( vector_type ( &result )[ 4 ] ) const;

	inline	type		width		( ) const;
	inline	type		height		( ) const;

	inline	bool		valid		( ) const;
}; // class rectangle

template <typename vector_type>
class rectangle : public rectangle_pod<vector_type>
{
public:
	inline				rectangle	( );
	inline				rectangle	( vector_type const& min, vector_type const& max );
};

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator +	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::type value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator +	( typename math::rectangle_pod<vector_type>::type value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator +	( math::rectangle<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::vector_type const& value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator +	( typename math::rectangle_pod<vector_type>::vector_type const& value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator -	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::type value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator -	( typename math::rectangle_pod<vector_type>::type value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator -	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::vector_type const& value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator -	( typename math::rectangle_pod<vector_type>::vector_type const& value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator *	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::type value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator *	( typename math::rectangle_pod<vector_type>::type value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator *	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::vector_type const& value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator *	( typename math::rectangle_pod<vector_type>::vector_type const& value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator /	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::type value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator /	( typename math::rectangle_pod<vector_type>::type value, math::rectangle_pod<vector_type> const& rectangle );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator /	( math::rectangle_pod<vector_type> const& rectangle, typename math::rectangle_pod<vector_type>::vector_type const& value );

template < typename vector_type >
inline 	math::rectangle<vector_type>	operator /	( typename math::rectangle_pod<vector_type>::vector_type const& value, math::rectangle_pod<vector_type> const& rectangle );

typedef rectangle<float2_pod>	float_rectangle;

} // namespace math
} // namespace xray

#include <xray/math_rectangle_inline.h>

#endif // #ifndef XRAY_MATH_RECTANGLE_H_INCLUDED