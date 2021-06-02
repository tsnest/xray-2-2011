////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
//#include "trajectory.h"
#include <xray/console_command.h>

namespace xray {
namespace rtp {

u32 g_max_trajectory_length						= 5;//15;
static console_commands::cc_u32 s_cmd_max_trajectory_length( "rtp_learn_max_trajectory_length", g_max_trajectory_length, 1, 1000, true, console_commands::command_type_engine_internal );

u32 g_dbg_trajectory_render_diapason_size	= 20;


u32 g_dbg_trajectory_render_diapason_id		= 0;


} // namespace rtp
} // namespace xray