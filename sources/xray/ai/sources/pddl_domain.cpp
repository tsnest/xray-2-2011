////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pddl_domain.h"
#include "pddl_predicate.h"
#include "pddl_problem.h"

namespace xray {
namespace ai {
namespace planning {

pddl_domain::~pddl_domain				( )
{
	while ( !m_predicates.empty() )
	{
		predicates_type::iterator iter	= m_predicates.begin();
		DELETE							( iter->second );
		m_predicates.erase				( iter );
	}
}

pddl_predicate const* pddl_domain::operator []	( pddl_predicate_id const type ) const
{
	predicates_type::const_iterator iter = m_predicates.find( type );
	if ( iter != m_predicates.end() )
		return							iter->second;

	return								0;
}

pddl_predicate const* pddl_domain::get_predicate	( u32 const index ) const
{
	R_ASSERT_CMP									( index, <, m_predicates.size(), "improper index" );
	predicates_type::const_iterator result			= m_predicates.begin();
	std::advance									( result, index );
	return											result->second;
}

void pddl_domain::add_action	( generalized_action* action_to_be_added )
{
	m_actions.push_back			( action_to_be_added );
}

generalized_action* pddl_domain::pop_action	( )
{
	return						m_actions.pop_front();
}

generalized_action const* pddl_domain::get_action_by_type( u32 const type ) const
{
	for ( generalized_action* it_action = m_actions.front(); it_action; it_action = m_actions.get_next_of_object( it_action ) )
		if ( it_action->get_type() == type )
			return							it_action;
	
	return									0;
}

parameter_type pddl_domain::get_registered_type	( pcstr registered_typename ) const
{
	registered_parameter_types::const_iterator iter = m_registered_types.find( registered_typename );
	return iter != m_registered_types.end() ? iter->second : parameter_type(-1);
}

} // namespace planning
} // namespace ai
} // namespace xray
