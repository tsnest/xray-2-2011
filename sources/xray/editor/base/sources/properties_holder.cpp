////////////////////////////////////////////////////////////////////////////
//	Created		: 12.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "properties_holder.h"
#include "property_value_editable_collection.h"
#include "property_value_lists_dictionary.h"
#include "property_value_bool.h"
#include "property_value_float.h"
#include "property_value_integer.h"
#include "property_value_float3.h"
#include "property_value_string.h"
#include "property_value_color.h"


using namespace System;
using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor_base {


properties_holder::properties_holder( configs::lua_config_value const& properties_config, System::String^ library_full_path ){
	
	m_property_container			= gcnew wpf_property_container();
	m_properties_config				= CRT_NEW(xray::configs::lua_config_value)( properties_config );
	
	m_library_full_path				= library_full_path;
}

configs::lua_config_value create_changes_config_from_string_path(String^ path, configs::lua_config_value config){
	if (path == "")
		return config;

	String^ field  = path->Substring(0, path->IndexOf("/"));
	configs::lua_config_value new_config = config[unmanaged_string(field).c_str()].create_table();
	String^ new_path = path->Remove(0, path->IndexOf("/") + 1);
	return create_changes_config_from_string_path(new_path, new_config);
}

static void copy_table_struct(configs::lua_config_value to_config, configs::lua_config_value from_config, String^ supertable_path, String^ library_full_path)
{
	configs::lua_config_value::iterator from_config_begin	= from_config.begin( );
	configs::lua_config_value::iterator from_config_end		= from_config.end( );
	for (;from_config_begin != from_config_end;++from_config_begin){
		if ( from_config_begin.key( ).get_type( ) != configs::t_string )
			continue;
		pcstr begin_key = ( pcstr )from_config_begin.key( );
		if ( ( *from_config_begin ).get_type( ) == configs::t_table_indexed || ( *from_config_begin ).get_type( ) == configs::t_table_named ){
			String^ new_supertable_path = supertable_path + "." + gcnew String( begin_key ) ;
			to_config[ begin_key ].create_table( );

			copy_table_struct( to_config[ begin_key ], *from_config_begin, new_supertable_path, library_full_path);
		}
	}	

	to_config.add_super_table( unmanaged_string( library_full_path ).c_str( ), unmanaged_string( supertable_path ).c_str() );	
}

bool properties_holder::load_property	( wpf_property_container_base^ container,
							 configs::lua_config_value* object_properties_config,
							 configs::lua_config_value* property_descriptor_value,
							 configs::lua_config_value const& defaults_config,
							 System::String^ const property_path )
{
	String^ prop_type	= gcnew String((*property_descriptor_value)["type"]);		
	String^ prop_name	= gcnew String((*property_descriptor_value)["name"]);

	if (dynamic_cast<wpf_property_container_collection^>(container) != nullptr)
	{
		prop_name = String::Format(dynamic_cast<wpf_property_container_collection^>(container)->item_name_template,container->properties->count); 
		if (!(*object_properties_config).value_exists( unmanaged_string(prop_name).c_str()))
			(*object_properties_config)[ unmanaged_string(prop_name).c_str()].assign_lua_value((*property_descriptor_value)["initialize_config"].copy());			
	}

	String^ prop_desc	= gcnew String((*property_descriptor_value)["description"]);
	String^ prop_category = gcnew String((*property_descriptor_value)["category"]);
	bool prop_readonly	= (bool)((*property_descriptor_value)["readonly"]);

	if (prop_type == "string"){
		String^ prop_value = gcnew String((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

		add_string		( container, prop_name, prop_category, prop_desc, prop_value, *object_properties_config, prop_readonly, property_path);   
	}
	else if (prop_type == "holder"){
		wpf_property_container_base^ prop_container = nullptr;

		String^ new_prop_path =	property_path + prop_name + "/"  ;

		if ( (*property_descriptor_value).value_exists("special") )
			if (gcnew String((*property_descriptor_value)["special"]) == "collection"){

				prop_container = gcnew wpf_property_container_collection( gcnew String((*property_descriptor_value)["items_name_template"]));

				property_value_editable_collection^ collection_holder = gcnew property_value_editable_collection( this, prop_container, (*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()], (*property_descriptor_value)["children_properties"] , (defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()] , new_prop_path);
				
				property_descriptor^ descr = gcnew property_descriptor(prop_name, prop_category,prop_desc, prop_container);

				descr->is_read_only = prop_readonly;
				descr->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
				descr->dynamic_attributes->add(gcnew xray::editor::wpf_controls::property_editors::attributes::collection_attribute(gcnew System::Func<Boolean>(collection_holder, &property_value_editable_collection::add_property), 
					gcnew Func<Object^, Boolean>(collection_holder, &property_value_editable_collection::remove_property),
					gcnew Func<Int32, String^, Boolean>(collection_holder, &property_value_editable_collection::move_property)));

				container->properties->add(descr);

				collection_holder->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
			}
			else{
				prop_container = gcnew wpf_property_container();

				fill_properties_table( prop_container, (*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()], (*property_descriptor_value)["children_properties"] , (defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()], new_prop_path );

				property_descriptor^ descr = gcnew property_descriptor(prop_name, prop_category,prop_desc, prop_container);

				descr->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
				descr->is_read_only = prop_readonly;
				descr->dynamic_attributes->add(gcnew xray::editor::wpf_controls::property_editors::attributes::special_attribute(gcnew String((*property_descriptor_value)["special"])));

				container->properties->add(descr);
			}   					
		else{
			prop_container = gcnew wpf_property_container();

			fill_properties_table( prop_container, (*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()], (*property_descriptor_value)["children_properties"] , (defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()], new_prop_path );

			property_descriptor^ descr = gcnew property_descriptor(prop_name, prop_category,prop_desc, prop_container);
			descr->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));
			descr->is_read_only = prop_readonly;

			container->properties->add(descr);
		}
	}
	else if (prop_type == "file"){
		String^ prop_value = gcnew String((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		String^ default_extension = gcnew String((*property_descriptor_value)["default_extension"]);
		String^ default_folder = gcnew String((*property_descriptor_value)["default_folder"]);
		String^ default_mask = gcnew String((*property_descriptor_value)["default_mask"]);
		String^ default_caption = gcnew String((*property_descriptor_value)["default_caption"]);

		add_string_file		( container, prop_name, prop_category, prop_desc, prop_value, *object_properties_config,
			default_extension,
			default_mask,
			default_folder,
			default_caption,
			prop_readonly, property_path);   
	}
	else if (prop_type == "bool"){
		bool prop_value = (bool)((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		add_bool		(container,prop_name, prop_category, prop_desc, prop_value, *object_properties_config, prop_readonly, property_path);
	}
	else if (prop_type == "color"){
		color prop_value = color((u32)((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]));
		Windows::Media::Color default_color = Windows::Media::Color::FromScRgb(1.0f, prop_value.get_Rf(), prop_value.get_Gf(), prop_value.get_Bf());
		add_color		(container,prop_name, prop_category, prop_desc, default_color, *object_properties_config, prop_readonly, property_path);      		
	}
	else if (prop_type == "float"){
		//*object_properties_config.save_as("f:/node_config");
		float prop_value = (float)((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		add_float		(container,prop_name, prop_category, prop_desc, prop_value, *object_properties_config, prop_readonly, property_path);   
	}
	else if (prop_type == "int"){
		int prop_value = (int)((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		add_integer		(container,prop_name, prop_category, prop_desc, prop_value, *object_properties_config, prop_readonly, property_path);   
	}		
	else if (prop_type == "float3"){
		float3 prop_value = (float3)((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);
		add_float3		(container,prop_name, prop_category, prop_desc, prop_value, *object_properties_config, prop_readonly, property_path);   
	}
	else if (prop_type == "list"){
		String^ prop_value = gcnew String((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]);

		configs::lua_config_value values_table = (*property_descriptor_value)["values_list"];

		Collections::ArrayList^ values = gcnew Collections::ArrayList();

		configs::lua_config_value::iterator values_begin		= values_table.begin();
		configs::lua_config_value::iterator values_end			= values_table.end();

		for(;values_begin != values_end;++values_begin){
			values->Add(gcnew String(*values_begin));
		}
		add_strings_list		(container, prop_name, prop_category, prop_desc, prop_value, *object_properties_config, values, prop_readonly, property_path);   

	} 
	else if (prop_type == "object_list"){   
		configs::lua_config_value values_table = (*property_descriptor_value)["values_list"];


		property_value_lists_dictionary^  obj_list_holder = gcnew property_value_lists_dictionary( this, container, (*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()],(defaults_config)[(pcstr)unmanaged_string(prop_name).c_str()],  prop_name, prop_desc, prop_name+" Settings", property_path + prop_name + "/");

		obj_list_holder->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );

		Collections::ArrayList^ values = gcnew Collections::ArrayList();

		configs::lua_config_value::iterator values_begin		= values_table.begin();
		configs::lua_config_value::iterator values_end			= values_table.end();

		for(;values_begin != values_end;++values_begin){
			values->Add(gcnew String(*values_begin));
		}

		property_descriptor^ prop_descriptor = container->properties->add( prop_name, prop_category, prop_desc, gcnew String(*values_table.begin()), obj_list_holder );
		prop_descriptor->is_read_only = prop_readonly;

		prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

		prop_descriptor->dynamic_attributes->add(gcnew xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute(values));
		obj_list_holder->set_value(gcnew String((*object_properties_config)[(pcstr)unmanaged_string(prop_name).c_str()]["selected_value"]));	
	}
	else
	{
		return false;
	}
	return true;
}

void properties_holder::fill_properties_table( wpf_property_container_base^ container,
	configs::lua_config_value object_properties_config,
	configs::lua_config_value const& properties_config,
	configs::lua_config_value const& defaults_config,
	String^ const property_path)
{
	configs::lua_config_value::iterator props_begin		= properties_config.begin();
	configs::lua_config_value::iterator props_end		= properties_config.end();

	for(;props_begin != props_end;++props_begin){
		configs::lua_config_value prop_descr_value = *props_begin;
		load_property(container, &object_properties_config, &prop_descr_value, defaults_config, property_path);		
	};
}

void properties_holder::load_properties( configs::lua_config_value* config, System::String^ properties_path ){
	
// 	config->save_as("d:/on_loading");
// 	m_properties_config->save_as("d:/on_loading_prop");

	if (!m_properties_config->has_super_tables())
		copy_table_struct(*m_properties_config, (*config)["default_values"], properties_path + ".default_values", m_library_full_path);

	fill_properties_table( m_property_container, *m_properties_config, (*config)["properties_config"] , (*config)["default_values"], "");	
};

void properties_holder::add_bool		(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	bool default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	property_value_bool^ prop_value = gcnew property_value_bool( prop_name, properties_config, property_path);

	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;

	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_float	(wpf_property_container_base^ container, 
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	float default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	property_value_float^ prop_value = gcnew property_value_float( prop_name, properties_config, property_path);
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;

	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_integer	(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	int default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	property_value_integer^ prop_value = gcnew property_value_integer( prop_name, properties_config, property_path);
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;
	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_float3	(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	float3 default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	Windows::Media::Media3D::Vector3D def_val = Windows::Media::Media3D::Vector3D(double(default_value.x), double(default_value.y), double(default_value.z));
	property_value_float3^ prop_value = gcnew property_value_float3( prop_name, properties_config, property_path);
	
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, def_val, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;

	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_string	(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	String^ default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	property_value_string^ prop_value = gcnew property_value_string( prop_name, properties_config, property_path);
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;
	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_color	(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	Windows::Media::Color default_value,
	configs::lua_config_value properties_config, 
	bool prop_readonly, 
	String^ property_path)
{
	property_value_color^ prop_value = gcnew property_value_color( prop_name, properties_config, property_path);
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;
	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_strings_list	(wpf_property_container_base^ container, 
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	String^ default_value,
	configs::lua_config_value properties_config,
	Collections::ArrayList^ values,
	bool prop_readonly, 
	String^ property_path)
{
	property_value_string^ prop_value = gcnew property_value_string( prop_name, properties_config, property_path);
	property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
	prop_descriptor->is_read_only = prop_readonly;
	prop_descriptor->dynamic_attributes->add(gcnew xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute(values));
	
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

	container->properties->add( prop_descriptor );

	prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );
}

void properties_holder::add_string_file	(wpf_property_container_base^ container,
	String^ prop_name, 
	String^ prop_category,
	String^ prop_desc,
	String^ default_value,
	configs::lua_config_value properties_config,
	String^ default_extension,
	String^ default_mask,
	String^ default_folder,
	String^ default_caption,
	bool prop_readonly, 
	String^ property_path)
{
		property_value_string^ prop_value = gcnew property_value_string( prop_name, properties_config, property_path);

		property_descriptor^ prop_descriptor = gcnew property_descriptor( prop_name, prop_category, prop_desc, default_value, prop_value  );
		prop_descriptor->is_read_only = prop_readonly;

		prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));

		using xray::editor::wpf_controls::property_editors::attributes::string_select_file_editor_attribute;

		prop_descriptor->dynamic_attributes->add(gcnew string_select_file_editor_attribute(		
																							default_extension,
																							default_mask,
																							default_folder,
																							default_caption
																							)
												);	

		container->properties->add( prop_descriptor );

		prop_value->on_property_value_changed += gcnew on_property_value_changed( this, &properties_holder::on_property_changed );

}



void properties_holder::on_property_changed( String^ property_full_path, configs::lua_config_value const& property_config_value )
{
	configs::lua_config_value changes_config = configs::create_lua_config()->get_root();

	configs::lua_config_value changes_config_value = create_changes_config_from_string_path( property_full_path,  changes_config );

	changes_config_value.assign_lua_value(property_config_value);


//	changes_config.save_as("d:/changes_config");

	process_outgoing_changes( &changes_config );

}

void properties_holder::process_outgoing_changes( configs::lua_config_value* )
{

}
	
} // namespace editor_base
} // namespace xray

