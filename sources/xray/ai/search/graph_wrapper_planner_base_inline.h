////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace graph_wrapper {

#define WRAPPER			graph_wrapper::propositional_planner_base::impl

inline WRAPPER::impl	( graph_type* graph ) :
	m_graph				( graph )
{
}

inline void WRAPPER::on_before_search	( )
{
	m_begin				= graph().operators().objects().begin();
	m_end				= graph().operators().objects().end();

#ifdef DEBUG
	m_current_state		= 0;
#endif // #ifdef DEBUG
}

inline void WRAPPER::on_after_search	( bool const success ) const
{
	XRAY_UNREFERENCED_PARAMETER			( success );
}

template < typename VertexType >
inline void WRAPPER::edge_iterators		( VertexType const& vertex, const_edge_iterator& begin, const_edge_iterator& end )
{
	begin				= m_begin;
	end					= m_end;
	m_current_state		= &vertex.id();
}

inline WRAPPER::edge_id_type WRAPPER::edge_id	( vertex_id_type const& id, const_edge_iterator const& iterator ) const
{
	XRAY_UNREFERENCED_PARAMETER	( id );
	return				(*iterator).id();
}

inline bool  WRAPPER::applied					( ) const
{
	return				m_applied;
}

inline void WRAPPER::graph						( graph_type& graph )
{
	m_graph				= &graph;
}

inline WRAPPER::graph_type& WRAPPER::graph		( ) const
{
	ASSERT				( m_graph );
	return				*m_graph;
}

#undef WRAPPER

} // namespace graph_wrapper
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BASE_INLINE_H_INCLUDED