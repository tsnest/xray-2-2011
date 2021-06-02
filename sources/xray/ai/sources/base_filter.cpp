////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "base_filter.h"

namespace xray {
namespace ai {
namespace planning {

base_filter::base_filter					( bool need_to_be_inverted ) :
	m_is_inverted							( need_to_be_inverted )
{
}

bool base_filter::is_object_available		( object_instance_type const& object ) const
{
	return m_is_inverted ? !is_passing_filter( object ) : is_passing_filter( object );
}

void base_filter::add_subfilter				( base_filter* filter )
{
	intrusive_filters_list* new_subfilter	= NEW( intrusive_filters_list )( filter );
	m_subfilters.push_back					( new_subfilter );
}

base_filter* base_filter::pop_subfilter		( )
{
	if ( m_subfilters.empty() )
		return								0;

	intrusive_filters_list* subfilter		= m_subfilters.pop_front();
	base_filter* result						= subfilter->list;
	DELETE									( subfilter );
	return									result;
}

} // namespace planning
} // namespace ai
} // namespace xray