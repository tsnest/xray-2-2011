////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_constructor {

#define TEMPLATE_SPECIALIZATION	template < typename vertex_type >
#define constructor				path_constructor::base::impl< vertex_type >

TEMPLATE_SPECIALIZATION
inline void constructor::on_before_search	( )
{
}

TEMPLATE_SPECIALIZATION
inline void constructor::on_after_search	( bool const success ) const
{
	XRAY_UNREFERENCED_PARAMETER				( success );
}

TEMPLATE_SPECIALIZATION
inline void constructor::assign_parent		( vertex_type& neighbour, vertex_type* parent )
{
	neighbour.back()						= parent;
}

TEMPLATE_SPECIALIZATION
template < typename T >
inline void constructor::assign_parent		( vertex_type& neighbour, vertex_type* parent, T const& )
{
	assign_parent							( neighbour, parent );
}

TEMPLATE_SPECIALIZATION
inline void constructor::update_successors	( vertex_type& neighbour )
{
	XRAY_UNREFERENCED_PARAMETER				( neighbour );
	NODEFAULT								( );
}

TEMPLATE_SPECIALIZATION
inline u32 constructor::length				( vertex_type const* vertex )
{
	R_ASSERT								( vertex, "null vertex passed" );
	u32 result								= 1;
	for ( vertex_type const* j = vertex->back(); j; j = j->back() )
		++result;

	R_ASSERT								( result >= 1, "invalid path" );
	return									result;
}

#undef TEMPLATE_SPECIALIZATION
#undef constructor

} // namespace path_constructor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_INLINE_H_INCLUDED