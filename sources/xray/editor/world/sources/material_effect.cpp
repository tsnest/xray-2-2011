////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_effect.h"
#include "material_stage.h"
#include "material.h"
#include "material_callback_binders.h"
#include "material_editor.h"
#include "material_property.h"
#include "texture_editor.h"
#include "effect_selector.h"

using xray::editor::wpf_controls::property_property_value;
using xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_event_handler;
using xray::editor::wpf_controls::color_rgb;
typedef System::Windows::Media::Color MColor;

namespace xray {
namespace editor {

material_effect::material_effect( material_stage^ parent, const configs::lua_config_value& config_value )
{
	m_parent_stage			= parent;
	m_config				= NEW(configs::lua_config_value)( config_value );
	name					= gcnew System::String( (*m_config)["effect_id"] );
	m_effect_description_config	= NEW(configs::lua_config_ptr)();
}

material_effect::~material_effect( )
{
	DELETE	( m_config );
	DELETE	( m_effect_description_config );
	for each (material_property_base^ p in m_properties.Values)
		delete p;
}

void material_effect::fail_loading( System::String^ error_message )
{
	System::Windows::Forms::MessageBox::Show	( error_message, "Effect Loading Fail", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error );
	m_properties.Clear							( );
	m_property_name_to_desc_idx.Clear			( );
	m_loaded_callback							( );
}

void material_effect::iterate_effect_properties(configs::lua_config_value config_root, bool& bchanged)
{
	if (config_root.get_type() != xray::configs::t_table_indexed &&
		config_root.get_type() != xray::configs::t_table_named)
		return;
	
	configs::lua_config_value::const_iterator it	= config_root.begin();
	configs::lua_config_value::const_iterator it_e	= config_root.end();
	
	for( ; it!=it_e; ++it )
	{
		configs::lua_config_value current	= *it;
		if (!current.value_exists("shader_parameter_id"))
		{
			iterate_effect_properties(*it, bchanged);
			continue;
		}
		
		pcstr property_id					= current["shader_parameter_id"];

		if( !(bool)current["is_readonly"] )
		{
			R_ASSERT( current.value_exists( "default_value" ), "property must contain default value" );

			System::String^ type_id		= gcnew System::String( (pcstr)current["type"] );
			System::String^	show_id		= gcnew System::String( (pcstr)current["show_id"] );
			bool	is_model_property	= current[ "is_model_parameter_by_default" ];
			
			//if is no such property in stage -> create default
			if( !(*m_config).value_exists( property_id ) )
			{
				(*m_config)[property_id]["value"].assign_lua_value	( current["default_value"].copy() );
				(*m_config)[property_id]["is_model_parameter"]		= is_model_property;
				 bchanged = true;
			}

			material_property_base^						prop;
			
			System::String^ property_name				= gcnew System::String(property_id);
			//m_property_name_to_desc_idx[property_name]	= idx;

			if( type_id == "int" )
			{
				prop	= gcnew material_property<int>( this, property_name, "value", (*m_config)[property_id] );

				System::String^ subtype_id			= ( current.value_exists("subtype") ) ? gcnew System::String( current[ "subtype" ] ) : "";
				
				if( subtype_id == "combo_box" )
				{
					ArrayList^ combo_box_strings				= gcnew ArrayList( );
					configs::lua_config_value combo_box_items	= current["combo_items"];
					int count									= combo_box_items.size( );

					for( int i = 0; i < count; ++i )
						combo_box_strings->Add( gcnew System::String( (pcstr)combo_box_items[i] ) );

				}
			}else if( type_id == "float" )
			{
				prop	= gcnew material_property<float>( this, property_name, "value", (*m_config)[property_id] );
			}else if( type_id == "bool" )
			{
				prop	= gcnew material_property<bool>( this, property_name, "value", (*m_config)[property_id] );
			}else if( type_id == "texture" )
			{
				if( !current.value_exists("subtype") )
				{
					fail_loading	( "Effect file " + ( (m_parent_stage) ? m_parent_stage->name : "" ) + "/" + name + " corrupt. <subtype> parameter in " + gcnew System::String( show_id ) + " texture descriptor is not exists." );
					return;
				}
				System::String^ subtype = gcnew System::String( (pcstr)current["subtype"] );

				prop	= gcnew material_property_converted<System::String^>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_string_read_writer() );

				prop->subtype = subtype;

				if( subtype == "_2d" )
					prop->value_changed += gcnew System::Action<material_property_base^>( this, &material_effect::diffuse_texture_value_chaged );
				
				int group_id = 0;
				if( current.value_exists("group_id") )
					group_id = (int)current["group_id"];

				prop->group_id = group_id;
			}else if( type_id == "color" )
			{
				prop	= gcnew material_property_converted<color_rgb>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_color_rgb_read_writer() );
			}else if( type_id == "float3" )
			{
				prop	= gcnew material_property_converted<Vector3D>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_vector3_read_writer() );
			}else
				continue;
			
			prop->show_id				= show_id;
			m_properties.Add			( prop->name, prop );
		} //!(bool)current["is_readonly"]
	}
}

void material_effect::iterate_effect_dependencies	( configs::lua_config_value config_root )
{
	if (config_root.get_type() != xray::configs::t_table_indexed &&
		config_root.get_type() != xray::configs::t_table_named)
		return;
	
	configs::lua_config_value::const_iterator it	= config_root.begin();
	configs::lua_config_value::const_iterator it_e	= config_root.end();

	for( ; it!=it_e; ++it )
	{
		configs::lua_config_value current	= *it;
		if (!current.value_exists("shader_parameter_id"))
		{
			iterate_effect_dependencies(*it);
			continue;
		}
		System::String^ current_property_id	= gcnew  System::String((pcstr)current["shader_parameter_id"]);
		
		material_property_base^		prop = m_properties[current_property_id];
		//dependencies
		if( current.value_exists( "depends_on" ) )
		{
			configs::lua_config_value::const_iterator cur_dependency	= current["depends_on"].begin();
			configs::lua_config_value::const_iterator end_dependency	= current["depends_on"].end();

			for( ; cur_dependency!=end_dependency; ++cur_dependency )
			{
				System::String^ dependent_property_id	= gcnew  System::String(cur_dependency.key());
				material_property_base^	dep_prop	= m_properties[dependent_property_id];
				dep_prop->dependant_properties->Add	( prop );
				prop->dependency_end_points->Add	( dep_prop, dep_prop->get_value( *cur_dependency ) );

				prop->set_is_visible				( dep_prop->match_to( *cur_dependency ) && dep_prop->visible );
			}
		}
	}
}

void material_effect::effect_properties_loaded( queries_result & result )
{
	if( !result[0].is_successful( ) )
	{
		fail_loading( "Effect file is corrupted and do not loaded!" );
		return;
	}
	
	bool bchanged = false;

	m_property_name_to_desc_idx.Clear		( );
	(*m_effect_description_config)			= static_cast_resource_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());
	
	configs::lua_config_value config_root	= (*m_effect_description_config)->get_root()["properties"];


	/*
	
	configs::lua_config_value::const_iterator it	= config_root.begin();
	configs::lua_config_value::const_iterator it_e	= config_root.end();

	for( u32 idx=0; it!=it_e; ++it,++idx )
	{
		configs::lua_config_value current	= *it;
		pcstr property_id					= current["shader_parameter_id"];
		
		if( !(bool)current["is_readonly"] )
		{
			R_ASSERT( current.value_exists( "default_value" ), "property must contain default value" );

			System::String^ type_id		= gcnew System::String( (pcstr)current["type"] );
			System::String^	show_id		= gcnew System::String( (pcstr)current["show_id"] );
			bool	is_model_property	= current[ "is_model_parameter_by_default" ];
			
			//if is no such property in stage -> create default
			if( !(*m_config).value_exists( property_id ) )
			{
				(*m_config)[property_id]["value"].assign_lua_value	( current["default_value"].copy() );
				(*m_config)[property_id]["is_model_parameter"]		= is_model_property;
				 bchanged = true;
			}

			material_property_base^						prop;
			
			System::String^ property_name				= gcnew System::String(property_id);
			m_property_name_to_desc_idx[property_name]	= idx;

			if( type_id == "int" )
			{
				prop	= gcnew material_property<int>( this, property_name, "value", (*m_config)[property_id] );

				System::String^ subtype_id			= ( current.value_exists("subtype") ) ? gcnew System::String( current[ "subtype" ] ) : "";
				
				if( subtype_id == "combo_box" )
				{
					ArrayList^ combo_box_strings				= gcnew ArrayList( );
					configs::lua_config_value combo_box_items	= current["combo_items"];
					int count									= combo_box_items.size( );

					for( int i = 0; i < count; ++i )
						combo_box_strings->Add( gcnew System::String( (pcstr)combo_box_items[i] ) );

				}
			}else if( type_id == "float" )
			{
				prop	= gcnew material_property<float>( this, property_name, "value", (*m_config)[property_id] );
			}else if( type_id == "bool" )
			{
				prop	= gcnew material_property<bool>( this, property_name, "value", (*m_config)[property_id] );
			}else if( type_id == "texture" )
			{
				if( !current.value_exists("subtype") )
				{
					fail_loading	( "Effect file " + ( (m_parent_stage) ? m_parent_stage->name : "" ) + "/" + name + " corrupt. <subtype> parameter in " + gcnew System::String( show_id ) + " texture descriptor is not exists." );
					return;
				}
				System::String^ subtype = gcnew System::String( (pcstr)current["subtype"] );

				prop	= gcnew material_property_converted<System::String^>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_string_read_writer() );

				prop->subtype = subtype;

				if( subtype == "_2d" )
					prop->value_changed += gcnew System::Action<material_property_base^>( this, &material_effect::diffuse_texture_value_chaged );
				
				int group_id = 0;
				if( current.value_exists("group_id") )
					group_id = (int)current["group_id"];

				prop->group_id = group_id;
			}else if( type_id == "color" )
			{
				prop	= gcnew material_property_converted<MColor>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_color_read_writer() );
			}else if( type_id == "float3" )
			{
				prop	= gcnew material_property_converted<Vector3D>( this, property_name, "value", (*m_config)[property_id], gcnew material_property_vector3_read_writer() );
			}else
				continue;
			
			prop->show_id				= show_id;
			m_properties.Add			( prop->name, prop );
		} //!(bool)current["is_readonly"]
	}//for( u32 idx=0; it!=it_e; ++it,++idx )

	*/
	
	iterate_effect_properties( config_root, bchanged );
		/*	
	it	= config_root.begin();
	for(; it!=it_e; ++it )
	{
		configs::lua_config_value current	= *it;
		System::String^ current_property_id	= gcnew  System::String((pcstr)current["shader_parameter_id"]);

		material_property_base^		prop = m_properties[current_property_id];
		//dependencies
		if( current.value_exists( "depends_on" ) )
		{
			configs::lua_config_value::const_iterator cur_dependency	= current["depends_on"].begin();
			configs::lua_config_value::const_iterator end_dependency	= current["depends_on"].end();

			for( ; cur_dependency!=end_dependency; ++cur_dependency )
			{
				System::String^ dependent_property_id	= gcnew  System::String(cur_dependency.key());
				material_property_base^	dep_prop	= m_properties[dependent_property_id];
				dep_prop->dependant_properties->Add	( prop );
				prop->dependency_end_points->Add	( dep_prop, dep_prop->get_value( *cur_dependency ) );

				prop->set_is_visible				( dep_prop->match_to( *cur_dependency ) && dep_prop->visible );
			}
		}
	}
*/
	iterate_effect_dependencies( config_root );
	
	if(bchanged)
		m_parent_stage->m_parent->properties_changed( );

	check_integrity				( );

	m_loaded_callback			( );
}

void material_effect::effect_selector_launch( wpf_controls::property_editors::property^ prop, Object^ filter )
{
	effect_selector^ selector	= gcnew effect_selector();
	selector->selected_item		= safe_cast<System::String^>(prop->value);
	selector->selecting_stage	= safe_cast<System::String^>(filter);

	if(selector->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		prop->value = selector->selected_item;
		reload( gcnew System::Action( this, &material_effect::effect_reloaded ) );
	}
}

void material_effect::effect_reloaded( )
{
	LOG_WARNING("refresh not implemented");
	//. m_parent_stage->parent->parent_editor->reset_material_in_property_grid( );
}

void material_effect::texture_selector_launch( wpf_controls::property_editors::property^ prop, Object^ filter )
{
	texture_editor_ptr selector( m_parent_stage->m_parent->m_editor_world, filter	);

	selector->selecting_entity = resource_editor::entity::resource_name;
	selector->selected_name = safe_cast<System::String^>(prop->value);

	System::Windows::Forms::DialogResult res = selector->ShowDialog();
	if( res == System::Windows::Forms::DialogResult::OK)
	{
		prop->value = selector->selected_name;
	}
}

void material_effect::diffuse_texture_value_chaged( material_property_base^ prop )
{
	System::String^ diffuse_path		= safe_cast<System::String^>( prop->get_value( ) );
	if( diffuse_path == "" )
		return;

	System::String^ bump_path			= "resources/textures/" + diffuse_path + "_bump.tga";
	fs_new::physical_path_info const &	bump_texture_info	=	resources::get_physical_path_info	
															(fs_new::virtual_path_string(unmanaged_string(bump_path).c_str()), resources::sources_mount);
	System::String^ bump_texture_name	= "";
	
	if(bump_texture_info.is_file())
		bump_texture_name = diffuse_path + "_bump";


	System::String^ nmap_path			= "resources/textures/" + diffuse_path + "_nmap.tga";
	fs_new::physical_path_info const & nmap_texture_info	=	resources::get_physical_path_info
																(fs_new::virtual_path_string(unmanaged_string(nmap_path).c_str()), resources::sources_mount);

	System::String^ nmap_texture_name	= "";

	if(nmap_texture_info.is_file())
		nmap_texture_name				= diffuse_path + "_nmap";


	System::String^ specular_path		= "resources/textures/" + diffuse_path + "_spec.tga";
	fs_new::physical_path_info const &  specular_texture_info	=	resources::get_physical_path_info
																	(fs_new::virtual_path_string(unmanaged_string(specular_path).c_str()), resources::sources_mount);

	System::String^ specular_texture_name	= "";

	if(specular_texture_info.is_file())
		specular_texture_name			= diffuse_path + "_spec";

	u8 pcount = 0;
	for each( material_property_base^ p in m_properties.Values )
	{
		if(p->group_id != prop->group_id)
			continue;

		if( p->subtype == "_bump" )
		{
			p->set_value( bump_texture_name );
			++pcount;
		}

		if( p->subtype == "_nmap" )
		{
			p->set_value( nmap_texture_name );
			++pcount;
		}

		if( p->subtype == "_spec" )
		{
			p->set_value( specular_texture_name );
			++pcount;
		}
	}
}

void material_effect::set_base_texture( System::String^ name )
{
	if(!m_properties.ContainsKey("texture_diffuse"))
		return;

	material_property_base^ p = m_properties["texture_diffuse"];
	p->set_value( name );
}

System::String^ material_effect::get_base_texture( )
{
	if(!m_properties.ContainsKey("texture_diffuse"))
		return nullptr;

	material_property_base^ p = m_properties["texture_diffuse"];
	return p->get_value()->ToString();
}

void material_effect::load( System::Action^ cb )
{
	m_loaded_callback		= cb;

	m_properties.Clear();

	System::String^ stage_name = m_parent_stage->name;
	if( name == "none" )
	{
		m_loaded_callback	( );
		return;
	}

	fs_new::virtual_path_string path	= "resources/effects/";
	path.append				( unmanaged_string( stage_name ).c_str() );
	path.append				( "/" );
	path.append				( unmanaged_string( name ).c_str( ) );
	path.append				( ".effect" );

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &material_effect::effect_properties_loaded ), g_allocator);

	resources::query_resource	(
		path.c_str( ), 
		resources::lua_config_class, 
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
}

void material_effect::reload( System::Action^ cb )
{
	m_loaded_callback		= cb;
	(*m_config).clear		( );
	(*m_config)["effect_id"] = unmanaged_string(name).c_str();
	load					( cb );
}

static void is_property_exists_in_effect(System::String^ property_name, configs::lua_config_value search_config, bool& out_result)
{
	if (search_config.get_type() != xray::configs::t_table_indexed &&
		search_config.get_type() != xray::configs::t_table_named)
		return;
	
	for (configs::lua_config_value::iterator it = search_config.begin(); it != search_config.end(); ++it)
	{
		if ((*it).value_exists("shader_parameter_id"))
		{
			System::String^ tmp = gcnew System::String((pcstr)(*it)["shader_parameter_id"]);
			if (tmp == property_name)
				out_result = true;
		}
		else
		{
			is_property_exists_in_effect(property_name, *it, out_result);
		}
	}
}

static void find_effect_property_impl(System::String^ property_name, configs::lua_config_value search_config, configs::lua_config_value& out_property_value)
{
	if (search_config.get_type() != xray::configs::t_table_indexed &&
		search_config.get_type() != xray::configs::t_table_named)
		return;
	
	for (configs::lua_config_value::iterator it = search_config.begin(); it != search_config.end(); ++it)
	{
		if ((*it).value_exists("shader_parameter_id"))
		{
			System::String^ tmp = gcnew System::String((pcstr)(*it)["shader_parameter_id"]);
			if (tmp == property_name)
			{
				out_property_value = *it;
			}
		}
		else
		{
			find_effect_property_impl(property_name, *it, out_property_value);
		}
	}
}

//static configs::lua_config_value find_effect_property(System::String^ property_name, configs::lua_config_value search_config)
//{
//	configs::lua_config_value result = search_config;
//	find_effect_property_impl(property_name, search_config, result);
//	return result;
//}

void material_effect::fill_effect_property( wpf_property_container^ cont, configs::lua_config_value property_cfg, bool material_editor_mode )
{
	System::String^ shader_parameter_id = gcnew System::String(pcstr(property_cfg["shader_parameter_id"]));
	
	for each( material_property_base^ p in m_properties.Values )
	{
		if(!p->visible)
			continue;
		
		bool can_override = (*(p->get_config()))["is_model_parameter"];
		if(!can_override && !material_editor_mode)
			continue;
		
		if (p->name != shader_parameter_id)
			continue;

		property_descriptor^ desc		= gcnew property_descriptor( p->name, p );
		
		desc->display_name				= p->show_id;
		
		//u32 idx									= m_property_name_to_desc_idx[p->name];
		//System::String^ tmp = gcnew System::String((pcstr)effect_cfg["shader_parameter_id"]);
		//R_ASSERT(tmp==p->name);
		
		// TODO:
		//.bool property_found = false;
		
		//is_property_exists_in_effect(p->name, effect_cfg_root, property_found);
		
		//if (!property_found)
		//	continue;
		
		configs::lua_config_value effect_cfg	= property_cfg;//find_effect_property(p->name, effect_cfg_root);//effect_cfg_root[idx];
		
		System::String^ type_id		= gcnew System::String( (pcstr)effect_cfg["type"] );
		
		if( type_id == "int" )
		{
			System::String^ subtype_id	= ( effect_cfg.value_exists("subtype") ) ? gcnew System::String( effect_cfg["subtype"] ) : "";
			
			if( subtype_id == "combo_box" )
			{
				ArrayList^ combo_box_strings				= gcnew ArrayList( );
				configs::lua_config_value combo_box_items	= effect_cfg["combo_items"];
				int count									= combo_box_items.size( );

				for( int i = 0; i < count; ++i )
					combo_box_strings->Add( gcnew System::String( (pcstr)combo_box_items[i] ) );

				desc->dynamic_attributes->add( gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( combo_box_strings ) );
			}
		}else if( type_id == "float" )
		{
			value_range_and_format_attribute^ attribute = gcnew value_range_and_format_attribute( Single::MinValue, Single::MaxValue, 0.01f );
			
			bool need_range				= false;
			if( effect_cfg.value_exists("min_value") )
			{
				need_range				= true;
				attribute->min_value	= (float)effect_cfg["min_value"];
			}
			if( effect_cfg.value_exists("max_value") )
			{
				need_range				= true;
				attribute->max_value	= (float)effect_cfg["max_value"];
			}
			if( effect_cfg.value_exists("change_step") )
			{
				need_range				= true;
				attribute->step_size	= (float)effect_cfg["change_step"];
			}
			if( need_range )
				desc->dynamic_attributes->add( attribute );

		}else if( type_id == "bool" )
		{
		}else if( type_id == "texture" )
		{
			System::String^ subtype = gcnew System::String( (pcstr)effect_cfg["subtype"] );

			desc->dynamic_attributes->add( gcnew external_editor_attribute( gcnew external_editor_event_handler( this, &material_effect::texture_selector_launch ), true, subtype ) );

		}else if( type_id == "color" )
		{
		}else if( type_id == "float3" )
		{
		}else
			continue;

		if(material_editor_mode)
		{
			property_descriptor^ idesc	= gcnew property_descriptor( p->show_id, gcnew lua_property_value_bool( *p->get_config(), "is_model_parameter" ) );
			desc->inner_properties->Add		( "asd", idesc );
		}

		cont->properties->add	( desc );
	}
}

void material_effect::fill_effect_property_grid(wpf_property_container^ cont, configs::lua_config_value config_root, bool material_editor_mode)
{
	if (config_root.get_type() != xray::configs::t_table_indexed &&
		config_root.get_type() != xray::configs::t_table_named)
		return;
	
	configs::lua_config_value::const_iterator it	= config_root.begin();
	configs::lua_config_value::const_iterator it_e	= config_root.end();
	
	for( ; it!=it_e; ++it )
	{
		configs::lua_config_value current	= *it;
		
		if (current.get_type() != xray::configs::t_table_indexed &&
			current.get_type() != xray::configs::t_table_named)
			return;
		
		if (!current.value_exists("shader_parameter_id"))
		{
			wpf_property_container^ inner_container = gcnew wpf_property_container;
			
			fill_effect_property_grid(inner_container, *it, material_editor_mode);
			
			System::String^ category_name = "";
			
			if (current.value_exists("category_name"))
				category_name = gcnew System::String(pcstr(current["category_name"]));
			
			property_descriptor^ desc		= gcnew property_descriptor( category_name, gcnew xray::editor::wpf_controls::object_property_value<xray::editor::wpf_controls::i_property_container^>(inner_container) );
			desc->is_expanded				= false;
			cont->properties->add			( desc );
			
			continue;
		}
		else
		{
			fill_effect_property(cont, *it, material_editor_mode);
		}
	}
}

wpf_property_container^ material_effect::get_property_container( bool material_editor_mode )
{
	wpf_property_container^ cont	= gcnew wpf_property_container;

	property_descriptor^ desc		= nullptr;

	desc							= gcnew property_descriptor( "EffectName", gcnew property_property_value( this, "name" ) );
	desc->dynamic_attributes->add	( gcnew external_editor_attribute( gcnew external_editor_event_handler( this, &material_effect::effect_selector_launch ), false, m_parent_stage->name ) );
	if(!material_editor_mode)
		desc->is_read_only = true;
	
	cont->properties->add			( desc );

	if( m_properties.Count !=0 )
	{
		fill_effect_property_grid(cont, (*m_effect_description_config)->get_root()["properties"], material_editor_mode);
		
		/*
		
		configs::lua_config_value effect_cfg_root = (*m_effect_description_config)->get_root()["properties"];

		for each( material_property_base^ p in m_properties.Values )
		{
			if(!p->visible)
				continue;

			bool can_override = (*(p->get_config()))["is_model_parameter"];
			if(!can_override && !material_editor_mode)
				continue;
			
			desc					= gcnew property_descriptor( p->name, p );
			
			desc->display_name				= p->name;
			
			//u32 idx									= m_property_name_to_desc_idx[p->name];
			//System::String^ tmp = gcnew System::String((pcstr)effect_cfg["shader_parameter_id"]);
			//R_ASSERT(tmp==p->name);
			
			// TODO:
			bool property_found = false;
			
			is_property_exists_in_effect(p->name, effect_cfg_root, property_found);
			
			if (!property_found)
				continue;
			
			configs::lua_config_value effect_cfg	= find_effect_property(p->name, effect_cfg_root);//effect_cfg_root[idx];
			
			System::String^ type_id		= gcnew System::String( (pcstr)effect_cfg["type"] );
			
			if( type_id == "int" )
			{
				System::String^ subtype_id	= ( effect_cfg.value_exists("subtype") ) ? gcnew System::String( effect_cfg["subtype"] ) : "";
				
				if( subtype_id == "combo_box" )
				{
					ArrayList^ combo_box_strings				= gcnew ArrayList( );
					configs::lua_config_value combo_box_items	= effect_cfg["combo_items"];
					int count									= combo_box_items.size( );

					for( int i = 0; i < count; ++i )
						combo_box_strings->Add( gcnew System::String( (pcstr)combo_box_items[i] ) );

					desc->dynamic_attributes->add( gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( combo_box_strings ) );
				}
			}else if( type_id == "float" )
			{
				value_range_and_format_attribute^ attribute = gcnew value_range_and_format_attribute( Single::MinValue, Single::MaxValue, 0.01f );
				
				bool need_range				= false;
				if( effect_cfg.value_exists("min_value") )
				{
					need_range				= true;
					attribute->min_value	= (float)effect_cfg["min_value"];
				}
				if( effect_cfg.value_exists("max_value") )
				{
					need_range				= true;
					attribute->max_value	= (float)effect_cfg["max_value"];
				}
				if( effect_cfg.value_exists("change_step") )
				{
					need_range				= true;
					attribute->step_size	= (float)effect_cfg["change_step"];
				}
				if( need_range )
					desc->dynamic_attributes->add( attribute );

			}else if( type_id == "bool" )
			{
			}else if( type_id == "texture" )
			{
				System::String^ subtype = gcnew System::String( (pcstr)effect_cfg["subtype"] );

				desc->dynamic_attributes->add( gcnew external_editor_attribute( gcnew external_editor_event_handler( this, &material_effect::texture_selector_launch ), true, subtype ) );

			}else if( type_id == "color" )
			{
			}else if( type_id == "float3" )
			{
			}else
				continue;

			if(material_editor_mode)
			{
				System::String^					full_key	= p->show_id;
				
				cli::array<System::String^>^	tokens		= full_key->Split('/');
				
				property_descriptor^	idesc	= nullptr;
				if (tokens->Length == 2)
				{
					System::String^				category	= tokens[0];
					System::String^				key			= tokens[1];
					
					idesc	= gcnew property_descriptor( key, gcnew lua_property_value_bool( *p->get_config(), "is_model_parameter" ) );
					idesc->category					= category;
				}
				else
				{
					idesc	= gcnew property_descriptor( p->show_id, gcnew lua_property_value_bool( *p->get_config(), "is_model_parameter" ) );
				}
				
				desc->inner_properties->Add		( "asd", idesc );
			}

			cont->properties->add	( desc );
		}
		*/
	}// if properties->Count!=0
	return cont;
}

void material_effect::check_integrity( )
{
	bool bchanged = false;
	configs::lua_config_iterator it		= (*m_config).begin();
	configs::lua_config_iterator it_e	= (*m_config).end();
	for(; it!=it_e; ++it)
	{
		pcstr p = (pcstr)it.key();
		System::String^ property_id = gcnew System::String(p);
		
		if (
			property_id!="effect_id" 
			//!m_property_name_to_desc_idx.ContainsKey(property_id)
		)
		{
			bool property_found;
			is_property_exists_in_effect(property_id, (*m_effect_description_config)->get_root()["properties"], property_found);
			if (!property_found)
			{
				(*m_config).erase		( p );
				bchanged				= true;
			}
		}
	}

	if(bchanged)
		m_parent_stage->m_parent->properties_changed( );
}

} // namespace editor
} // namespace xray