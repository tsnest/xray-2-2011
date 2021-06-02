////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_source_base.h"
#include "particle_data_source_color.h"



namespace xray {
namespace particle_editor {

particle_data_source_color::particle_data_source_color( System::String^ source_name, configs::lua_config_value source_config )
{	
	this->name = source_name;

	m_source_config = CRT_NEW( configs::lua_config_value )( source_config );

	m_curve_container = editor_base::curves_serializer::load_color_curve_from_config ( source_config[ "data" ][ "ramp" ] );

	m_curve_container->edit_completed += gcnew System::EventHandler(this, &particle_data_source_color::on_curve_changed);;	
}

void particle_data_source_color::on_curve_changed	( System::Object^ sender, System::EventArgs^ )
{
	configs::lua_config_value color_ramp_config = (*m_source_config)["data"]["ramp"];
	editor_base::curves_serializer::synchronize_color_curve_config(	safe_cast<editor::wpf_controls::color_curve^>(sender),
																color_ramp_config );

	configs::lua_config_value* changes_config = CRT_NEW( configs::lua_config_value ) ( configs::create_lua_config()->get_root() );
	
	((*changes_config)[(*m_source_config).get_field_id()]["data"]["ramp"]).assign_lua_value( color_ramp_config.copy( ) );

	if ( data_source_data_changed != nullptr )
		data_source_data_changed( *changes_config );
}


} // namespace particle_editor
} // namespace xray