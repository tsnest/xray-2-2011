////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_BACKWARD_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_BACKWARD_H_INCLUDED

#include <xray/ai/search/path_heuristrics_planner_base.h>

namespace xray {
namespace ai {
namespace path_heuristics {

struct planner_backward : public propositional_planner_base
{
	typedef propositional_planner_base		super;

	class impl : public super::impl
	{
	public:
		inline						impl		( graph_type& graph );
				edge_weight_type	estimate	( vertex_id_type const* current_vertex_id_ptr, vertex_id_type const& neighbour_vertex_id ) const;

	private:
		typedef super::impl					super;
	}; // class impl
}; // struct planner_backward

} // namespace path_heuristics
} // namespace ai
} // namespace xray

#include <xray/ai/search/path_heuristics_planner_backward_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_BACKWARD_H_INCLUDED
