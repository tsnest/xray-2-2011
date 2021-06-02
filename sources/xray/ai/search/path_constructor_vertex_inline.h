////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_constructor {

#define TEMPLATE_SPECIALIZATION	template < typename VertexType, typename PathType >
#define constructor				path_constructor::vertex::impl< VertexType, PathType >

TEMPLATE_SPECIALIZATION
inline constructor::impl					( PathType* path ) :
	m_path					( path )
{
}

TEMPLATE_SPECIALIZATION
inline void constructor::on_before_search	( )
{
	if ( m_path )
		m_path->clear		( );
}

TEMPLATE_SPECIALIZATION
inline void constructor::construct_path		( VertexType const& best )
{
	if ( !m_path )
		return;

	m_path->resize			( length(&best) );

	typename PathType::reverse_iterator	i = m_path->rbegin();
	typename PathType::reverse_iterator	e = m_path->rend();
	for (VertexType const* current_vertex = &best; i != e; ++i, current_vertex = current_vertex->back() )
		*i					= current_vertex->id();
}

TEMPLATE_SPECIALIZATION
inline PathType& constructor::path			( )
{
	R_ASSERT_CMP			( m_path, !=, 0 );
	return					*m_path;
}

#undef TEMPLATE_SPECIALIZATION
#undef constructor

} // namespace path_constructor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_INLINE_H_INCLUDED