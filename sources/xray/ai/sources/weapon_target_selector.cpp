////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "weapon_target_selector.h"
#include <xray/ai/engine.h>
#include "ai_world.h"
#include "object_instance.h"
#include <xray/ai/npc_statistics.h>

namespace xray {
namespace ai {
namespace selectors {

weapon_target_selector::weapon_target_selector	(
	ai_world& world,
	working_memory const& memory,
	blackboard& board,
	brain_unit& brain,
	pcstr name
	) :
	target_selector_base			( world, memory, board, name ),
	m_brain_unit					( brain )
{
}

void weapon_target_selector::tick	( )
{
}

planning::object_type weapon_target_selector::get_target	( u32 const target_index )
{
	using namespace					planning;	
	
	object_instance_type			instance;
	object_instance::set_instance	( instance, m_selected_weapons[target_index].first );
	return							std::make_pair( instance, typeid( m_selected_weapons[target_index].first ).name() );
}

static bool sort_by_type	( weapon_target_selector::weapon_type const& object1, weapon_target_selector::weapon_type const& object2 )
{
   return					object1.second < object2.second;
}

void weapon_target_selector::fill_targets_list	( )
{
	clear_targets								( );
	weapons_list available_weapons				( g_allocator );
	m_world.get_available_weapons				( &m_brain_unit.get_npc(), available_weapons );
	
	for ( u32 i = 0; i < available_weapons.size(); ++i )
	{
		weapon const* available_weapon			= available_weapons[i];
		
		// TODO: if there is a heavier weapon but list is full - replace the wickest item
		planning::object_instance_type			target_weapon;
		planning::object_instance::set_instance	( target_weapon, available_weapon );
		if ( m_selected_weapons.size() < m_selected_weapons.max_size() )
			m_selected_weapons.push_back		( std::make_pair( available_weapon, available_weapon->get_type() ) );
	}

	std::sort									( m_selected_weapons.begin(), m_selected_weapons.end(), sort_by_type );
}

void weapon_target_selector::clear_targets		( )
{
	m_selected_weapons.clear					( );
}

pcstr weapon_target_selector::get_target_caption( u32 const target_index ) const
{
	game_object const* object			= m_selected_weapons[target_index].first->cast_game_object();
	R_ASSERT							( object, "invalid game_object" );
	return								object->get_name();
}

void weapon_target_selector::dump_state	( npc_statistics& stats ) const
{
	npc_statistics::sensor_info_type	new_stats_item;
	new_stats_item.caption				= m_name;
	new_stats_item.caption.append		( " selector state:" );

	for ( u32 i = 0; i < m_selected_weapons.size(); ++i )
		new_stats_item.content.push_back( get_target_caption( i ) );

	if ( m_selected_weapons.empty() )
		new_stats_item.content.push_back( "none" );

	stats.selectors_state.push_back		( new_stats_item );
}

} // namespace selectors
} // namespace ai
} // namespace xray