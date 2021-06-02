////////////////////////////////////////////////////////////////////////////
//	Created		: 28.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "animation_target_selector.h"
#include "object_instance.h"
#include "brain_unit.h"
#include <xray/ai/npc_statistics.h>

namespace xray {
namespace ai {
namespace selectors {

animation_target_selector::animation_target_selector	(
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

void animation_target_selector::tick( )
{
}

planning::object_type animation_target_selector::get_target	( u32 const target_index )
{
	using namespace					planning;	
	
	object_instance_type			instance;
	object_instance::set_instance	( instance, m_selected_animations[target_index] );
	return							std::make_pair( instance, typeid( m_selected_animations[target_index] ).name() );
}

 static bool sort_by_name			( animation_item const* object1, animation_item const* object2 )
 {
	 return							strings::less( object1->name.c_str(), object2->name.c_str() );
 }

void animation_target_selector::fill_targets_list	( )
{
	clear_targets									( );
	m_brain_unit.get_available_animations			( m_selected_animations );
	std::sort										( m_selected_animations.begin(), m_selected_animations.end(), sort_by_name );
}

void animation_target_selector::clear_targets		( )
{
	m_selected_animations.clear						( );
}

pcstr animation_target_selector::get_target_caption	( u32 const target_index ) const
{
	animation_item const* target		= m_selected_animations[target_index];
	R_ASSERT							( target );
	return								fs_new::file_name_from_path( target->name );
}

void animation_target_selector::dump_state			( npc_statistics& stats ) const
{
	npc_statistics::sensor_info_type	new_stats_item;
	new_stats_item.caption				= m_name;
	new_stats_item.caption.append		( " selector state:" );

	for ( u32 i = 0; i < m_selected_animations.size(); ++i )
		new_stats_item.content.push_back( get_target_caption( i ) );

	if ( m_selected_animations.empty() )
		new_stats_item.content.push_back( "none" );

	stats.selectors_state.push_back		( new_stats_item );
}

} // namespace selectors
} // namespace ai
} // namespace xray
