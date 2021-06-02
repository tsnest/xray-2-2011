////////////////////////////////////////////////////////////////////////////
//	Created		: 06.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_DATA_H_INCLUDED
#define GAME_RESMAN_DATA_H_INCLUDED

#include <xray/resources_memory.h>
#include "game_resman_satisfaction_tree.h"

namespace xray {
namespace resources {

struct game_resources_manager_data
{	
	game_resources_manager_data() : current_increase_quality_tick(1) { increase_quality_timer.start(); }

	enum		flags_enum					{	flag_schedule_release_all_resources	=	1 << 0,		};

	flags_type< flags_enum, threading::simple_lock >	flags;
	memory_type_list						memory_types;
	target_satisfaction_tree_type			increase_quality_tree;
	u64										current_increase_quality_tick;
	timing::timer							increase_quality_timer;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_DATA_H_INCLUDED