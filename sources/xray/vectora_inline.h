////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VECTORA_INLINE_H_INCLUDED
#define XRAY_VECTORA_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template < typename T >
#define VECTORA				vectora < T >

namespace xray {

TEMPLATE_SIGNATURE
inline VECTORA::vectora									( allocator_type const allocator ) :
	super				( allocator )
{
}


TEMPLATE_SIGNATURE
inline VECTORA::vectora									( xray::memory::base_allocator* const allocator ) :
														super( allocator_type(allocator) )
{
}

TEMPLATE_SIGNATURE
inline VECTORA::vectora									( allocator_type const& allocator, size_type const count, value_type const& value ) :
	super				( count, value, allocator )
{
}

TEMPLATE_SIGNATURE
inline VECTORA::vectora									( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline VECTORA::vectora									( allocator_type const& allocator, input_iterator const& first, input_iterator const& last ) :
	super				( first, last, allocator )
{
}

TEMPLATE_SIGNATURE
inline typename VECTORA::self_type& VECTORA::operator=	( self_type const& other )
{
	return					(self_type&)super::operator=( other );
}

TEMPLATE_SIGNATURE
inline u32 VECTORA::size								( ) const
{
	return					( ( u32 ) super::size( ) );
}

} // namespace xray

#undef VECTORA
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_VECTORA_INLINE_H_INCLUDED