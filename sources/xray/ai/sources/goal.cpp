////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "goal.h"
#include "action_instance.h"
#include "pddl_predicate.h"
#include "action_parameter.h"

namespace xray {
namespace ai {
namespace planning {

goal::goal					( goal_types_enum goal_type, u32 priority, pcstr caption ) :
	m_goal_type				( goal_type ),
	m_caption				( caption ),
	m_next					( 0 ),
	m_priority				( priority ),
	m_can_be_executed		( true )
{
}

void goal::add_parameter	( action_parameter* parameter )
{
	m_parameters.push_back	( parameter );
}

bool goal::has_owner_parameter	( ) const
{
	for ( u32 i = 0; i < m_parameters.size(); ++i )
		if ( m_parameters[i]->get_type() == parameter_type_owner )
			return			true;

	return					false;
}

void goal::add_filters_list	( parameters_filters_type* parameters_filters )
{
	u32 const required_count = has_owner_parameter() ? m_parameters.size() - 1 : m_parameters.size();
	R_ASSERT				( parameters_filters->size() == required_count );
	
	filter_set_type* item	= NEW( filter_set_type )( parameters_filters );
	m_filters_set.push_back	( item );
}

parameters_filters_type* goal::pop_filter_list( )
{
	if ( m_filters_set.empty() )
		return				0;

	filter_set_type* filter_set = m_filters_set.pop_front();
	parameters_filters_type* result = filter_set->list;
	DELETE					( filter_set );
	return					result;
}

action_parameter* goal::get_parameter	( u32 const index ) const
{
	return					m_parameters[index];
}

void goal::add_target_property	( pddl_predicate const* required_predicate, bool value, indices_type const& object_ids )
{
	R_ASSERT					( required_predicate->get_parameters_count() == object_ids.size(), "invalid objects count" );
	pddl_world_state_property_impl	property_to_be_added( required_predicate, value );
	for ( u32 i = 0; i < object_ids.size(); ++i )
		property_to_be_added.add_index( object_ids[i] );
	
	m_target_state.push_back	( property_to_be_added );	
}

action_parameter* goal::pop_parameter	( )
{
	if ( m_parameters.empty() )
		return							0;

	parameters_type::iterator it_begin	= m_parameters.begin();
	action_parameter* result			= *it_begin;
	m_parameters.erase					( it_begin );
	return								result;
}

pddl_world_state_property_impl const& goal::get_target_state_property	( u32 const index ) const
{
	return								m_target_state[index];
}

struct parameters_suitability_checker_predicate : private boost::noncopyable
{
	inline parameters_suitability_checker_predicate	( property_values_type const& objects ) :
		instances									( objects )
	{
	}

	bool operator ()					( goal::filter_set_type* current_filters_set ) const
	{
		R_ASSERT						( current_filters_set->list->size() == instances.size() );
		u32 index						= 0;
		for ( parameter_filters_item_type* it_filter = current_filters_set->list->front(); it_filter; it_filter = current_filters_set->list->get_next_of_object( it_filter ), ++index )
		{
			if ( !action_instance::is_object_suitable( *( it_filter->list ), instances[index] ) )
				return					false;
		}
		return							true;
	}

	property_values_type const&			instances;
};

bool goal::are_parameters_suitable			( property_values_type const& objects )  const
{
	parameters_suitability_checker_predicate const suitability_checker( objects );
	return m_filters_set.find_if			( suitability_checker ) != 0;
}

} // namespace planning
} // namespace ai
} // namespace xray