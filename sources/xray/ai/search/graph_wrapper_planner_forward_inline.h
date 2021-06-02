////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_FORWARD_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_FORWARD_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace graph_wrapper {

#define WRAPPER			graph_wrapper::planner_forward::impl

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
	properties_type const& start_state		= graph().current().properties();

	if ( !applicable( search_state, preconditions, start_state ) )
	{
		m_applied							= false;
		return								m_new_state;
	}

	m_applied								= true;
	properties_type const& effects			= operator_impl.effects().properties();
	apply									( search_state, effects, start_state );
	return									m_new_state;
}

#undef WRAPPER

} // namespace graph_wrapper
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_FORWARD_INLINE_H_INCLUDED