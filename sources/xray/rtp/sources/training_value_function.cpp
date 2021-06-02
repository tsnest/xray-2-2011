////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "training_value_function.h"
#include <xray/console_command.h>

namespace xray {
namespace rtp {

u32 g_num_trajectories					= 30;
static console_commands::cc_u32 s_cmd_num_trajectories( "rtp_learn_num_trajectories", g_num_trajectories, 1, 100, true, console_commands::command_type_engine_internal );
} //namespace rtp 
} //namespace xray 
