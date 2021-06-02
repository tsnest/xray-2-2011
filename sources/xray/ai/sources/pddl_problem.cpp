////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pddl_problem.h"
#include "pddl_predicate.h"
#include "object_instance.h"

namespace xray {
namespace ai {
namespace planning {

action_instance const* pddl_problem::get_action_instance	( u32 const action_type ) const
{
	for ( u32 i = 0; i < m_action_instances.size(); ++i )
		if ( m_action_instances[i]->get_type() == action_type )
			return						m_action_instances[i];

	return								0;
}

action_instance* pddl_problem::pop_action_instance	( )
{
	if ( m_action_instances.empty() )
		return								0;

	actions_type::iterator it_begin			= m_action_instances.begin();
	action_instance* result					= *it_begin;
	m_action_instances.erase				( it_begin );
	return									result;
}

} // namespace planning
} // namespace ai
} // namespace xray