////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_source_float.h"
#include "particle_data_source_float3.h"

namespace xray {
namespace particle_editor {


particle_data_source_float3::particle_data_source_float3( System::String^ source_name, configs::lua_config_value source_config )
{
	this->name = source_name;

	m_curves = gcnew System::Collections::Generic::List<editor::wpf_controls::float_curve^>();

	particle_data_source_base^ data_source = gcnew particle_data_source_float ( "", source_config[ "source0"  ] );	
	((editor::wpf_controls::float_curve^)data_source->data)->name = "X";
	m_curves->Add((editor::wpf_controls::float_curve^)data_source->data );
	
	data_source->data_source_data_changed += gcnew on_data_source_data_changed( this , &particle_data_source_float3::on_data_changed );

	data_source = gcnew particle_data_source_float ( "", source_config[ "source1" ] );	
	((editor::wpf_controls::float_curve^)data_source->data)->name = "Y";
	m_curves->Add((editor::wpf_controls::float_curve^)data_source->data );

	data_source->data_source_data_changed += gcnew on_data_source_data_changed( this , &particle_data_source_float3::on_data_changed );
	
	data_source = gcnew particle_data_source_float ( "", source_config[  "source2"  ] );	
	((editor::wpf_controls::float_curve^)data_source->data)->name = "Z";
	m_curves->Add((editor::wpf_controls::float_curve^)data_source->data );

	data_source->data_source_data_changed += gcnew on_data_source_data_changed( this , &particle_data_source_float3::on_data_changed );
			
	
}


} // namespace particle_editor
} // namespace xray