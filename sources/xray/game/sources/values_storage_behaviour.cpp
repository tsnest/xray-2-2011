////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "values_storage_behaviour.h"
#include "object_values_storage.h"
#include "object_scene.h"

namespace stalker2{

values_storage_behaviour::values_storage_behaviour( object_scene_job* owner, 
configs::binary_config_value const& data, 
pcstr name )
:super	( owner, data, name ),
m_object		( NULL )
{
}
void values_storage_behaviour::attach_to_object( object_controlled* o )
{
	m_object	 = o;
	super::attach_to_object		( o );	
	object_values_storage* values_storage = static_cast_checked<object_values_storage*>( o );
	values_storage->set_value_changed_callback( fastdelegate::FastDelegate< void (pcstr) >( this, &values_storage_behaviour::attached_storage_value_changed ) );
}

void values_storage_behaviour::detach_from_object( object_controlled* o )
{
	m_object = NULL;
	super::detach_from_object( o );
}

void values_storage_behaviour::attached_storage_value_changed( pcstr value )
{
 	game_event ev( value );
 	m_owner->get_event_manager()->emit_event ( ev, m_object );
}

}