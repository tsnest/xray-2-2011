////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "position_target_selector.h"
#include "object_instance.h"
#include "brain_unit.h"
#include <xray/ai/npc_statistics.h>
#include <xray/ai/npc.h>

namespace xray {
namespace ai {
namespace selectors {

position_target_selector::position_target_selector	(
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

void position_target_selector::tick	( )
{
}

planning::object_type position_target_selector::get_target	( u32 const target_index )
{
	using namespace					planning;	
	
	object_instance_type			instance;
	object_instance::set_instance	( instance, m_selected_positions[target_index] );
	return							std::make_pair( instance, typeid( m_selected_positions[target_index] ).name() );
}

struct sort_by_distance_predicate
{
	inline sort_by_distance_predicate( brain_unit const& owner_brain ) :
		brain						( &owner_brain )
	{
	}

	bool operator ()				( movement_target const* object1, movement_target const* object2 ) const
	{
		float3 const& position		= brain->get_npc().get_position( float3( 0.f, 0.f, 0.f ) );
		float const length1			= ( object1->target_position - position ).length();
		float const length2			= ( object2->target_position - position ).length();
		return						length1 < length2;
	}

	brain_unit const*				brain;
};

void position_target_selector::fill_targets_list( )
{
	clear_targets								( );
	m_brain_unit.get_available_movement_targets	( m_selected_positions );
	std::sort									( m_selected_positions.begin(), m_selected_positions.end(), sort_by_distance_predicate( m_brain_unit ) );
}

void position_target_selector::clear_targets	( )
{
	m_selected_positions.clear					( );
}

pcstr position_target_selector::get_target_caption	( u32 const target_index ) const
{
	movement_target const* target					= m_selected_positions[target_index];
	R_ASSERT										( target );
	return											target->caption.c_str();
}

void position_target_selector::dump_state		( npc_statistics& stats ) const
{
	npc_statistics::sensor_info_type			new_stats_item;
	new_stats_item.caption						= m_name;
	new_stats_item.caption.append				( " selector state:" );

	for ( u32 i = 0; i < m_selected_positions.size(); ++i )
		new_stats_item.content.push_back		( get_target_caption( i ) );

	if ( m_selected_positions.empty() )
		new_stats_item.content.push_back		( "none" );

	stats.selectors_state.push_back				( new_stats_item );
}

} // namespace selectors
} // namespace ai
} // namespace xray