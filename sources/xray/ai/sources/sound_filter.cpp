////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "sound_filter.h"
#include <xray/ai/sound_item.h>

namespace xray {
namespace ai {
namespace planning {

sound_filter::sound_filter				( bool need_to_be_inverted ) :
	base_filter							( need_to_be_inverted )
{
}

sound_filter::~sound_filter				( )
{
}

void sound_filter::add_filtered_item	( pcstr item )
{
	m_filtered_items.push_back			( item );
}

bool sound_filter::contains_object		( fs_new::virtual_path_string const& item ) const
{
	for ( filtered_items_type::const_iterator iter = m_filtered_items.begin(); iter != m_filtered_items.end(); ++iter )
		if ( *iter == item )
			return						true;

	return								false;
}

bool sound_filter::is_passing_filter	( object_instance_type const& object ) const
{
	sound_item const* const sound		= static_cast_checked< sound_item const* >( object );
	R_ASSERT							( sound, "invalid object" );
	
	if ( !contains_object( sound->name ) )
		return								false;
	
	for ( intrusive_filters_list* it_filter = m_subfilters.front(); it_filter; it_filter = m_subfilters.get_next_of_object( it_filter ) )
		if ( !it_filter->list->is_object_available( object ) )
			return							false;

	return									true;
}

} // namespace planning
} // namespace ai
} // namespace xray