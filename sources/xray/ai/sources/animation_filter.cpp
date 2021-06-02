////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "animation_filter.h"
#include <xray/ai/animation_item.h>

namespace xray {
namespace ai {
namespace planning {

animation_filter::animation_filter		( bool need_to_be_inverted ) :
	base_filter							( need_to_be_inverted )
{
}

animation_filter::~animation_filter		( )
{
}

void animation_filter::add_filtered_item( pcstr item )
{
	m_filtered_items.push_back			( item );
}

bool animation_filter::contains_item	( fs_new::virtual_path_string const& item ) const
{
	for ( filtered_items_type::const_iterator iter = m_filtered_items.begin(); iter != m_filtered_items.end(); ++iter )
		if ( item == *iter )
			return						true;

	return								false;
}

bool animation_filter::is_passing_filter	( object_instance_type const& object ) const
{
	animation_item const* const collection	= static_cast_checked< animation_item const* >( object );
	R_ASSERT								( collection, "invalid object" );
	
	if ( !contains_item( collection->name ) )
		return								false;
	
	for ( intrusive_filters_list* it_filter = m_subfilters.front(); it_filter; it_filter = m_subfilters.get_next_of_object( it_filter ) )
		if ( !it_filter->list->is_object_available( object ) )
			return							false;

	return									true;
}

} // namespace planning
} // namespace ai
} // namespace xray
