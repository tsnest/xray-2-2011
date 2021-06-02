////////////////////////////////////////////////////////////////////////////
//	Created		: 05.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_environment_settings.h"
#include "object_environment_settings_key.h"

namespace xray{
namespace editor{

////////////////////////////////////////			I N I T I A L I Z E 			////////////////////////////////////////////////

object_environment_settings::object_environment_settings ( tool_base^ tool ):
	super( tool )
{
	keys		= gcnew List<key^>( );
	keys->Add	( gcnew key( this ) );
}
object_environment_settings::~object_environment_settings ( )
{

}

////////////////////////////////////			P U B L I C   M E T H O D S 			////////////////////////////////////////////

void object_environment_settings::save ( configs::lua_config_value t )
{
	super::save ( t );
}

void object_environment_settings::load_props ( configs::lua_config_value const& t )
{
	super::load_props( t );
}

void object_environment_settings::load_contents ( )
{
	
}

void object_environment_settings::unload_contents ( bool )
{
	
}

property_container^	object_environment_settings::get_property_container ( )
{
	wpf_controls::property_container^ cont = super::get_property_container( );
	cont->properties->remove( "general/scale" );
	cont->properties->remove( "general/rotation" );

	return cont;
}

} // namespace editor
} // namespace xray