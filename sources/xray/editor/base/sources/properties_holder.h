////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTIES_HOLDER_H_INCLUDED
#define PROPERTIES_HOLDER_H_INCLUDED

typedef xray::editor::wpf_controls::property_container wpf_property_container;
typedef xray::editor::wpf_controls::property_container_collection wpf_property_container_collection;
typedef xray::editor::wpf_controls::property_container_base wpf_property_container_base;

namespace xray {
namespace editor_base {

public ref class properties_holder
{

public:
	
	properties_holder( configs::lua_config_value const& properties_config, System::String^ library_full_path );	

	!properties_holder()
	{
		CRT_DELETE						(m_properties_config);					
	}
	~properties_holder()
	{
		this->!properties_holder();
	}


public:
	
	property System::String^					type;
	property System::String^					name;

	property configs::lua_config_value* config 
	{
		configs::lua_config_value*	get(){return m_properties_config;}				
	}
	property wpf_property_container^	container
	{
		wpf_property_container^			get(){return m_property_container;}
	}

			
private:
	wpf_property_container^						m_property_container;
	configs::lua_config_value*					m_properties_config;
	System::String^								m_library_full_path;

public:
	virtual void		load_properties	( configs::lua_config_value* config, System::String^ properties_path );
	virtual bool		load_property	( wpf_property_container_base^ container,
		configs::lua_config_value*			object_properties_config,
		configs::lua_config_value*			property_descriptor_value,
		configs::lua_config_value const&	property_default_value,
		System::String^ const property_path );

internal:

	void				fill_properties_table( wpf_property_container_base^ container,
		configs::lua_config_value object_properties_config,
		configs::lua_config_value const& properties_config,
		configs::lua_config_value const& defaults_config,
		System::String^ const property_path);

	void				add_bool	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		bool default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_float	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		float default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_integer	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		int default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_float3	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		float3 default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_string	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		System::String^ default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_color	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		System::Windows::Media::Color default_value,
		configs::lua_config_value properties_config, 
		bool prop_readonly, 
		System::String^ property_path);

	void				add_strings_list(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		System::String^ default_value,
		configs::lua_config_value properties_config,
		System::Collections::ArrayList^ values,
		bool prop_readonly, 
		System::String^ property_path);

	void				add_string_file	(wpf_property_container_base^ container,
		System::String^ prop_name, 
		System::String^ prop_category,
		System::String^ prop_desc,
		System::String^ default_value,
		configs::lua_config_value properties_config,
		System::String^ default_extension,
		System::String^ default_mask,
		System::String^ default_folder,
		System::String^ default_caption,
		bool prop_readonly, 
		System::String^ property_path);

	

protected:
	virtual void process_outgoing_changes( configs::lua_config_value* config );

public:
	void on_property_changed( System::String^ property_full_path, configs::lua_config_value const& property_config_value );

}; // class particle_node_type
}; //namespace editor_base
}; //namespace xray


#endif // #ifndef PROPERTIES_HOLDER_H_INCLUDED