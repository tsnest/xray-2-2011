////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_source_color_matrix.h"

namespace xray {
namespace particle_editor {

	particle_data_source_color_matrix::particle_data_source_color_matrix(System::String^ source_name,configs::lua_config_value source_config)
	{
		this->name = source_name;

		m_source_config = CRT_NEW( configs::lua_config_value )( source_config );

		m_color_matrix_container = editor_base::curves_serializer::load_color_matrix_from_config( source_config[ "data" ] );

		m_color_matrix_container->edit_complete += gcnew System::EventHandler( this, &particle_data_source_color_matrix::on_matrix_changed );
	}

	void particle_data_source_color_matrix::on_matrix_changed	( System::Object^ sender, System::EventArgs^ )
	{
		configs::lua_config_value matrix_config = (*m_source_config)["data"];
		editor_base::curves_serializer::synchronize_color_matrix_config( safe_cast<editor::wpf_controls::color_matrix^>( sender ), matrix_config );

		configs::lua_config_value* changes_config = CRT_NEW( configs::lua_config_value ) ( configs::create_lua_config()->get_root() );

		((*changes_config)[(*m_source_config).get_field_id()]["data"]).assign_lua_value( matrix_config.copy( ) );
		
		if ( data_source_data_changed != nullptr )
			data_source_data_changed( *changes_config );
	}

} // namespace particle_editor
} // namespace xray
