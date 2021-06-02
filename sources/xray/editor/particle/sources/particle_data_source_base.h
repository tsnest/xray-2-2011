////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCE_BASE_H_INCLUDED
#define PARTICLE_DATA_SOURCE_BASE_H_INCLUDED

using namespace xray::editor::wpf_controls;

namespace xray {
namespace particle_editor {

public delegate void on_data_source_data_changed( configs::lua_config_value changed_data );

public interface class particle_data_source_base {
public:
	
	//virtual void load( configs::lua_config_value config );	
	[System::ComponentModel::DisplayNameAttribute("Data")]
	virtual property System::Object^ data
	{
		System::Object^ get();
	}

	virtual property System::String^ name;
	
	virtual property on_data_source_data_changed^	data_source_data_changed;	

}; // class particle_data_source_base

} // namespace particle_
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCE_BASE_H_INCLUDED