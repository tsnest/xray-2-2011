////////////////////////////////////////////////////////////////////////////
//	Created		: 02.09.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_COLOR_PROPERTY_HOLDER_H_INCLUDED
#define PARTICLE_COLOR_PROPERTY_HOLDER_H_INCLUDED

using namespace System;

#include "property_value_base.h"

namespace xray {
namespace editor_base {

ref class property_value_color : xray::editor::wpf_controls::i_property_value , property_value_base {
public:

	property_value_color( String^ name, configs::lua_config_value const& config, String^ property_parent_path){
		m_name = name;
		m_property_path = property_parent_path + m_name + "/";
		m_config = CRT_NEW(configs::lua_config_value) (config);
	}

	!property_value_color()
	{ 
		CRT_DELETE( m_config );
	}

	~property_value_color()
	{
		this->!property_value_color();
	}
private:
	String^						m_name;
	configs::lua_config_value*	m_config;
	String^						m_property_path;
public:
	virtual void			set_value		(System::Object^ value){
		
		System::Windows::Media::Color clr = System::Windows::Media::Color(value);
		
		(*m_config)[(pcstr)unmanaged_string(m_name).c_str()] = Drawing::Color::FromArgb(255,int(clr.R),int(clr.G),int(clr.B)).ToArgb();

		on_property_value_changed( m_property_path, (*m_config)[unmanaged_string(m_name).c_str()] );

	}
	
	virtual System::Object	^get_value		(){
		u32 val = (*m_config)[(pcstr)unmanaged_string(m_name).c_str()];
		math::color clr( val );
		return System::Windows::Media::Color::FromScRgb(1.0f, clr.get_Rf(), clr.get_Gf(), clr.get_Bf());
	}

	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Windows::Media::Color::typeid;}
	}

	virtual property on_property_value_changed^ on_property_value_changed;

}; // class property_value_bool
} // namespace editor_base
} // namespace xray

#endif // #ifndef PARTICLE_COLOR_PROPERTY_HOLDER_H_INCLUDED