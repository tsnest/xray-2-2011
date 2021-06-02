////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_RECTANGLE_INLINE_H_INCLUDED
#define XRAY_MATH_RECTANGLE_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template < typename vector_type >
#define RECTANGLE			rectangle< vector_type >
#define RECTANGLE_POD		rectangle_pod< vector_type >

namespace xray {
namespace math {

TEMPLATE_SIGNATURE
inline RECTANGLE::rectangle								( )
{
}

TEMPLATE_SIGNATURE
inline RECTANGLE::rectangle								( vector_type const& _min, vector_type const& _max )
{
	min		= _min;
	max		= _max;
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::zero						( )
{
	min			= max = vector_type ( type(0), type(0), type(0) );
	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::invalidate					( )
{
	min			= vector_type ( +infinity, +infinity, +infinity );
	max			= vector_type ( -infinity, -infinity, -infinity );

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator +=				( type value )
{
	min			-= value;
	max			+= value;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator +=				( vector_type const& value )
{
	min			-= value;
	max			+= value;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator -=				( type value )
{
	min			+= value;
	max			-= value;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator -=				( vector_type const& value )
{
	min			+= value;
	max			-= value;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator *=				( type value )
{
	RECTANGLE_POD::vector_type		extents, center = this->center( extents );
	extents		*= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator *=				( vector_type const& value )
{
	RECTANGLE_POD::vector_type		extents, center = this->center( extents );
	extents		*= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator /=				( type value )
{
	RECTANGLE_POD::vector_type		extents, center = this->center( extents );
	extents		/= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::operator /=				( vector_type const& value )
{
	RECTANGLE_POD::vector_type		extents, center = this->center( extents );
	extents		/= value;
	min			= center - extents;
	max			= center + extents;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::move						( vector_type const& value )
{
	min			+= value;
	max			+= value;

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::modify						( vector_type const& position )
{
	min			= math::min( min, position );
	max			= math::max( max, position );

	return		( *this );
}

TEMPLATE_SIGNATURE
inline RECTANGLE& RECTANGLE_POD::modify						( rectangle_pod const& other )
{
	modify		( other.min );
	modify		( other.max );
}

TEMPLATE_SIGNATURE
inline vector_type RECTANGLE_POD::size						( ) const
{
	return		( max - min );
}

TEMPLATE_SIGNATURE
inline typename RECTANGLE_POD::type RECTANGLE_POD::radius		( ) const
{
	return		( size( ).length( )/type(2) );
}

TEMPLATE_SIGNATURE
inline typename RECTANGLE_POD::type RECTANGLE_POD::volume		( ) const
{
	RECTANGLE_POD::vector_type const size = this->size( );
	return		( size.x*size.y*size.z );
}

TEMPLATE_SIGNATURE
inline vector_type RECTANGLE_POD::center					( ) const
{
	return		( ( min + max )/type(2) );
}

TEMPLATE_SIGNATURE
inline vector_type RECTANGLE_POD::center					( vector_type& size ) const
{
	size		= ( min - max )/type(2);
	return		( min + size );
}

TEMPLATE_SIGNATURE
inline bool	RECTANGLE_POD::intersect						( rectangle_pod const& other ) const
{
	return		( ( max >= other.min ) && ( min <= max ) );
}

TEMPLATE_SIGNATURE
inline bool	RECTANGLE_POD::contains							( vector_type const& position ) const
{
	return		( ( min <= position ) && ( max >= position ) );
}

TEMPLATE_SIGNATURE
inline bool	RECTANGLE_POD::contains							( rectangle_pod const& other ) const
{
	return		( ( min <= other.min ) && ( max >= other.max  ) );
}

TEMPLATE_SIGNATURE
inline bool	RECTANGLE_POD::similar							( rectangle_pod const& other ) const
{
	return		( min.is_similar( other.min ) && max.is_similar( other.max ) );
}

TEMPLATE_SIGNATURE
inline vector_type RECTANGLE_POD::vertex					( u32 index ) const
{
	ASSERT		( index < 4 );

	switch ( index ) {
		case 0 : return( vector_type ( min.x, min.y ) );
		case 1 : return( vector_type ( min.x, max.y ) );
		case 2 : return( vector_type ( max.x, min.y ) );
		case 3 : return( vector_type ( max.x, max.y ) );
		default: NODEFAULT(return vector_type( ));
	}
}

TEMPLATE_SIGNATURE
inline void	RECTANGLE_POD::vertices							( vector_type ( &result )[ 4 ] ) const
{
	result[ 0 ]	= vector_type ( min.x, min.y );
	result[ 1 ]	= vector_type ( min.x, max.y );
	result[ 2 ]	= vector_type ( max.x, min.y );
	result[ 3 ]	= vector_type ( max.x, max.y );
}

TEMPLATE_SIGNATURE
inline bool	RECTANGLE_POD::valid			( ) const
{
	return		( min.valid( ) && max.valid( ) );
}

TEMPLATE_SIGNATURE
inline typename RECTANGLE_POD::type RECTANGLE_POD::width		( ) const
{
	ASSERT		( max.x >= min.x );
	return		( max.x - min.x );
}

TEMPLATE_SIGNATURE
inline typename RECTANGLE_POD::type RECTANGLE_POD::height		( ) const
{
	ASSERT		( max.y >= min.y );
	return		( max.y - min.y );
}

#undef RECTANGLE
#undef TEMPLATE_SIGNATURE

#define TEMPLATE_SIGNATURE	template < typename vector_type >

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator +						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::type value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min - value;
	result.max	= rectangle.max + value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator +						( typename math::rectangle<vector_type>::type value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min - value;
	result.max	= rectangle.max + value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator +						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::vector_type const& value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min - value;
	result.max	= rectangle.max + value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator +						( typename math::rectangle<vector_type>::vector_type const& value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min - value;
	result.max	= rectangle.max + value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator -						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::type value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min + value;
	result.max	= rectangle.max - value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator -						( typename math::rectangle<vector_type>::type value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min + value;
	result.max	= rectangle.max - value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator -						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::vector_type const& value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min + value;
	result.max	= rectangle.max - value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator -						( typename math::rectangle<vector_type>::vector_type const& value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min + value;
	result.max	= rectangle.max - value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator *						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::type value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min * value;
	result.max	= rectangle.max * value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator *						( typename math::rectangle<vector_type>::type value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min * value;
	result.max	= rectangle.max * value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator *						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::vector_type const& value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min * value;
	result.max	= rectangle.max * value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator *						( typename math::rectangle<vector_type>::vector_type const& value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min * value;
	result.max	= rectangle.max * value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator /						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::type value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min / value;
	result.max	= rectangle.max / value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator /						( typename math::rectangle<vector_type>::type value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min / value;
	result.max	= rectangle.max / value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator /						( math::rectangle<vector_type> const& rectangle, typename math::rectangle<vector_type>::vector_type const& value )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min / value;
	result.max	= rectangle.max / value;
	return		( result );
}

TEMPLATE_SIGNATURE
inline math::rectangle<vector_type>	operator /						( typename math::rectangle<vector_type>::vector_type const& value, math::rectangle<vector_type> const& rectangle )
{
	math::rectangle<vector_type>	result;
	result.min	= rectangle.min / value;
	result.max	= rectangle.max / value;
	return		( result );
}

} // namespace math
} // namespace xray

#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_MATH_RECTANGLE_INLINE_H_INCLUDED