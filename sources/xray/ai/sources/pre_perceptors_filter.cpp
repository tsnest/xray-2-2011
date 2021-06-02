////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "pre_perceptors_filter.h"
#include "object_instance.h"

namespace xray {
namespace ai {

pre_perceptors_filter::pre_perceptors_filter( )
{
}

void pre_perceptors_filter::ignore			( ignorable_game_object const* begin, ignorable_game_object const* end )
{
	using namespace							planning;
	
	m_ignorable_objects.clear				( );

	typedef ignorable_game_object const*	const_iterator;
	const_iterator iter						= begin;
	const_iterator iter_end					= end;
	for ( ; iter != iter_end; ++iter )
	{
		object_instance_type				instance;
		object_instance::set_instance		( instance, c_ptr( iter->first ) );
		bool can_be_ignored					= true;
		for ( base_filter* it_filter = m_aux_filters.front(); it_filter; it_filter = m_aux_filters.get_next_of_object( it_filter ) )
			can_be_ignored					&= it_filter->is_object_available( instance );

		if ( can_be_ignored )
			m_ignorable_objects.push_back	( *iter );
	}
}

ignorable_game_object* pre_perceptors_filter::find_ignored_object( game_object const* const object )
{
	for ( u16 i = 0; i < m_ignorable_objects.size(); ++i )
		if ( m_ignorable_objects[i].first == object )
			return							&m_ignorable_objects[i];

	return									0;
}

ignorable_game_object const* pre_perceptors_filter::find_ignored_object( game_object const* const object ) const
{
	for ( u16 i = 0; i < m_ignorable_objects.size(); ++i )
		if ( m_ignorable_objects[i].first == object )
			return							&m_ignorable_objects[i];

	return									0;
}

void pre_perceptors_filter::stop_ignoring	( game_object const* const object )
{
	ignorable_game_object* found_object		= find_ignored_object( object );
	if ( found_object && found_object->second == ignorance_type_until_hit )
		m_ignorable_objects.erase			( found_object );
}

bool pre_perceptors_filter::is_object_ignored	( game_object const* const object ) const
{
	return find_ignored_object( object )	!= 0;
}

void pre_perceptors_filter::on_hit_event	( game_object const* const hitting_object )
{
	stop_ignoring							( hitting_object );
}

void pre_perceptors_filter::add_aux_filter	( planning::base_filter* const aux_filter )
{
	m_aux_filters.push_back					( aux_filter );
}

planning::base_filter* pre_perceptors_filter::pop_aux_filter	( )
{
	return m_aux_filters.pop_front			( );
}

} // namespace ai
} // namespace xray