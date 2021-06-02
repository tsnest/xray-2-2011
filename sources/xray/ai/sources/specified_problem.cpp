////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "specified_problem.h"
#include "pddl_predicate.h"
#include "pddl_domain.h"
#include "pddl_problem.h"
#include "base_lexeme.h"
#include "brain_unit.h"
#include <xray/memory_stack_allocator.h>
#include <xray/ai/npc.h>

namespace xray {
namespace ai {
namespace planning {

specified_problem::specified_problem		( pddl_domain const& domain, pddl_problem const& problem, brain_unit* const owner ) :
	m_domain								( domain ),
	m_problem								( problem ),
	m_owner									( owner )
{
	u32 const predicates_count				= m_domain.get_predicates_count();
	for ( u32 i = 0; i < predicates_count; ++i )
	{
		pddl_predicate const* const current	= m_domain.get_predicate( i );
		m_predicates[current->get_type()]	= std::make_pair( current, u32(-1) );
	}
}

specified_problem::~specified_problem		( )
{
	restore_predicates						( );
}

u32 specified_problem::get_predicate_offset	( pddl_predicate_id const type ) const
{
	predicates_type::const_iterator iter	= m_predicates.find( type );
	return									iter != m_predicates.end() ? iter->second.second : u32(-1);
}

u32 specified_problem::get_world_state_size	( ) const
{
	u32 world_state_size				= 0;
	for ( predicates_type::const_iterator iter = m_predicates.begin(); iter != m_predicates.end(); ++iter )
	{
		pddl_predicate const& current_predicate = *( iter->second.first );
		R_ASSERT						(
			iter->second.second != u32(-1),
			"parametrized predicates with no corresponding objects in problem can't be present in world state"
		);

		u32 combinations_count			= 1;
		for ( u32 j = 0; j < current_predicate.get_parameters_count(); ++j )
			combinations_count			*= get_count_of_objects_by_type( current_predicate.get_parameter( j ) );

		world_state_size				+= combinations_count;
	}
	return								world_state_size;
}

void specified_problem::restore_predicates	( )
{
	for ( u32 i = 0; i < m_excluded_predicates.size(); ++i )
	{
		pddl_predicate const* const current_predicate = m_excluded_predicates[i];
		m_predicates[current_predicate->get_type()] = std::make_pair( current_predicate, u32(-1) );
	}
	m_excluded_predicates.clear				( );
}

void specified_problem::calculate_predicates_offsets	( )
{
	restore_predicates						( );
	u32 offset								= 0;
	for ( predicates_type::iterator iter = m_predicates.begin(); iter != m_predicates.end(); ++iter )
	{
		pddl_predicate const& current_predicate = *( iter->second.first );
		
		u32 combinations_count				= 1;
		bool exclude_from_world_state		= false;
		for ( u32 j = 0; j < current_predicate.get_parameters_count(); ++j )
		{
			u32 const objects_count			= get_count_of_objects_by_type( current_predicate.get_parameter( j ) );
			if ( objects_count == 0 )
			{
				exclude_from_world_state	= true;
				break;
			}
			else
				combinations_count			*= objects_count;
		}

		if ( !exclude_from_world_state )
		{
			iter->second.second				= offset;
			offset							+= combinations_count;
		}
		else
			iter->second.second				= u32(-1);
	}
	predicates_type::iterator iter			= m_predicates.begin();
	while ( iter != m_predicates.end() )
	{
		if ( iter->second.second == u32(-1) )
		{
			m_excluded_predicates.push_back( iter->second.first );
			m_predicates.erase				( iter++ );
		}
		else
			++iter;
	}
}

bool operator <	( specified_problem::predicates_type::value_type const& predicate1, specified_problem::predicates_type::value_type const& predicate2 )
{
	return												predicate1.second.second < predicate2.second.second;
}

pddl_predicate const* specified_problem::get_lower_bound_predicate( u32 const offset ) const
{
	pddl_predicate temp_predicate						( 0, "" );
	specified_problem::predicate_type const temp_pair	= std::make_pair( &temp_predicate, offset );
	predicates_type::value_type const temp_value		= std::make_pair( 0, temp_pair );
 	
 	predicates_type::const_iterator iter				= std::lower_bound( m_predicates.begin(), m_predicates.end(), temp_value );
  
 	if ( iter->second.second == offset )
 		return											iter->second.first;
 
 	if ( iter == m_predicates.begin() )
 		return											0;

	return												(--iter)->second.first;
}

void specified_problem::reset	( )
{
	m_target_world_state.clear	( );
	m_objects.clear				( );
}

void specified_problem::add_object_instance	( object_type instance, pcstr caption )
{
	parameter_type const type				= m_domain.get_registered_type( instance.second );
	R_ASSERT								( type != parameter_type(-1), "invalid object type" );
	
	if ( get_object_index( type, instance.first ) == u32(-1) )
		m_objects[type].push_back			( object_instance( type, instance.first, caption ) );
}

object_instance const* specified_problem::get_object_by_type_and_index	( parameter_type const type, u32 const index ) const
{
	objects_type::const_iterator iter		= m_objects.find( type );
	
	if ( iter != m_objects.end() )
	{
		R_ASSERT							( index < iter->second.size(), "index is out of bounds" );
		return								&(iter->second[index]);
	}
	
	return									0;
}

void specified_problem::add_target_property	( pddl_world_state_property_impl const& property_to_be_added )
{
	m_target_world_state.push_back			( property_to_be_added );
}

u32 specified_problem::get_count_of_objects_by_type	( parameter_type const type ) const
{
	objects_type::const_iterator iter		= m_objects.find( type );
	return									iter == m_objects.end() ? 0 : iter->second.size();
}

u32 specified_problem::get_object_index		( parameter_type const type, object_instance_type const& instance ) const
{
	objects_type::const_iterator iter		= m_objects.find( type );
	
	if ( iter != m_objects.end() )
	{
		u32 const current_count				= iter->second.size();
		for ( u32 i = 0; i < current_count; ++i )
		{
			if ( iter->second[i].get_instance() == instance )
				return						i;
		}
	}
	
	return									u32(-1);
}

void specified_problem::dump_target_world_state	( ) const
{
	fixed_string4096						output_text( "target world state:\n" );
	
	u32 const offsets_count					= m_target_offsets.size();
	u32 const world_state_size				= m_target_world_state.size();
	u32 current_index						= 0;

	if ( offsets_count == 0 )
	{
		for ( u32 i = 0; i < m_target_world_state.size(); ++i )
		{
			pddl_world_state_property_impl const& property	= m_target_world_state[i];
			output_text.append				( property.get_predicate()->get_caption() );
			output_text.append				( " (");
			
			for ( u32 j = 0; j < property.get_indices_count(); ++j )
			{
				object_instance const* const object	= get_object_by_type_and_index(
														property.get_predicate()->get_parameter( j ),					
														property.get_index( j )
													  );
				R_ASSERT					( object, "invalid object type and/or index" );
				output_text.append			( object->get_caption() );
				output_text.append			( j == property.get_indices_count() - 1 ? ")" : ", " );
			}
			if ( property.get_indices_count() == 0 )
				output_text.append			( ")" );
			
			if ( i < m_target_world_state.size() - 1 )
				output_text.append			( "\n" );
		}
		LOG_INFO							( output_text.c_str() );
		return;
	}

	for ( u32 j = 0; j <= offsets_count; ++j )
	{
		u32 const current_offset			= j < offsets_count ? m_target_offsets[j] : world_state_size;
		for ( ; current_index < current_offset; ++current_index )
		{
			pddl_world_state_property_impl const& property = m_target_world_state[current_index];
			output_text.append				( property.get_predicate()->get_caption() );
			output_text.append				( " (");
			
			for ( u32 j = 0; j < property.get_indices_count(); ++j )
			{
				object_instance const* const object	= get_object_by_type_and_index(
														property.get_predicate()->get_parameter( j ),					
														property.get_index( j )
													  );
				R_ASSERT					( object, "invalid object type and/or index" );
				output_text.append			( object->get_caption() );
				output_text.append			( j == property.get_indices_count() - 1 ? ")" : ", " );
			}
			if ( property.get_indices_count() == 0 )
				output_text.append			( ")" );

			if ( current_index < m_target_world_state.size() - 1 )
				output_text.append			( "\n" );
		}
		if ( current_index < world_state_size - 1 )
			output_text.append				( "  or  \n" );
	}
	LOG_INFO								( output_text.c_str() );
}

void specified_problem::set_target_state( base_lexeme const& target_expression )
{
	memory::stack_allocator				allocator;
	u32 const memory_size				= target_expression.memory_size_for_brackets_expansion();
	allocator.initialize				( ALLOCA( memory_size ), memory_size, "world state expression" );
	base_lexeme_ptr const new_expression = target_expression.expand_brackets( allocator );
	u32 offset							= 0;
	(*new_expression).add_to_target_world_state( *this, offset );
}

bool specified_problem::are_parameters_suitable	( u32 const action_type, indices_type const& objects_indices ) const
{
	action_instance const* const action		= m_problem.get_action_instance( action_type );
	R_ASSERT								( action, "action instance wasn't found in problem" );

	property_values_type					object_instances;
	for ( u32 i = 0; i < objects_indices.size(); ++i )
	{
		object_instance const* const object	= get_object_by_type_and_index	(
												action->get_parameter_type( i ),					
												objects_indices[i]
											  );
		R_ASSERT							( object, "invalid object type and/or index" );
		object_instances.push_back			( object->get_instance() );
	}
	
	return									action->are_parameters_suitable( object_instances );
}

bool specified_problem::parameter_iterate_first_only	( u32 const action_type, u32 const parameter_index ) const
{
	action_instance const* const action		= m_problem.get_action_instance( action_type );
	R_ASSERT								( action, "action instance wasn't found in problem" );
	return									action->get_parameter( parameter_index )->iterate_only_first();
}

selectors::target_selector_base* specified_problem::get_parameter_selector ( u32 const action_type, u32 const parameter_index ) const
{
	if ( !m_owner )
	{
		LOG_WARNING							( "no brain unit set, selectors are unavailable" );
		return								0;
	}
	
	action_instance const* const action		= m_problem.get_action_instance( action_type );
	R_ASSERT								( action, "action instance wasn't found in problem" );
	selectors::target_selector_base* selector = m_owner->get_selector_by_name( action->get_parameter( parameter_index )->get_selector_name() );
	return									selector;
}

selectors::target_selector_base* specified_problem::get_parameter_selector ( action_parameter const* const parameter ) const
{
	if ( !m_owner )
	{
		LOG_WARNING							( "no brain unit set, selectors are unavailable" );
		return								0;
	}
	
	selectors::target_selector_base* selector = m_owner->get_selector_by_name( parameter->get_selector_name() );
	return									selector;
}

void specified_problem::fill_parameter_targets	( u32 const action_type )
{
	if ( !m_owner )
	{
		LOG_WARNING							( "no brain unit set, selectors are unavailable" );
		return;
	}
	
	action_instance const* const action		= m_problem.get_action_instance( action_type );
	R_ASSERT								( action, "action instance wasn't found in problem" );
	for ( u32 i = 0; i < action->get_parameters_count(); ++i )
		if ( selectors::target_selector_base* selector = m_owner->get_selector_by_name( action->get_parameter( i )->get_selector_name() ) )
			selector->fill_targets_list		( );
}

void specified_problem::fill_parameter_targets	( action_parameter const* const parameter )
{
	if ( selectors::target_selector_base* selector = get_parameter_selector( parameter ) )
		selector->fill_targets_list			( );
}

bool specified_problem::has_no_targets		( action_parameter const* const parameter ) const
{
	selectors::target_selector_base* selector = get_parameter_selector( parameter );
	bool const has_targets					= selector && selector->get_targets_count() > 0;
	return									!has_targets;
}

// void specified_problem::add_owner_as_object	( )
// {
// 	R_ASSERT								( m_owner, "no brain unit set" );
// 	brain_unit const* const owner			= m_owner;
// 	object_instance_type					instance;
// 	object_instance::set_instance			( instance, owner );
// 	add_object_instance						( std::make_pair( instance, typeid( owner ).name() ), m_owner->get_npc().cast_game_object()->get_name() );
// }

object_type specified_problem::get_owner_as_object	( ) const
{
	R_ASSERT								( m_owner, "no owner brain unit set" );
	brain_unit const* const owner			= m_owner;
	object_instance_type					instance;
	object_instance::set_instance			( instance, owner );
	return									std::make_pair( instance, typeid( owner ).name() );
}

pcstr specified_problem::get_owner_caption	( ) const
{
	if ( m_owner )
		return								m_owner->get_npc().cast_game_object()->get_name();

	return									"";
}

} // namespace planning
} // namespace ai
} // namespace xray