////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/propositional_planner_base.h>

namespace xray {
namespace ai {
namespace planning {

#ifdef DEBUG
void propositional_planner_base::validate	( state_type const& state ) const
{
	properties_type::const_iterator	iter		= state.properties().begin();
	properties_type::const_iterator	iter_end	= state.properties().end();
	for ( ; iter != iter_end; ++iter )
		R_ASSERT					(
			oracles().objects().find( (*iter).id()) != oracles().objects().end(),
			"cannot find corresponding oracle to the property with id %d",
			(*iter).id()
		);
}
#endif // #ifdef DEBUG

} // namespace planning
} // namespace ai
} // namespace xray

