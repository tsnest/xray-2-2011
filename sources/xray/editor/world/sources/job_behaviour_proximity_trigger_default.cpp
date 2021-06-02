////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_proximity_trigger_default.h"

using xray::editor::wpf_controls::property_descriptor;

namespace xray {
namespace editor {

////////////////		I N I T I A L I Z E			///////////////

job_behaviour_proximity_trigger_default::job_behaviour_proximity_trigger_default ( object_job^ parent_job )
	:job_behaviour_base( parent_job )
{
	testees_source = "HUD";
}

////////////////		P U B L I C   M E T H O D S			////////////////

void job_behaviour_proximity_trigger_default::load ( configs::lua_config_value const& config )
{
	testees_source				= gcnew String( (pcstr)config["testees_source"] );
	job_behaviour_base::load( config );
}

void job_behaviour_proximity_trigger_default::save ( configs::lua_config_value& config )
{
	config["testees_source"]	= unmanaged_string( testees_source ).c_str( );	
	job_behaviour_base::save( config );
}

List<String^>^ job_behaviour_proximity_trigger_default::get_available_events_list ( )
{
	List<String^>^ events = gcnew List<String^>( );
	events->Add( "on enter" );
	events->Add( "on exit" );
	return events;
}

void job_behaviour_proximity_trigger_default::get_properties ( wpf_property_container^ to_container )
{
	to_container->properties->add( gcnew property_descriptor( this, "testees_source" ) );

	job_behaviour_base::get_properties( to_container );
}


} // namespace editor
} // namespace xray