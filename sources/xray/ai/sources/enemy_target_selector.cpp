////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "enemy_target_selector.h"
#include "percept_memory_object.h"
#include "percept_memory_object_types.h"
#include "working_memory.h"
#include "ai_world.h"
#include "object_instance.h"
#include <xray/ai/npc.h>
#include <xray/ai/npc_statistics.h>

namespace xray {
namespace ai {
namespace selectors {

enemy_target_selector::enemy_target_selector	( ai_world& world, working_memory const& memory, blackboard& board, pcstr name ) :
	target_selector_base						( world, memory, board, name )
{
}

static float get_recalculated_confidence		( ai_world const& world, percept_memory_object* object )
{
	// after every 1000 milliseconds confidence of memory object must be considered as decreased by 0.05
	return object->confidence - ( ( world.get_current_time_in_ms() - object->update_time ) * 0.05f / 1000.f );
}

struct find_highest_confidence_predicate : private boost::noncopyable
{
	inline find_highest_confidence_predicate	( ai_world const& selector_world ) :
		object_with_highest_confidence			( 0 ),
		world									( selector_world )
	{
	}

	inline void operator()				( percept_memory_object* memory_object )
	{
		object_with_highest_confidence	= !object_with_highest_confidence ? memory_object :
										( get_recalculated_confidence( world, object_with_highest_confidence ) <
										  get_recalculated_confidence( world, memory_object ) ?
										memory_object : object_with_highest_confidence );
	}

	percept_memory_object*				object_with_highest_confidence;
	ai_world const&						world;
};

void enemy_target_selector::tick		( )
{
	find_highest_confidence_predicate	find_predicate( m_world );
	m_working_memory.for_each			( percept_memory_object_type_enemy, find_predicate );
	
	if ( percept_memory_object* memory_object = find_predicate.object_with_highest_confidence )
		m_blackboard.set_current_enemy	( c_ptr( memory_object->object ) );
}

struct fill_list_with_the_best_enemies_predicate : private boost::noncopyable
{
	inline fill_list_with_the_best_enemies_predicate	(
			enemy_target_selector::selected_enemies_type& enemies,
			ai_world const& selector_world
		) :
		enemies_list					( enemies ),
		world							( selector_world )
	{
	}

	inline void operator()				( percept_memory_object* memory_object )
	{
		game_object const* object		= c_ptr( memory_object->object );
		float const confidence			= get_recalculated_confidence( world, memory_object );
		npc const* target				= object->cast_npc();
		R_ASSERT						( target );

		for ( enemy_target_selector::selected_enemies_type::iterator it = enemies_list.begin(); it != enemies_list.end(); ++it )
		{
			if ( it->first == target )
			{
				if ( confidence > it->second )
				{
					enemies_list.erase	( it );
					break;
				}
				else
					return;
			}
		}
		
		if ( enemies_list.size() == enemies_list.max_size() )
		{
			R_ASSERT			( !enemies_list.empty() );
			enemy_target_selector::selected_enemies_type::iterator min_confidence_item = enemies_list.begin();

			for ( enemy_target_selector::selected_enemies_type::iterator it = enemies_list.begin(); it != enemies_list.end(); ++it )
				if ( min_confidence_item->second > it->second )
					min_confidence_item = it;

			if ( confidence > min_confidence_item->second )
				enemies_list.erase( min_confidence_item );
		}
		
		planning::object_instance_type target_npc;
		planning::object_instance::set_instance( target_npc, target );
		
		if ( enemies_list.size() < enemies_list.max_size() )
			enemies_list.push_back		( std::make_pair( target, confidence ) );
	}

	enemy_target_selector::selected_enemies_type&	enemies_list;
	ai_world const&									world;
};

static bool sort_by_confidence	( enemy_target_selector::enemy_type const& object1, enemy_target_selector::enemy_type const& object2 )
{
   return						object1.second > object2.second;
}

void enemy_target_selector::fill_targets_list	( )
{
	clear_targets								( );
	
	fill_list_with_the_best_enemies_predicate	get_best_enemies_predicate( m_selected_enemies, m_world );
	m_working_memory.for_each					( percept_memory_object_type_enemy, get_best_enemies_predicate );
	std::sort									( m_selected_enemies.begin(), m_selected_enemies.end(), sort_by_confidence );
}

void enemy_target_selector::clear_targets		( )
{
	m_selected_enemies.clear					( );
}

planning::object_type enemy_target_selector::get_target( u32 const target_index )
{
	using namespace						planning;

	object_instance_type				instance;
	npc const* target					= m_selected_enemies[target_index].first;
	object_instance::set_instance		( instance, target );
	return								std::make_pair( instance, typeid( target ).name() );
}

pcstr enemy_target_selector::get_target_caption	( u32 const target_index ) const
{
	game_object const* object			= m_selected_enemies[target_index].first->cast_game_object();
	R_ASSERT							( object, "invalid game_object" );
	return								object->get_name();
}

void enemy_target_selector::dump_state	( npc_statistics& stats ) const
{
	npc_statistics::sensor_info_type	new_stats_item;
	new_stats_item.caption				= m_name;
	new_stats_item.caption.append		( " selector state:" );

	for ( u32 i = 0; i < m_selected_enemies.size(); ++i )
		new_stats_item.content.push_back( get_target_caption( i ) );

	if ( m_selected_enemies.empty() )
		new_stats_item.content.push_back( "none" );

	stats.selectors_state.push_back		( new_stats_item );
}

} // namespace selectors
} // namespace ai
} // namespace xray