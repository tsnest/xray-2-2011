////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_BACKWARD_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_BACKWARD_H_INCLUDED

#include <xray/ai/search/search_base.h>
#include <xray/ai/search/graph_wrapper_planner_backward.h>
#include <xray/ai/search/path_constructor_planner_backward.h>
#include <xray/ai/search/path_heuristics_planner_backward.h>
#include <xray/ai/search/search_restrictor_planner_backward.h>

namespace xray {
namespace ai {
namespace planning {

class search_backward : private boost::noncopyable
{
private:
	typedef search_base::vertex_type							vertex_type;
	typedef search_base::distance_type							distance_type;
	typedef search_base::algorithm_type							algorithm_type;
	typedef graph_wrapper::planner_backward						graph_wrapper_helper_type;
	typedef graph_wrapper_helper_type::impl						graph_wrapper_type;
	typedef path_constructor::planner_backward					path_constructor_helper_type;
	typedef search_base::path_type								path_type;
	typedef path_constructor_helper_type::impl<
				vertex_type,
				path_type
			>													path_constructor_type;
	typedef path_heuristics::planner_backward::impl				path_heuristics_type;
	typedef search_restrictor::planner_backward::impl			search_restrictor_type;

public:
	inline				search_backward							( search_base& search );
	inline bool			search									( propositional_planner& graph, path_type& path );

private:
	search_base&		m_search;
	graph_wrapper_type	m_graph_wrapper;
};

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_backward_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_BACKWARD_H_INCLUDED