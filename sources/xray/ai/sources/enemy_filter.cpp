////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "enemy_filter.h"
#include <xray/ai/npc.h>

namespace xray {
namespace ai {
namespace planning {

enemy_filter::enemy_filter				( bool need_to_be_inverted ) :
	base_filter							( need_to_be_inverted )
{
}

enemy_filter::~enemy_filter				( )
{
}

void enemy_filter::add_filtered_id		( filtered_id const id )
{
	m_filtered_ids.push_back			( id );
}

bool enemy_filter::contains_object_id	( filtered_id const id ) const
{
	for ( filtered_ids_type::const_iterator iter = m_filtered_ids.begin(); iter != m_filtered_ids.end(); ++iter )
		if ( *iter == id )
			return						true;

	return								false;
}

bool enemy_filter::is_passing_filter	( object_instance_type const& object ) const
{
	u32 object_id						= u32(-1);
	npc const* const object_npc			= static_cast_checked< npc const* >( object );
	R_ASSERT							( object_npc, "invalid game object, only npc can be treated as enemies" );
	
	switch ( m_filter_type )
	{
		case enemy_filter_type_group:
			object_id					= object_npc->get_group_id();
		break;

		case enemy_filter_type_character:
			object_id					= object_npc->cast_game_object()->get_id();
		break;

		case enemy_filter_type_class:
			object_id					= object_npc->get_class_id();
		break;

		case enemy_filter_type_outfit:
			object_id					= object_npc->get_outfit_id();
		break;
	}
	
	R_ASSERT							( object_id != u32(-1), "id not found in dictionary for type %d", m_filter_type );
	
	if ( !contains_object_id( object_id ) )
		return							false;
	
	for ( intrusive_filters_list* it_filter = m_subfilters.front(); it_filter; it_filter = m_subfilters.get_next_of_object( it_filter ) )
		if ( !it_filter->list->is_object_available( object ) )
			return						false;

	return								true;
}

} // namespace planning
} // namespace ai
} // namespace xray