////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "listener.h"

namespace xray {
namespace sound {

command_result_enum listener::on_sound_event		( sound_producer const& sound_source )
{
	return command_result_executed;
}

} // namespace sound
} // namespace xray