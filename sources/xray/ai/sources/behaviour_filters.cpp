////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "behaviour.h"
#include "ai_world.h"
#include "enemy_filter.h"
#include "weapon_filter.h"
#include "cover_filter.h"
#include "animation_filter.h"
#include "sound_filter.h"
#include "position_filter.h"
#include "action_instance.h"

namespace xray {
namespace ai {

void behaviour::delete_filter				( planning::base_filter* filter_to_be_deleted )
{
	while ( planning::base_filter* subfilter_to_be_deleted = filter_to_be_deleted->pop_subfilter() )
		delete_filter						( subfilter_to_be_deleted );

	DELETE									( filter_to_be_deleted );
}

void behaviour::delete_parameters_filters	( planning::parameters_filters_type* filters )
{
	using namespace							planning;
	
	while ( parameter_filters_item_type* current_item = filters->pop_front() )
	{
		parameter_filters_type* filter_list	= current_item->list;
		while ( intrusive_filters_list* intrusive_filters = filter_list->pop_front() )
		{
			delete_filter					( intrusive_filters->list );

			DELETE							( intrusive_filters );
		}
		DELETE								( filter_list );
		DELETE								( current_item );
	}
	DELETE									( filters );
}

static void parse_enemy_filter						(
		configs::binary_config_value const& filter_options,
		ai_world& world,
		planning::enemy_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	enemy_filter_types_enum const subfilter_type	= (enemy_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	typedef boost::function< u32 ( pcstr ) >		find_by_name_function;
	find_by_name_function							selector;

	switch ( subfilter_type )
	{
		case enemy_filter_type_group:
			selector								= boost::bind( &ai_world::get_group_id_by_name, &world, _1 );
		break;

		case enemy_filter_type_character:
			selector								= boost::bind( &ai_world::get_character_id_by_name, &world, _1 );
		break;

		case enemy_filter_type_class:
			selector								= boost::bind( &ai_world::get_class_id_by_name, &world, _1 );
		break;

		case enemy_filter_type_outfit:
			selector								= boost::bind( &ai_world::get_outfit_id_by_name, &world, _1 );
		break;

		default:
			NODEFAULT								( );
	}
	
	configs::binary_config_value const& values		= filter_options["names"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		pcstr const name							= (pcstr)value;
		u32 const id								= selector( name );
		if ( id != u32(-1) )
			filter.add_filtered_id					( id );
	}
}

static void parse_weapon_filter						(
		configs::binary_config_value const& filter_options,
		ai_world& world,
		planning::weapon_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	weapon_filter_types_enum const subfilter_type	= (weapon_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	typedef boost::function< u32 ( pcstr ) >		find_by_name_function;
	find_by_name_function							selector;
	
	switch ( subfilter_type )
	{
		case weapon_filter_type_melee:
			selector								= boost::bind( &ai_world::get_melee_weapon_id_by_name, &world, _1 );
		break;

		case weapon_filter_type_sniper:
			selector								= boost::bind( &ai_world::get_sniper_weapon_id_by_name, &world, _1 );
		break;

		case weapon_filter_type_heavy:
			selector								= boost::bind( &ai_world::get_heavy_weapon_id_by_name, &world, _1 );
		break;

		case weapon_filter_type_light:
			selector								= boost::bind( &ai_world::get_light_weapon_id_by_name, &world, _1 );
		break;

		case weapon_filter_type_energy:
			selector								= boost::bind( &ai_world::get_energy_weapon_id_by_name, &world, _1 );
		break;

		default:
			NODEFAULT								( );
	}
	
	configs::binary_config_value const& values		= filter_options["weapons"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		pcstr const name							= (pcstr)value;
		u32 const id								= selector( name );
		if ( id != u32(-1) )
			filter.add_filtered_id					( id );
	}
}

static void parse_cover_filter						(
		configs::binary_config_value const& filter_options,
		ai_world& world,
		planning::cover_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	cover_filter_types_enum const subfilter_type	= (cover_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	typedef boost::function< u32 ( pcstr ) >		find_by_name_function;
	find_by_name_function							selector;

	switch ( subfilter_type )
	{
		case cover_filter_type_node:
			selector								= boost::bind( &ai_world::get_node_id_by_name, &world, _1 );
		break;

		default:
			NODEFAULT								( );
	}
	
	configs::binary_config_value const& values		= filter_options["nodes"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		pcstr const name							= (pcstr)value;
		u32 const id								= selector( name );
		if ( id != u32(-1) )
			filter.add_filtered_id					( id );
	}
}

static void parse_animation_filter					(
		configs::binary_config_value const& filter_options,
		planning::animation_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	animation_filter_types_enum const subfilter_type = (animation_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	configs::binary_config_value const& values		= filter_options["filenames"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		pcstr const name							= (pcstr)value["name"];
		filter.add_filtered_item					( name );
	}
}

static void parse_sound_filter						(
		configs::binary_config_value const& filter_options,
		planning::sound_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	sound_filter_types_enum const subfilter_type	= (sound_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	configs::binary_config_value const& values		= filter_options["filenames"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		pcstr const name							= (pcstr)value["name"];
		filter.add_filtered_item					( name );
	}
}

static void parse_position_filter					(
		configs::binary_config_value const& filter_options,
		planning::position_filter& filter
	)
{
	u32	const subfilter_id							= filter_options["subtype"];
	position_filter_types_enum const subfilter_type	= (position_filter_types_enum)subfilter_id;
	filter.set_filter_type							( subfilter_type );
	
	configs::binary_config_value const& values		= filter_options["positions"];
			
	configs::binary_config_value::const_iterator it		= values.begin();
	configs::binary_config_value::const_iterator it_end = values.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value	= *it;
		float3 const& position						= value["target_point"];
		float3 const& direction						= value["direction"];
		float3 const& velocity						= value["velocity"];
		pcstr const animation_name					= value["animation"]["name"];
		filter.add_filtered_item					( position, direction, velocity, animation_name );
	}
}

static planning::base_filter* create_filter			(
		configs::binary_config_value const& filter_options,
		ai_world& world
	)
{
	u32	const filter_id								= filter_options["type"];
	filter_types_enum const filter_type				= (filter_types_enum)filter_id;
	bool const is_inverted							= filter_options.value_exists( "is_inverted" ) ? filter_options["is_inverted"] : false;

	planning::base_filter* result					= 0;

	if ( filter_type == filter_type_enemy )
	{
		planning::enemy_filter* filter				= NEW( planning::enemy_filter )( is_inverted );
		parse_enemy_filter							( filter_options, world, *filter );
		result										= filter;
	}
	else if ( filter_type == filter_type_weapon )
	{
		planning::weapon_filter* filter				= NEW( planning::weapon_filter )( is_inverted );
		parse_weapon_filter							( filter_options, world, *filter );
		result										= filter;
	}
	else if ( filter_type == filter_type_cover )
	{
		planning::cover_filter* filter				= NEW( planning::cover_filter )( is_inverted );
		parse_cover_filter							( filter_options, world, *filter );
		result										= filter;
	}
	else if ( filter_type == filter_type_animation )
	{
		planning::animation_filter* filter			= NEW( planning::animation_filter )( is_inverted );
		parse_animation_filter						( filter_options, *filter );
		result										= filter;
	}
	else if ( filter_type == filter_type_sound )
	{
		planning::sound_filter* filter				= NEW( planning::sound_filter )( is_inverted );
		parse_sound_filter							( filter_options, *filter );
		result										= filter;
	}
	else if ( filter_type == filter_type_position )
	{
 		planning::position_filter* filter			= NEW( planning::position_filter )( is_inverted );
 		parse_position_filter						( filter_options, *filter );
 		result										= filter;
	}

	R_ASSERT										( result );

	if ( filter_options.value_exists( "filters" ) )
	{
		configs::binary_config_value const& values	= filter_options["filters"];
			
		configs::binary_config_value::const_iterator it		= values.begin();
		configs::binary_config_value::const_iterator it_end = values.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			planning::base_filter* subfilter				= create_filter( value, world );
			result->add_subfilter							( subfilter );
		}
	}
	
	return											result;
}

static planning::parameter_filters_type* create_parameter_filters	(
		configs::binary_config_value const& options,
		ai_world& world,
		pcstr parameter_number
	)
{
	using namespace											planning;
	parameter_filters_type* result							= 0;
	
	if ( options.value_exists( parameter_number ) )
	{
		result												= NEW( parameter_filters_type );
		configs::binary_config_value const& filters			= options[parameter_number];
		
		configs::binary_config_value::const_iterator it		= filters.begin();
		configs::binary_config_value::const_iterator it_end = filters.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			intrusive_filters_list* new_list				= NEW( intrusive_filters_list )( create_filter( value, world ) );
			result->push_back								( new_list );
		}
	}
	return													result;
}

static planning::parameters_filters_type* create_parameters_filters(
		configs::binary_config_value const& parameters_options,
		ai_world& world
	)
{
	using namespace							planning;
	
	parameters_filters_type* filters		= NEW( parameters_filters_type );
	
	if ( parameter_filters_type* parameter_filters = create_parameter_filters( parameters_options, world, "parameter0_filter" ) )
	{
		parameter_filters_item_type* item	= NEW( parameter_filters_item_type )( parameter_filters );	
		filters->push_back					( item );
	}
	if ( parameter_filters_type* parameter_filters = create_parameter_filters( parameters_options, world, "parameter1_filter" ) )
	{
		parameter_filters_item_type* item	= NEW( parameter_filters_item_type )( parameter_filters );	
		filters->push_back					( item );
	}
	if ( parameter_filters_type* parameter_filters = create_parameter_filters( parameters_options, world, "parameter2_filter" ) )
	{
		parameter_filters_item_type* item	= NEW( parameter_filters_item_type )( parameter_filters );	
		filters->push_back					( item );
	}
	if ( parameter_filters_type* parameter_filters = create_parameter_filters( parameters_options, world, "parameter3_filter" ) )
	{
		parameter_filters_item_type* item	= NEW( parameter_filters_item_type )( parameter_filters );	
		filters->push_back					( item );
	}

	return									filters;
}

void behaviour::fill_goal_filter_sets						(
		configs::binary_config_value const& options,
		ai_world& world,
		planning::goal& owner
	)
{
	using namespace											planning;

	if ( options.value_exists( "filter_sets" ) )
	{
		configs::binary_config_value const& filters			= options["filter_sets"];
			
		configs::binary_config_value::const_iterator it		= filters.begin();
		configs::binary_config_value::const_iterator it_end = filters.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			parameters_filters_type* parameters_filters		= create_parameters_filters( value, world );
			owner.add_filters_list							( parameters_filters );
		}
	}
}

void behaviour::fill_action_filter_sets						(
		configs::binary_config_value const& options,
		ai_world& world,
		planning::action_instance& owner
	)
{
	using namespace											planning;

	if ( options.value_exists( "filter_sets" ) )
	{
		configs::binary_config_value const& filters			= options["filter_sets"];
			
		configs::binary_config_value::const_iterator it		= filters.begin();
		configs::binary_config_value::const_iterator it_end = filters.end();

		for ( ; it != it_end; ++it )
		{
			configs::binary_config_value const& value		= *it;
			parameters_filters_type* parameters_filters		= create_parameters_filters( value, world );
			owner.add_filters_list							( parameters_filters );
		}
	}
}

void behaviour::fill_ignorance_filter	( configs::binary_config_value const& options, ai_world& world )
{
	configs::binary_config_value::const_iterator it		= options.begin();
	configs::binary_config_value::const_iterator it_end = options.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& value		= *it;
		m_ignorance_filter.add_aux_filter				( create_filter( value, world ) );
	}
}

} // namespace ai
} // namespace xray