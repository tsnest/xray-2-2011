////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/plan_tracker.h>
#include <xray/ai/search/oracle.h>
#include <xray/ai/search/search.h>

namespace xray {
namespace ai {
namespace planning {

propositional_planner::propositional_planner( ) :
	propositional_planner_base	( return_this() )
{
	m_failed					= false;
	m_forward_search			= false;
}

propositional_planner::~propositional_planner( )
{
}

void propositional_planner::evaluate	( properties_const_iterator& iter, properties_const_iterator& iter_end, property_id_type const& property_id )
{
	oracles_type::const_iterator iter_oracles = oracles().objects().find( property_id );

/*
#ifdef DEBUG
	if ( iter_oracles == oracles().objects().end() )
	{
		tracker().dump			( "" );
	}
#endif //#ifdef DEBUG
*/

	R_ASSERT					( iter_oracles != oracles().objects().end(), "can't find the corresponding oracle id for the property to evaluate" );
	size_t index				= iter - m_current_state.properties().begin();
	m_current_state.add			( iter, world_state_property( property_id, (*iter_oracles).second->value() ) );
	iter						= m_current_state.properties().begin() + index;
	iter_end					= m_current_state.properties().end();
}

bool propositional_planner::actual		( )
{
	if ( !m_actual )
		return					false;

	properties_type::const_iterator iter		= current().properties().begin();
	properties_type::const_iterator iter_end	= current().properties().end();
	
	for ( ; iter != iter_end; ++iter )
	{
		oracles_type::const_iterator iter_oracles = oracles().objects().find( (*iter).id() );
		R_ASSERT				( oracles().objects().end() != iter_oracles, "can't find the corresponding oracle id for the property to evaluate" );
		
		if ( (*iter_oracles).second->value() == (*iter).value() )
			continue;
	
//		m_current_state.erase	(I,E);
		return					false;
	}

	return						true;
}

bool propositional_planner::update			( planning::search& search_service, pcstr id )
{
 	if ( actual() )
 	{
// 		tracker().track			( );
 		return true;
 	}

	m_actual					= true;
	m_current_id				= id;
	m_current_state.clear		( );

	if ( forward_search() )
		m_failed				= !search_service.search_forward( *this );
	else
		m_failed				= !search_service.search_backward( *this );

// 	if ( m_failed )
// 		return;

	return						false;
// 	on_plan_changed				( search_service );
// 	tracker().track				( );
}

} // namespace planning
} // namespace ai
} // namespace xray

