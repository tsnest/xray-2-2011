////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/plan_tracker.h>
//#include <xray/ai/search/oracle.h>
//#include <xray/ai/search/search.h>
#include "action_instance.h"
#include "pddl_planner.h"

namespace xray {
namespace ai {
namespace planning {

static bool are_parameters_equal			( property_values_type const& first, property_values_type const& second )
{
	if ( first.size() != second.size() )
		return								false;
	
	for ( u32 i = 0; i < first.size(); ++i )
		if ( first[i] != second[i] )
			return							false;
	
	return									true;
}

void plan_tracker::start_new_operator		( plan_type const& new_plan )
{
	m_current_operator						= new_plan.front();
	m_current_operator.action->initialize	( m_current_operator.parameters );
}

void plan_tracker::on_plan_changed			( plan_type const& new_plan )
{
	//dump_plan_changed						( search_service );
	//plan_type const& plan					= search_service.plan();

	if ( m_first_time )
	{
// 		if ( m_planner.failed() )
// 			return;

		m_first_time						= false;
		start_new_operator					( new_plan );
		return;
	}

	plan_item const& first_action			= new_plan.front(); 

	if ( first_action.action == m_current_operator.action && are_parameters_equal( first_action.parameters, m_current_operator.parameters ) )
		 return;

	//finalize								( );
	//m_current_operator.action->finalize		( m_current_operator.parameters );
	
// 	if ( plan.front() == m_current_operator_id )
// 		return;

// 	operator_ptr_type current_operator = m_planner.operators().object( m_current_operator_id );
// 	if ( current_operator )
// 		current_operator->finalize	( );

	start_new_operator						( new_plan );
}

void plan_tracker::track					( )
{
	if ( m_first_time )
	{
		R_ASSERT							( m_planner.failed(), "internal check failed" );
		return;
	}

	m_current_operator.action->execute		( m_current_operator.parameters );
}

void plan_tracker::finalize			( )	const
{
	if ( !m_first_time )
		m_current_operator.action->finalize	( m_current_operator.parameters );
// 	operator_ptr_type current_operator = m_planner.operators().object( m_current_operator_id );
// 	R_ASSERT						( current_operator );
// 	current_operator->finalize		( );
// 	m_planner.make_inactual			( );
// 
// 	m_first_time					= true;
}

/*
inline plan_tracker::operator_type& plan_tracker::operator_object( operator_id_type const& id ) const
{
	operator_type* object			= m_planner.operators().object( id );
	R_ASSERT						( object, "can't find operator with id %d", id );
	return							*object;
}

inline pcstr plan_tracker::operator_descriptive_id	( operator_id_type const& id ) const
{
	return							operator_object( id ).id();
}

inline plan_tracker::oracle_type& plan_tracker::oracle	( property_id_type const& id ) const
{
	oracle_type* object				= m_planner.oracles().object( id );
	R_ASSERT						( object, "can't find oracle with id %d", id );
	return							*object;
}

inline pcstr plan_tracker::oracle_descriptive_id		( property_id_type const& id ) const
{
	return							oracle( id ).id();
}

void plan_tracker::dump_oracles		( pcstr offset ) const
{
	LOG_INFO						( "\n%sORACLES : %d\n", offset, m_planner.oracles().objects().size() );

	typedef oracle_holder::objects_type	oracles_type;
	
	oracles_type::const_iterator iter		= m_planner.oracles().objects().begin();
	oracles_type::const_iterator iter_end	= m_planner.oracles().objects().end();
	
	for ( ; iter != iter_end; ++iter )
		LOG_INFO					( "%s  oracle [%d][%s]", offset, (*iter).first, oracle_descriptive_id( (*iter).first ) );
}

inline void plan_tracker::dump_state			( world_state const& state, pcstr offset ) const
{
	typedef world_state							state_type;
	typedef state_type::properties_type			properties_type;
	
	properties_type const& properties			= state.properties();
	properties_type::const_iterator iter		= properties.begin();
	properties_type::const_iterator iter_end	= properties.end();
	for ( ; iter != iter_end; ++iter )
		LOG_INFO								( "%s     %c : [%d][%s]", offset, (*iter).value() ? '+' : '-', (*iter).id(), oracle_descriptive_id( (*iter).id() ) );
}

void plan_tracker::dump_operators	( pcstr offset ) const
{
	LOG_INFO						( "\n%sOPERATORS : %d\n", offset, m_planner.operators().objects().size() );

	typedef operator_holder::objects_type	operators_type;
	operators_type::const_iterator iter		= m_planner.operators().objects().begin();
	operators_type::const_iterator iter_end	= m_planner.operators().objects().end();
	for ( ; iter != iter_end; ++iter )
	{
		R_ASSERT							( (*iter).get_operator(), "operator holder is corrupted" );
		operator_type& object				= *(*iter).get_operator();
		LOG_INFO							( "%s  operator  [%d][%s]", offset, (*iter).id(), object.id() );
		
		LOG_INFO							( "%s    preconditions :", offset );
		dump_state							( object.preconditions(), offset );
		
		LOG_INFO							( "%s    effects :", offset );
		dump_state							( object.effects(), offset );

		object.dump							( offset );

		LOG_INFO							( " " );
	}
}

void plan_tracker::dump	( pcstr offset ) const
{
	fixed_string2048	temp;
	strings::join		( temp.c_str(), sizeof( fixed_string2048 ), offset, "    " );
	dump_oracles		( temp.c_str() );
	dump_operators		( temp.c_str() );
}

inline void plan_tracker::dump_current_state	( ) const
{
	LOG_INFO			( "current world state :" );
	dump_state			( m_planner.current(), "" );
}

inline void plan_tracker::dump_target_state		( ) const
{
	LOG_INFO			( "target world state :" );
	dump_state			( m_planner.target(), "" );
}

void plan_tracker::dump_fail	( ) const
{
	dump				( "" );
	LOG_INFO			( "there is no operator sequence, which can transfer current world state to the target one" );
	LOG_INFO			( "propositional_planner : %s", m_planner.id() );
	dump_current_state	( );
	dump_target_state	( );
}

void plan_tracker::dump_search	( search& search_service ) const
{
	LOG_INFO			( "plan for object %s [%d vertices searched]", m_planner.id(), search_service.visited_vertex_count() );
}

void plan_tracker::dump_plan			( plan_type const& plan ) const
{
	LOG_INFO							( "solution :" );
	plan_type::const_iterator iter		= plan.begin();
	plan_type::const_iterator iter_end	= plan.end();
	for ( ; iter != iter_end; ++iter )
		LOG_INFO						( "     %s", operator_descriptive_id( *iter ) );
}

void plan_tracker::dump_plan_changed	( search& search_service ) const
{
	if ( m_planner.failed() )
	{
		dump_fail						( );
		return;
	}

	if ( !m_verbose )
		return;

	dump_search							( search_service );
	dump_current_state					( );
	dump_target_state					( );
	dump_plan							( search_service.plan() );
}
*/

} // namespace planning
} // namespace ai
} // namespace xray