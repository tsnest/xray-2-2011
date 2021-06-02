////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace graph_wrapper {

#define WRAPPER			graph_wrapper::planner_backward::impl

inline WRAPPER::impl	( graph_type* graph ) :
	super				( graph )
{
}

inline WRAPPER::vertex_id_type const& WRAPPER::vertex_id( vertex_id_type const& current_vertex_id, const_edge_iterator const& iterator )
{
	XRAY_UNREFERENCED_PARAMETER				( current_vertex_id );

	R_ASSERT								( m_current_state, "current state is null" );
	properties_type const& search_state		= m_current_state->properties();
	edge_type& operator_impl				= *(*iterator).get_operator();
	properties_type const& preconditions	= operator_impl.preconditions().properties();
	properties_type const& effects			= operator_impl.effects().properties();

	if ( !applicable( search_state, preconditions, effects ) )
	{
		m_applied		= false;
		return			m_new_state;
	}

	m_applied			= apply( search_state, preconditions, effects );
	return				m_new_state;
}

#undef WRAPPER

} // namespace graph_wrapper
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_INLINE_H_INCLUDED