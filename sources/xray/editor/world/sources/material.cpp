////////////////////////////////////////////////////////////////////////////
//	Created		: 23.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material.h"
#include "material_callback_binders.h"
#include "material_editor.h"
#include "texture_editor.h"
#include "effect_selector.h"
#include "stage_selector.h"
#include "material_stage.h"
#include "editor_world.h"

#pragma managed( push, off )
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

using namespace System;
using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::property_editors::attributes;

namespace xray{
namespace editor{

material::material( editor_world& w )
:m_editor_world		( w ), 
m_material_config	( NULL ),
m_load_query		( NULL )
{
	m_material_config_backup		= NEW( configs::lua_config_ptr)( configs::create_lua_config() );
}

material::~material( )
{
	destroy_contents( );
	DELETE			( m_material_config );
	DELETE			( m_material_config_backup );
	m_material_config			= NULL;
	m_material_config_backup	= NULL;
	if(m_load_query)
		m_load_query->m_rejected = true;
}

void material::destroy_contents( )
{
	for each (material_stage^ s in m_stages.Values)
		delete s;

	m_stages.Clear	( );
}

configs::lua_config_value material::config_root( )
{
	return (*m_material_config)->get_root()["material"];
}

bool material::add_stage( )
{
	stage_selector^ selector	= gcnew stage_selector();
	selector->existing_items	= m_stages.Keys;

	if(selector->ShowDialog() == DialogResult::OK)
	{
		System::String^ stage_name		= selector->selected_item;
		unmanaged_string				s( stage_name );
		configs::lua_config_value new_stage	= config_root( )[s.c_str()];
		config_root()["common"]["stages"][s.c_str()]	= 1;

		new_stage["effect"]["effect_id"] = "none";

		material_stage^ stage			= gcnew material_stage( this, stage_name, new_stage );
		m_loading_stages				+= 1;
		m_stages.Add					( stage_name, stage );
		stage->load						( );	
		m_changed_delegate				( this );
		return true;
	}
	return false;
}

bool material::remove_stage( Object^ stage )
{
	System::String^ stage_name = (System::String^)stage;
	m_stages.Remove			( stage_name );
	
	unmanaged_string		s( stage_name );

	config_root().erase		( s.c_str() );
	config_root()["common"]["stages"].erase( s.c_str() );
	m_changed_delegate		( this );
	return					true;
}

void material::stage_loaded( )
{
	--m_loading_stages;
	
	R_ASSERT( m_loading_stages>=0 );
	
	if( m_loading_stages == 0 )
	{
		loaded				= true;
		m_loaded_delegate	( this, true );
	}
}

void material::store_backup_config( )
{
	//save default config
	(*m_material_config_backup)->get_root().assign_lua_value( (*m_material_config)->get_root().copy() );
}

void material::load_from_config( configs::lua_config_ptr config )
{
	//save config ptr
	m_material_config	= NEW( configs::lua_config_ptr )( config );
	store_backup_config	( );
	load_internal		( );
	check_integrity		( );
}

void material::load_internal( )
{
	loaded						= false;
	destroy_contents			( );

	m_loading_stages							= 0;

	configs::lua_config_value mtl_root			= config_root();

	{
		configs::lua_config_value::iterator st_it	= mtl_root["common"]["stages"].begin();
		configs::lua_config_value::iterator st_it_e	= mtl_root["common"]["stages"].end();
		if( st_it != st_it_e )
		{
			for(; st_it != st_it_e; ++st_it )
			{
				pcstr name = st_it.key();
				configs::lua_config_value current	= mtl_root[name];

				System::String^ stage_name	= gcnew System::String(name);
				material_stage^ stage		= gcnew material_stage( this, stage_name, current );
				++m_loading_stages;
				m_stages.Add				( stage_name, stage );
				stage->load					( );	
			}
		}
		else
		{
			m_loading_stages	= 1;
			stage_loaded		( );
		}
	}
}

System::String^ material::description::get( )							
{ 
	return gcnew System::String( (pcstr)config_root()["common"]["description"] ); 
}

void material::description::set( System::String^ value )	
{ 
	config_root()["common"]["description"] = unmanaged_string(value).c_str();
}

void material::init_new( )
{
	m_material_config			= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	description					= "No Description";
}

void material::create_from( configs::lua_config_value const& config )
{
	m_material_config					= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	( *m_material_config )->get_root( )	= ( config.copy() );
	store_backup_config					( );
	load_internal						( );
}

void material::copy_from( resource_editor_resource^ other )
{
	material^ other_material			= safe_cast<material^>( other );
	m_material_config					= NEW( configs::lua_config_ptr )( configs::create_lua_config() );
	( *m_material_config )->get_root( )	= ( *other_material->m_material_config )->get_root( ).copy( );
	store_backup_config					( );
	load_internal						( );
}

void material::preview_changes( )
{
	m_editor_world.get_renderer().apply_material_changes(
		(pcstr)unmanaged_string(m_name).c_str(),
		(*m_material_config)->get_root()
	);
}

void material::apply_changes( )
{
	(*m_material_config_backup)->get_root().assign_lua_value( (*m_material_config)->get_root().copy() );
}

void material::reset_to_default( )
{
	(*m_material_config)->get_root().assign_lua_value( (*m_material_config_backup)->get_root().copy() );
	
	preview_changes				( );
	
	m_is_renamed				= false;
	m_is_deleted				= false;
	m_name						= m_old_name;
	load_internal				( );
}

void material::config_loaded( resources::queries_result& result )
{
	m_load_query				= NULL;

	if(!result[0].is_successful())
	{
		m_is_broken				= true;
		loaded					= true;
		properties_changed		( );
		return;
	}

	configs::lua_config_ptr config	= static_cast_resource_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());
	
	R_ASSERT(m_name->Length!=0);
	m_old_name			= m_name;

	load_from_config	( config );
}

System::String^ material::source_path( System::String^ short_name, System::String^ resources_path )
{
	R_ASSERT(short_name && short_name->Length!=0);
	return resources_path + "/sources/materials/" + short_name + ".material";
}

System::String^ material::resource_name( System::String^ short_name )
{
	R_ASSERT(short_name && short_name->Length!=0);
	return "resources/materials/" + short_name + ".material";
}

void material::load( System::String^ name )
{
	m_name					= name;

	m_load_query = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate( this, &material::config_loaded), g_allocator);

	resources::query_resource(
		unmanaged_string(resource_name(m_name)).c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, m_load_query, _1),
		g_allocator
	);
}

void material::save( )
{
	R_ASSERT(!m_is_broken);

	System::String^ resource_path	= gcnew System::String (m_editor_world.engine().get_resources_path() );

	System::String^ current_name	= source_path( m_name, resource_path );

	if(m_is_deleted)
	{
		editor_base::fs_helper::remove_to_recycle_bin( current_name );
		return;
	}

	if(m_is_renamed && !m_is_new)
	{
		System::String^ old_name	= source_path( m_old_name, resource_path );
		File::Move					( old_name, current_name );
	}
	configs::lua_config_ptr config_ptr		= configs::create_lua_config( unmanaged_string( resource_name(m_name)).c_str() );

	if( !config_root()["common"].value_exists("stages") )
	{
		R_ASSERT(m_stages.Count==0);
	}

	config_ptr->get_root().assign_lua_value	( ( *m_material_config )->get_root() );
	config_ptr->save						( configs::target_sources );	

	m_old_name					= m_name;
	m_is_new					= false;
	m_is_renamed				= false;
}
	
wpf_property_container^	material::get_property_container( bool chooser_mode )
{
	if(m_is_broken)
		return nullptr;

	XRAY_UNREFERENCED_PARAMETER		( chooser_mode );

	wpf_property_container^ cont	= gcnew wpf_property_container;
	cont->properties->add_from_obj	( this, "description" );

	wpf_property_container^ stages_property_container= gcnew wpf_property_container();
	property_descriptor^ desc		= cont->properties->add_container( "Stages", "Stages", "", stages_property_container );

	desc->dynamic_attributes->add	( gcnew wpf_controls::property_editors::attributes::collection_attribute(
			gcnew System::Func<bool>( this, &material::add_stage),
			gcnew System::Func<Object^, bool>( this, &material::remove_stage) )
	);
	desc->is_expanded			= true;

	for each( material_stage^ stage in m_stages.Values )
	{
		wpf_property_container^ stage_cont	= stage->get_property_container( true );

		property_descriptor^ desc			= stages_property_container->properties->add_container( stage->name, "", "", stage_cont );
		desc->dynamic_attributes->add		( gcnew special_attribute( "material_stage_holder" ) );
	}
	return cont;
}

void material::check_integrity( )
{
	for each( material_stage^ stage in m_stages.Values )
	{
		stage->check_integrity();
	}
}

} // namespace editor
} // namespace xray