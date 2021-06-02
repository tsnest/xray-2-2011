////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_NODE_PROPERTY_HOLDERS_H_INCLUDED
#define PARTICLE_NODE_PROPERTY_HOLDERS_H_INCLUDED

//#include "particle_graph_node.h"
#include "property_container.h"

using namespace System;

namespace xray {
namespace editor {

ref class particle_graph_node;


public delegate void particle_property_changed(particle_graph_node^ node);

public ref class particle_node_time_parameters_holder {
public:
	particle_node_time_parameters_holder(particle_graph_node^ owner, configs::lua_config_value const& prop_config)
	:m_owner(owner)
	{
		m_config = NEW(configs::lua_config_value) (prop_config);
		m_length_time_pos = start_time + length_time;		
	}
	!particle_node_time_parameters_holder()
	{
		DELETE(m_config);
	}
	~particle_node_time_parameters_holder()
	{
		this->!particle_node_time_parameters_holder();
	}

private:

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |

#pragma endregion

#pragma region | Fields |
private:
	float						m_start_time;
	float						m_length_time;
	float						m_length_time_pos;
	particle_graph_node^		m_owner;
	configs::lua_config_value*	m_config;
#pragma endregion

#pragma region | Methods |
public:
	void					set_start_time_impl(float value);
	float					get_start_time_impl();

	void					set_length_time_impl(float value);
	float					get_length_time_impl();

	void					set_start_length_offsets(float offset);

#pragma endregion

#pragma region | Properties |
public:
	property float			start_time{
		float				get(){return get_start_time_impl();}
		void				set(float value){set_start_time_impl(value);}
	}
	property float			length_time{
		float				get(){return get_length_time_impl();}
		void				set(float value){set_length_time_impl(value);}
	}

#pragma endregion

#pragma region | Events |
public:
	event particle_property_changed^ on_property_changed;

#pragma endregion

}; // class particle_node_time_parameters_holder

public ref class particle_float_curve_data_source_holder{
public:
	particle_float_curve_data_source_holder(xray::editor::property_holder_ptr holder,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, float def_value);
	
private:
	!particle_float_curve_data_source_holder(){
		DELETE(m_holder);
		DELETE(m_config);
	}
	~particle_float_curve_data_source_holder(){
		this->!particle_float_curve_data_source_holder();
	}

private:
	particle_graph_node^	m_parent_node;
	xray::editor::property_holder_ptr*		m_holder;
	configs::lua_config_value* m_config;
	String^					m_property_name;
	String^					m_property_description;
	String^					m_property_category;
	String^					m_selected_source;
	const float&			m_default_value;
	float					m_last_float_value;

public:
	String^					get_data_source_name(){return m_selected_source;}		
	void					set_data_source_name(String^ name){set_data_source_impl(name);}

private:
	void					set_data_source_impl(String^ name);


};// class particle_curve_data_source_holder

public ref class particle_color_curve_data_source_holder{
public:
	particle_color_curve_data_source_holder(xray::editor::property_holder_ptr holder,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, u32 def_value);
private:
	!particle_color_curve_data_source_holder(){
		DELETE(m_holder);
		DELETE(m_config);
	}
	~particle_color_curve_data_source_holder(){
		this->!particle_color_curve_data_source_holder();
	}
private:
	particle_graph_node^					m_parent_node;
	xray::editor::property_holder_ptr*		m_holder;
	configs::lua_config_value*				m_config;
	String^									m_property_name;
	String^									m_property_description;
	String^									m_property_category;
	String^									m_selected_source;
	const u32&								m_default_value;
	u32										m_last_color_value;

public:
	String^					get_data_source_name(){return m_selected_source;}		
	void					set_data_source_name(String^ name){set_data_source_impl(name);}

private:
	void					set_data_source_impl(String^ name);


};// class particle_color_curve_data_source_holder

public ref class particle_object_list_holder{
public: 
	particle_object_list_holder(particle_graph_node^ parent_node, configs::lua_config_value const& properties_config, configs::lua_config_value const& defaults_config, String^ prop_name, String^ prop_desc, String^ prop_category)
	:m_parent_node(parent_node),
	m_property_name(prop_name),
	m_prop_desc(prop_desc),
	m_prop_category(prop_category)
	{
		m_properties_config		= MT_NEW(configs::lua_config_value)(properties_config);
		m_defaults_config	= MT_NEW(configs::lua_config_value)(defaults_config);
		
	};
	!particle_object_list_holder()
	{
		MT_DELETE(m_properties_config);
		MT_DELETE(m_defaults_config);
	}
	~particle_object_list_holder()
	{
		this->!particle_object_list_holder();
	}


private:
	configs::lua_config_value*	m_properties_config;
	configs::lua_config_value*	m_defaults_config;
	String^						m_selected_value;
	particle_graph_node^		m_parent_node;
	String^						m_property_name;
	String^						m_prop_desc;
	String^						m_prop_category;


public:
	void						set_value(String^ value);
	String^						get_value(){return m_selected_value;};
};// class particle_object_list_holder

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_NODE_PROPERTY_HOLDERS_H_INCLUDED