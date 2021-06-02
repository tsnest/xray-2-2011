////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_H_INCLUDED

#include <xray/ai/search/search_backward.h>
#include <xray/ai/search/search_forward.h>

namespace xray {
namespace ai {
namespace planning {

void decision_cleanup();

class search : private boost::noncopyable
{
public:
	typedef propositional_planner										propositional_planner;
	typedef search_base::vertex_type									vertex_type;
	typedef search_base::path_constructor_base_type						path_constructor_base_type;
	typedef search_base::path_type										plan_type;

public:
						search					( );
						~search					( );
	
	inline bool			search_backward			( propositional_planner& graph );
	inline bool			search_forward			( propositional_planner& graph );
	inline plan_type const&	plan				( ) const;
	inline u32			visited_vertex_count	( ) const;

private:
	friend	void		decision_cleanup		( );

private:
	template < typename search_type >
	inline bool			search_impl				( propositional_planner& graph, search_type& search );

private:
	plan_type									m_plan;
	search_base									m_search;
	planning::search_backward					m_search_backward;
	planning::search_forward					m_search_forward;
	search*										m_previous_search;
	search*										m_next_search;
}; // class search

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/search_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_H_INCLUDED