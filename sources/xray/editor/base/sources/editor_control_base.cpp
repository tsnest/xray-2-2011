////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_control_base.h"

namespace xray {
namespace editor_base {

void editor_control_base::activate( bool b_activate )
{
	if(b_activate)
		on_activated(this);
	else
		on_deactivated(this);
}

void editor_control_base::subscribe_on_activated( activate_control_event^ d)
{
	on_activated += d;
}

void editor_control_base::subscribe_on_deactivated( deactivate_control_event^ d)
{
	on_deactivated += d;
}

void editor_control_base::subscribe_on_property_changed(property_changed^ d)
{
	on_property_changed	+= d;
}

void editor_control_base::change_property( System::String^ prop_name, float const dx, float const dy )
{
	float const value	= (math::abs(dx) > math::abs(dy)) ? dx : dy;

	if(prop_name=="sensitivity")
	{
		sensitivity	+= value;
		sensitivity	= math::clamp_r(sensitivity, 0.1f, 20.0f);
	}
	on_property_changed(this);
}

RegistryKey^ get_sub_key( RegistryKey^ root, System::String^ name );
void editor_control_base::load_settings( RegistryKey^ key )
{
	RegistryKey^ self_key	= get_sub_key(key, name);
	sensitivity				= convert_to_single(self_key->GetValue("sensitivity", "1.0") );
	self_key->Close			( );
}

void editor_control_base::save_settings( RegistryKey^ key )
{
	RegistryKey^ self_key	= get_sub_key(key, name);
	self_key->SetValue		( "sensitivity", convert_to_string(sensitivity) );
	self_key->Close			( );
}

void editor_control_base::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	XRAY_UNREFERENCED_PARAMETERS		( mode, button, coll, pos );
	set_mouse_sensivity			( sensitivity );
}

void editor_control_base::end_input( )
{
	set_mouse_sensivity(1.0f);
}


} // editor
} // xray
