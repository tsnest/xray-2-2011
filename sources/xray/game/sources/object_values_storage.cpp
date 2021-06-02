////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_values_storage.h"

namespace stalker2
{

object_values_storage::object_values_storage			( game_world& w )
:super( w )
{
	m_current_value_initialized = false;
}

void	object_values_storage::load						( configs::binary_config_value const& t )
{
	m_current_value = ( pcstr )t["selected_value"];
}

void	object_values_storage::set_value		( pcstr value )
{
	m_current_value = value;
 	if ( m_value_changed_callback )
 		m_value_changed_callback( value );

}
void	object_values_storage::set_value_changed_callback		( const fastdelegate::FastDelegate< void (pcstr) >	&value_changed_callback )
{ 
	m_value_changed_callback = value_changed_callback; 	
};

void	object_values_storage::on_scene_start					( )
{
	super::on_scene_start();
	if ( !m_current_value_initialized )
	{
		m_current_value_initialized = true;
		m_value_changed_callback( m_current_value );
	}
}

} //namespace stalker2