////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_FLOAT_H_INCLUDED
#define PARTICLE_DATA_SOURCE_FLOAT_H_INCLUDED

#include "particle_data_source_base.h"

namespace xray {
namespace particle_editor {

public ref class particle_data_source_float : public particle_data_source_base {
public:
	particle_data_source_float( )
	{
		name = "float_curve";

	}
	particle_data_source_float( System::String^ source_name, configs::lua_config_value source_config );
	
	[System::ComponentModel::DisplayNameAttribute("Data")]
	virtual property System::Object^ data
	{
		System::Object^ get( ) { return m_curve; }
	}
	virtual property System::String^ name;

	virtual property on_data_source_data_changed^	data_source_data_changed;

	
private:
	xray::editor::wpf_controls::float_curve^	m_curve;
	configs::lua_config_value*										m_source_config;
	

private:
	void on_curve_changed	( System::Object^, System::EventArgs^ );
}; // class particle_data_source_float


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_FLOAT_H_INCLUDED