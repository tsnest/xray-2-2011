////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "position_filter.h"
#include <xray/ai/movement_target.h>
#include <xray/ai/animation_item.h>

namespace xray {
namespace ai {
namespace planning {

movement_target_wrapper::movement_target_wrapper	(		
		float3 const& target_position,
		float3 const& eyes_direction,
		float3 const& preferable_velocity,		
		pcstr animation
	) :
	position							( target_position ),
	direction							( eyes_direction ),
	velocity							( preferable_velocity ),
	animation_name						( animation )
{
}

position_filter::position_filter		( bool need_to_be_inverted ) :
	base_filter							( need_to_be_inverted )
{
}

position_filter::~position_filter		( )
{
}

void position_filter::add_filtered_item	(
		float3 const& target_position,
		float3 const& eyes_direction,
		float3 const& preferable_velocity,		
		pcstr animation
	)
{
	m_filtered_items.push_back			( movement_target_wrapper( target_position, eyes_direction, preferable_velocity, animation ) );
}

static bool are_targets_similar			( movement_target const& first, movement_target_wrapper const& second )
{
		return							first.target_position == second.position &&
										first.direction == second.direction &&
										first.velocity == second.velocity &&
										strings::equal( first.preferable_animation->name.c_str(), second.animation_name.c_str() );
}

bool position_filter::contains_item		( movement_target const& item ) const
{
	for ( filtered_items_type::const_iterator iter = m_filtered_items.begin(); iter != m_filtered_items.end(); ++iter )
		if ( are_targets_similar( item, *iter ) )
			return						true;

	return								false;
}

bool position_filter::is_passing_filter	( object_instance_type const& object ) const
{
	movement_target const* const target	= static_cast_checked< movement_target const* >( object );
	R_ASSERT							( target, "invalid object" );
	
	if ( !contains_item( *target ) )
		return							false;
	
	for ( intrusive_filters_list* it_filter = m_subfilters.front(); it_filter; it_filter = m_subfilters.get_next_of_object( it_filter ) )
		if ( !it_filter->list->is_object_available( object ) )
			return						false;

	return								true;
}

} // namespace planning
} // namespace ai
} // namespace xray