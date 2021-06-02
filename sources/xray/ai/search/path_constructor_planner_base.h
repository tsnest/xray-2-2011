////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BASE_H_INCLUDED

#include <xray/ai/search/path_constructor_edge.h>

namespace xray {
namespace ai {
namespace path_constructor {

struct propositional_planner_base : public edge
{
	typedef edge										super;
	typedef planning::operator_id_type					edge_id_type;
	typedef super::helper< edge_id_type >				helper;

	template < typename final_type >
	struct vertex_impl : public helper::vertex_impl< final_type > {};
}; // struct propositional_planner_base

} // namespace path_constructor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BASE_H_INCLUDED