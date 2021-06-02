////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_NODE_TYPE_H_INCLUDED
#define PARTICLE_NODE_TYPE_H_INCLUDED

using namespace System::ComponentModel;

namespace xray {	
namespace particle_editor {

typedef xray::editor::wpf_controls::property_container wpf_property_container;
typedef xray::editor::wpf_controls::property_container_base wpf_property_container_base;

ref class particle_time_parameters_holder;
	
public ref class particle_node_properties : xray::editor_base::properties_holder
{
	
public:
	particle_node_properties(particle_graph_node^ owner);	
					
public:	

	property particle_graph_node^	m_particle_node;

	property particle_time_parameters_holder^				time_parameteters_holder
	{
		particle_time_parameters_holder^					get(){return m_time_parameters_holder;}
	}


private:
	particle_time_parameters_holder^			m_time_parameters_holder;	

public:
	virtual void	load_properties	( configs::lua_config_value* config, System::String^ properties_path ) override;
	virtual bool	load_property	(	wpf_property_container_base^ container,
										configs::lua_config_value*			object_properties_config,
										configs::lua_config_value*			property_descriptor_value,
										configs::lua_config_value const&	property_default_value,
										System::String^ const property_path ) override;

	virtual void process_outgoing_changes	( configs::lua_config_value* ) override;
	

}; // class particle_node_properties
}; //namespace particle_editor
}; //namespace xray

#endif // #ifndef PARTICLE_NODE_TYPE_H_INCLUDED