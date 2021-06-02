////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "composite_object_behaviour.h"
#include "object_values_storage.h"
#include "object_scene.h"
#include "game_world.h"

namespace stalker2{

composite_object_behaviour::composite_object_behaviour( object_scene_job* owner, 
	configs::binary_config_value const& data, 
	pcstr name )
	:super	( owner, data, name ),
	m_object		( NULL )
{
	configs::binary_config_value::const_iterator values_it		= data["values"].begin();
	configs::binary_config_value::const_iterator values_it_e	= data["values"].end();
	for ( ;values_it != values_it_e; ++values_it  )
	{
		m_storage_items_list.push_back( storage_item( (pcstr)values_it->key(), (pcstr)(*values_it) ) );
	}
}
void composite_object_behaviour::attach_to_object( object_controlled* o )
{
	m_object = o;
	super::attach_to_object		( o );	
	vector< storage_item >::iterator values_iterator = m_storage_items_list.begin();
	vector< storage_item >::iterator values_iterator_end = m_storage_items_list.end();
	for ( ; values_iterator != values_iterator_end; ++values_iterator )
	{
		
		dynamic_cast< object_values_storage* >( m_owner->get_game_world().get_object_by_name( (*values_iterator).name ).c_ptr() )->set_value( (*values_iterator).value );
	}
}

void composite_object_behaviour::detach_from_object( object_controlled* o )
{
	m_object = NULL;
	super::detach_from_object( o );
}
}