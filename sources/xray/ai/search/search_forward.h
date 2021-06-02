////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_FORWARD_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_FORWARD_H_INCLUDED

#include <xray/ai/search/search_base.h>
#include <xray/ai/search/graph_wrapper_planner_forward.h>
#include <xray/ai/search/path_constructor_planner_forward.h>
#include <xray/ai/search/path_heuristics_planner_forward.h>
#include <xray/ai/search/search_restrictor_planner_forward.h>

namespace xray {
namespace ai {
namespace planning {

class search_forward : private boost::noncopyable
{
private:
	typedef search_base::vertex_type							vertex_type;
	typedef search_base::algorithm_type							algorithm_type;
	typedef search_base::distance_type							distance_type;
	typedef graph_wrapper::planner_forward						graph_wrapper_helper_type;
	typedef graph_wrapper_helper_type::impl						graph_wrapper_type;
	typedef path_constructor::planner_forward					path_constructor_helper_type;
	typedef search_base::path_type								path_type;
	typedef path_constructor_helper_type::impl<
				vertex_type,
				path_type
			>													path_constructor_type;
	typedef path_heuristics::planner_forward::impl				path_heuristics_type;
	typedef search_restrictor::planner_forward::impl			search_restrictor_type;

public:
	inline				search_forward	( search_base& search );
	inline bool			search			( propositional_planner& graph, path_type& path );

private:
	search_base&		m_search;
	graph_wrapper_type	m_graph_wrapper;
}; // class search_forward

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_forward_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_FORWARD_H_INCLUDED