////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "blackboard.h"
#include <xray/ai/game_object.h>
#include "goal.h"
#include <xray/ai/npc_statistics.h>
#include <xray/ai/goal_types.h>

namespace xray {
namespace ai {

blackboard::blackboard		( ) :
	m_current_enemy			( 0 ),
	m_current_threat		( 0 ),
	m_current_pickup_item	( 0 ),
	m_current_disturbance	( 0 ),
	m_current_weapon		( 0 ),
	m_current_goal			( 0 )
{
}

blackboard::~blackboard		( )
{
	clear					( );
}

void blackboard::clear		( )
{
	delete_played_animations( );
	delete_played_sounds	( );
	
	m_current_enemy			= 0;
	m_current_threat		= 0;
	m_current_pickup_item	= 0;
	m_current_disturbance	= 0;
	m_current_weapon		= 0;
	m_current_goal			= 0;
}

void blackboard::dump_state		( npc_statistics& stats ) const
{
	stats.blackboard_state.caption = "blackboard state:";
	
	if ( m_current_goal )
	{
		typedef npc_statistics::blackboard_info_type::content_type content_type;
		
		content_type			new_item_content( "current goal: " );
		new_item_content.append	( m_current_goal->get_caption() );
		stats.blackboard_state.content.push_back( new_item_content );

		new_item_content.clear	( );
		new_item_content.appendf( "priority: %u", m_current_goal->get_priority() );
		stats.blackboard_state.content.push_back( new_item_content );
		
		// TODO: include parameters info
	}
	if ( stats.blackboard_state.content.empty() )
		stats.blackboard_state.content.push_back( "no entries" );
}

void blackboard::add_played_animation	( animation_item const* const collection )
{
	m_played_animations.push_back		( NEW( animation_item_wrapper )( collection ) );
}

void blackboard::add_played_sound		( sound_item const* const item )
{
	m_played_sounds.push_back			( NEW( sound_item_wrapper )( item ) );
}

void blackboard::delete_played_animations	( )
{
	while ( animation_item_wrapper* item = m_played_animations.pop_front() )
		DELETE								( item );
}

void blackboard::delete_played_sounds		( )
{
	while ( sound_item_wrapper* item = m_played_sounds.pop_front() )
		DELETE								( item );
}

struct find_animation_collection_item_predicate : private boost::noncopyable
{
	inline	find_animation_collection_item_predicate( animation_item const* const collection_item ) :
		collection							( collection_item )
	{
	}
	
	bool	operator	( )					( animation_item_wrapper const* const item ) const
	{
		return								item->animation == collection;
	}

	animation_item const* const		collection;
};

bool blackboard::is_animation_played		( animation_item const* const collection ) const
{
	return m_played_animations.find_if		( find_animation_collection_item_predicate( collection ) ) != 0;
}

struct find_sound_item_predicate : private boost::noncopyable
{
	inline	find_sound_item_predicate		( sound_item const* const item ) :
		sound								( item )
	{
	}
	
	bool	operator	( )					( sound_item_wrapper* item ) const
	{
		return								item->sound == sound;
	}

	sound_item const* const					sound;
};

bool blackboard::is_sound_played			( sound_item const* const item ) const
{
	return m_played_sounds.find_if			( find_sound_item_predicate( item ) ) != 0;
}

} // namespace ai
} // namespace xray