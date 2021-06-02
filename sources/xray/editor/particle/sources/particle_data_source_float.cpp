////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_source_float.h"

using namespace xray::editor::wpf_controls;

namespace xray {
	namespace particle_editor {

particle_data_source_float::particle_data_source_float( System::String^ source_name, configs::lua_config_value source_config )
{
	
	this->name = source_name;

	m_source_config = CRT_NEW( configs::lua_config_value )( source_config );

	System::Windows::Media::Color color = System::Windows::Media::Colors::Black;

	if (source_config.value_exists("color")){

		System::String^ str_color = gcnew System::String( source_config["color"] );

		if ( str_color == "red" )
		{
			color = System::Windows::Media::Colors::Red;
		}
		else if ( str_color == "green" )
		{
			color = System::Windows::Media::Colors::Green;
		}
		else if ( str_color == "blue" )
		{
			color = System::Windows::Media::Colors::Blue;
		}
	}

	if ( ( *m_source_config )["data"].value_exists("curve1") )
	{
		( *m_source_config )["data"]["curve0"].assign_lua_value(( *m_source_config )["data"]["curve1"].copy( ) );
		( *m_source_config )["data"].erase("curve1");
	}

	float_curve^ curve0 = editor_base::curves_serializer::load_float_curve_from_config( source_config["data"]["curve0"] );		
	curve0->color = color;
		
	m_curve = curve0;
	curve0->edit_completed += gcnew System::EventHandler( this, &particle_data_source_float::on_curve_changed );		

	return;
}

void particle_data_source_float::on_curve_changed	( System::Object^, System::EventArgs^ )
{
	configs::lua_config_value curve0_config =  (*m_source_config)["data"]["curve0"];
	editor_base::curves_serializer::synchronize_float_curve_config( m_curve, curve0_config );
	
	(*m_source_config)["data"]["is_range"] = m_curve->is_range;

	configs::lua_config_value* changes_config = CRT_NEW(configs::lua_config_value) (configs::create_lua_config()->get_root());
	
	((*changes_config)[(*m_source_config).get_field_id()]["data"]["curve0"]).assign_lua_value(
		curve0_config.copy() 
	);
	
	if ( data_source_data_changed != nullptr )
		data_source_data_changed( *changes_config );
}


} // namespace particle_editor
} // namespace xray