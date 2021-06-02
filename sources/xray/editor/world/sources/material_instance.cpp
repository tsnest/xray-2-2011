////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_instance.h"
#include "material.h"
#include "material_stage.h"
#include "material_effect.h"
#include "material_property_base.h"
#include "editor_world.h"
#include "material_editor.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

bool material_instance::check_material_inheritance( )
{
	configs::lua_config_value root			= (*m_material_config)->get_root()["material"];

	if(!root.are_all_super_tables_loaded_correctly())
	{
	
		return false;
	}

	configs::lua_config_value base_root		= root.get_super_tables_table()[0];

	fs_new::virtual_path_string	supertable_name;
	fs_new::virtual_path_string	supertable_module_name		= "materials/";
	supertable_module_name						+= (pcstr)root["instance"]["base_material"];
	supertable_module_name						+= ".material";

// first check old stages in instance
	configs::lua_config_iterator stages_it		= root.begin();
	configs::lua_config_iterator stages_it_e	= root.end();
	for (; stages_it!=stages_it_e; ++stages_it)
	{
		pcstr stage_name						= (pcstr)stages_it.key();
		
		if( strings::equal(stage_name,"instance") )//reserved
			continue;

		if(!base_root["common"]["stages"].value_exists(stage_name))
		{
			root.erase							( stage_name );
			properties_changed					( );
		}
	}


	stages_it									= base_root["common"]["stages"].begin();
	stages_it_e									= base_root["common"]["stages"].end();

	for (; stages_it!=stages_it_e; ++stages_it)
	{
		pcstr stage_name						= (pcstr)stages_it.key();
		
		configs::lua_config_value t0			= root[stage_name];
		configs::lua_config_value t1			= base_root[stage_name];
		if( t0.is_same_object(t1) )
		{
			supertable_name.assignf				( "material.%s", stage_name );
			root[stage_name].create_table().add_super_table( supertable_module_name.c_str(), supertable_name.c_str() );
			properties_changed();
		}

		t0	= root[stage_name]["effect"];
		t1	= base_root[stage_name]["effect"];
		if( t0.is_same_object(t1) )
		{
			supertable_name.assignf				( "material.%s.effect", stage_name );
			root[stage_name]["effect"].create_table().add_super_table( supertable_module_name.c_str(), supertable_name.c_str() );
			properties_changed();
		}

		// effect properties
		// material->instance
		configs::lua_config_iterator it			= base_root[stage_name]["effect"].begin();
		configs::lua_config_iterator it_e		= base_root[stage_name]["effect"].end();

		for(; it!=it_e; ++it)
		{
			pcstr prop_name					= (pcstr)it.key();
			configs::lua_config_value table	= *it;

			if( !table.value_exists("is_model_parameter"))
				continue;

			t0	= root[stage_name]["effect"][prop_name];
			t1	= base_root[stage_name]["effect"][prop_name];

			if( (bool)table["is_model_parameter"] )
			{
				configs::enum_types t0_type = t0["value"].get_type();
				configs::enum_types t1_type = t1["value"].get_type();
				bool types_compartible =(t0_type==t1_type) ||
										(t0_type==configs::t_float && t1_type==configs::t_integer) ||
										(t1_type==configs::t_float && t0_type==configs::t_integer);

				if( t0.is_same_object(t1) || !types_compartible )
				{
					supertable_name.assignf("material.%s.effect.%s", stage_name, prop_name );
					root[stage_name]["effect"][prop_name].create_table().add_super_table( supertable_module_name.c_str(), supertable_name.c_str() );
					properties_changed();
				}
			}else
			{
				if( !t0.is_same_object(t1) )
				{
					root[stage_name]["effect"].erase(prop_name);
					properties_changed	();
				}
			}
		} //all effects

// instance->material
		it			= root[stage_name]["effect"].begin();
		it_e		= root[stage_name]["effect"].end();

		configs::lua_config_value base_effect = base_root[stage_name]["effect"];

		for(; it!=it_e; ++it)
		{
			pcstr prop_name					= (pcstr)it.key();
			if(!base_root[stage_name]["effect"].value_exists(prop_name))
			{
				root[stage_name]["effect"].erase(prop_name);
				properties_changed();
			}
		}


		// stage properties
		it		= base_root[stage_name].begin();
		it_e	= base_root[stage_name].end();

		for(; it!=it_e; ++it)
		{
			pcstr prop_name						= (pcstr)it.key();

			configs::lua_config_value table		= *it;

			if( !table.value_exists("is_model_parameter"))
				continue;

			configs::lua_config_value t0	= root[stage_name][prop_name];
			configs::lua_config_value t1	= base_root[stage_name][prop_name];

			if( (bool)table["is_model_parameter"] )
			{
				configs::enum_types t0_type = t0["value"].get_type();
				configs::enum_types t1_type = t1["value"].get_type();
				bool types_compartible =(t0_type==t1_type) ||
					(t0_type==configs::t_float && t1_type==configs::t_integer) ||
					(t1_type==configs::t_float && t0_type==configs::t_integer);

				if( t0.is_same_object(t1) || !types_compartible )
				{
					supertable_name.assignf("material.%s.%s", stage_name, prop_name );
					root[stage_name][prop_name].create_table().add_super_table( supertable_module_name.c_str(), supertable_name.c_str() );
					properties_changed();
				}
			}else
			{
				if( !t0.is_same_object(t1) )
				{
					root[stage_name].erase(prop_name);
					properties_changed	();
				}
			}
		}
	}
	return true;
}

material_instance::material_instance( editor_world& w )
:super						( w )
{
}

material_instance::~material_instance( )
{
}

void material_instance::init_new( )
{
	m_material_config	= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	base_material		= "default";
}

void material_instance::load_from_config( configs::lua_config_ptr config )
{
	m_material_config	= NEW( configs::lua_config_ptr )( config );
	store_backup_config	( );

	if( check_material_inheritance() )
	{
		super::load_internal();
	}else
	{
		base_material		= "default";
		properties_changed	( );
	}
}

System::String^ material_instance::base_material::get( )
{
	return gcnew System::String( (pcstr)config_root()["instance"]["base_material"] );
}

void material_instance::base_material::set( System::String^ base_material_name )	
{ 
	System::Collections::Generic::Dictionary<System::String^, System::String^>	stored_base_textures;
	for each( material_stage^ s in m_stages.Values)
	{
		System::String^ texture_name = s->m_effect->get_base_texture();
		if(texture_name)
			stored_base_textures[s->name] = texture_name;
	}

	configs::lua_config_value root	= (*m_material_config)->get_root()["material"].create_table();// force empty table
	config_root()["instance"]["base_material"] = unmanaged_string(base_material_name).c_str();
	
	System::String^ supertable_module_name = "materials/" + base_material_name + ".material";
	root.add_super_table			( unmanaged_string(supertable_module_name).c_str(), "material");

	check_material_inheritance		( );
	load_internal					( );


	for each( material_stage^ s in m_stages.Values)
	{
		System::String^ stage_name = s->name;
		if(stored_base_textures.ContainsKey(stage_name))
			s->m_effect->set_base_texture( stored_base_textures[stage_name] );
	}
}

System::String^ material_instance::source_path( System::String^ short_name, System::String^ resources_path )
{
	R_ASSERT(short_name && short_name->Length!=0);
	return resources_path + "/sources/material_instances/" + short_name + ".material";
}

System::String^ material_instance::resource_name( System::String^ short_name )
{
	R_ASSERT(short_name && short_name->Length!=0);
	return "resources/material_instances/" + short_name + ".material";
}

void material_instance::create_from( configs::lua_config_value const& config )
{
	m_material_config					= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	( *m_material_config )->get_root( )	= ( config.copy( ) );
	store_backup_config					( );
	load_internal						( );
}

void material_instance::copy_from( resource_editor_resource^ other )
{
	material_instance^ other_material	= safe_cast<material_instance^>(other);
	m_material_config					= NEW(configs::lua_config_ptr)(configs::create_lua_config() );
	(*m_material_config)->get_root()	= (*other_material->m_material_config)->get_root().copy();
	store_backup_config					( );
	load_internal						( );
}

void material_instance::base_material_selector( wpf_controls::property_editors::property^ prop, Object^ filter )
{
	System::String^ current_mtl			= safe_cast<System::String^>(filter);
	System::String^ selected_material	= nullptr;
	
	bool result = editor_base::resource_chooser::choose( "material", current_mtl, nullptr, selected_material, false );
	if(result)
		prop->value = selected_material;
}

wpf_property_container^	material_instance::get_property_container( bool chooser_mode )
{
	if(m_is_broken)
		return nullptr;

	XRAY_UNREFERENCED_PARAMETERS( chooser_mode );

	wpf_property_container^ cont	= gcnew wpf_property_container;
	property_descriptor^ desc		= cont->properties->add_from_obj	( this, "base_material" );
	
//.	if(!chooser_mode)
	desc->dynamic_attributes->add( gcnew external_editor_attribute( gcnew external_editor_event_handler( this, &material_instance::base_material_selector ), false, base_material ) );

	wpf_property_container^ stages_property_container= gcnew wpf_property_container();
	desc							= cont->properties->add_container( "Stages", "Stages", "", stages_property_container );
	desc->is_expanded				= true;

	for each( material_stage^ stage in m_stages.Values )
	{
		desc = stages_property_container->properties->add_container( stage->name, "", "", stage->get_property_container( false ) );
		desc->is_expanded				= true;
	}

	return cont;
}

} // namespace editor
} // namespace xray
