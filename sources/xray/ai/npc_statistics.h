////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_NPC_STATISTICS_H_INCLUDED
#define XRAY_AI_NPC_STATISTICS_H_INCLUDED

namespace xray {
namespace ai {

template < int ItemSize, int ItemsCount >
struct statistics_item
{
	typedef fixed_string< ItemSize >			content_type;
	
	fixed_string< 32 >							caption;
	fixed_vector< content_type, ItemsCount >	content;
};

struct npc_statistics
{
	typedef	statistics_item< 46, 16 >			sensor_info_type;
	typedef fixed_vector< sensor_info_type, 5 >	input_info_type;
	typedef statistics_item< 32, 128 >			memory_info_type;
	typedef statistics_item< 64, 8 >			blackboard_info_type;
	typedef statistics_item< 64, 1 >			goal_info_type;
	typedef statistics_item< 32, 16 >			plan_info_type;
	typedef statistics_item< 64, 16 >			general_info_type;

	input_info_type			sensors_state;
	input_info_type			selectors_state;
	memory_info_type		working_memory_state;
	blackboard_info_type	blackboard_state;
	goal_info_type			goal_selector_state;
	plan_info_type			plan_tracker_state;
	general_info_type		general_state;
}; // struct npc_statistics

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_NPC_STATISTICS_H_INCLUDED