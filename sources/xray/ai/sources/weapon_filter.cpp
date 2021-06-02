////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "weapon_filter.h"
#include <xray/ai/weapon.h>

namespace xray {
namespace ai {
namespace planning {

weapon_filter::weapon_filter			( bool need_to_be_inverted ) :
	base_filter							( need_to_be_inverted )
{
}

weapon_filter::~weapon_filter			( )
{
}

void weapon_filter::add_filtered_id		( filtered_weapon_id const id )
{
	m_filtered_ids.push_back			( id );
}

bool weapon_filter::contains_object_id	( filtered_weapon_id const id ) const
{
	for ( filtered_ids_type::const_iterator iter = m_filtered_ids.begin(); iter != m_filtered_ids.end(); ++iter )
		if ( *iter == id )
			return						true;

	return								false;
}

bool weapon_filter::is_passing_filter	( object_instance_type const& object ) const
{
	weapon const* const object_weapon	= static_cast_checked< weapon const* >( object );
	R_ASSERT							( object_weapon, "invalid object, only weapons allowed" );

	u32 const object_id					= object_weapon->cast_game_object()->get_id();
	R_ASSERT							( object_id != u32(-1), "id not found in dictionary for type %d", m_filter_type );
	
	if ( object_weapon->get_type() != m_filter_type )
		return							false;
	
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