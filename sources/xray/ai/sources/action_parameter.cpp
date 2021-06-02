////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "action_parameter.h"
#include "target_selector_base.h"

namespace xray {
namespace ai {
namespace planning {

action_parameter::action_parameter	( parameter_type const type ) :
	m_type							( type ),
	m_iterate_only_first			( true )
{
}

} // namespace planning
} // namespace ai
} // namespace xray