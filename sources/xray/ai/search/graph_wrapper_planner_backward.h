////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_H_INCLUDED
#define XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_H_INCLUDED

#include <xray/ai/search/graph_wrapper_planner_base.h>

namespace xray {
namespace ai {
namespace graph_wrapper {

struct planner_backward : private propositional_planner_base
{
	typedef propositional_planner_base	super;

	class impl : public super::impl
	{
		typedef super::impl		super;
	
	public:
		inline							impl		( graph_type* graph );
		inline vertex_id_type const&	vertex_id	( vertex_id_type const& current_vertex_id, const_edge_iterator const& iterator );

	private:
		typedef vertex_id_type::properties_type		properties_type;
		typedef properties_type::const_iterator		const_iterator;

	private:
		bool	applicable	( properties_type const& search_state, properties_type const& preconditions, properties_type const& effects );
		bool	apply		( properties_type const& search_state, properties_type const& preconditions, properties_type const& effects );
	};
};

} // namespace graph_wrapper
} // namespace ai
} // namespace xray

#include <xray/ai/search/graph_wrapper_planner_backward_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_GRAPH_WRAPPER_PLANNER_BACKWARD_H_INCLUDED