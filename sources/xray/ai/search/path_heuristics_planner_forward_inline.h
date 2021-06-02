////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_FORWARD_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_FORWARD_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_heuristics {

#define HEURISTICS		path_heuristics::planner_forward::impl

inline HEURISTICS::impl	( graph_type& graph ) :
	super				( graph )
{
}

#undef HEURISTICS

} // namespace path_heuristics
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_HEURISTICS_PLANNER_FORWARD_INLINE_H_INCLUDED