////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_constructor {

#define TEMPLATE_SPECIALIZATION	template < typename VertexType, typename PathType >
#define constructor				path_constructor::edge::impl< VertexType, PathType >

TEMPLATE_SPECIALIZATION
inline constructor::impl		( path_type* path ) :
	m_path						( path )
{
}

TEMPLATE_SPECIALIZATION
inline void constructor::assign_parent	( VertexType& neighbour, VertexType* parent, edge_id_type const& edge_id )
{
	super::assign_parent		( neighbour, parent, edge_id );
	neighbour.edge_id()			= edge_id;
}

TEMPLATE_SPECIALIZATION
inline void constructor::construct_path	( VertexType& best )
{
	if ( !m_path )
		return;

	m_path->resize				( length( &best ) - 1 );

	VertexType* i				= &best;
	typedef typename path_type::reverse_iterator	reverse_iterator_type;
	reverse_iterator_type iter		= m_path->rbegin();
	reverse_iterator_type iter_end	= m_path->rend();
	for ( ; iter != iter_end; ++iter )
	{
		*iter					= i->edge_id();
		i						= i->back();
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef constructor

} // namespace path_constructor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_INLINE_H_INCLUDED