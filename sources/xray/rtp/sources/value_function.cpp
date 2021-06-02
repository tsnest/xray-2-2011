////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "value_function.h"
#include <xray/console_command.h>
namespace xray {
namespace rtp {

float g_discount	= 0.8f;//dicount factor
static console_commands::cc_float s_cmd_discount( "rtp_learn_discount", g_discount, 0.f, 10000.f, true, console_commands::command_type_engine_internal );

} // namespace rtp 
} //namespace xray 