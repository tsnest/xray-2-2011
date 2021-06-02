////////////////////////////////////////////////////////////////////////////
//	Created		: 22.10.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "tool_misc.h"
#include "level_editor.h"
#include "misc_tool_tab.h"
#include "project_items.h"
#include "editor_world.h"
#include "project.h"
#include "object_track.h"
#include "object_patrol_graph.h"
#include "object_camera.h"
#include "object_human_npc.h"
#include "object_environment.h"
#include "object_environment_settings.h"
#include "object_sky.h"
#include "object_decal.h"
#include "object_collision_geometry.h"
#include "object_proximity_trigger.h"
#include "object_collision_geometry_set_plane_action.h"
#include "object_values_storage.h"
#include "object_wire_set.h"


namespace xray {
namespace editor {

tool_misc::tool_misc		( level_editor^ le ) :
	super					( le, "misc" ),
	m_animated_model_names	( gcnew List< String^ >() )
{
	default_image_index		= xray::editor_base::misc_tree_icon;
	m_tool_tab 				= gcnew misc_tool_tab( this );
	load_library			( );

	object_patrol_graph::initialize_in_tool( this );

	m_collision_geometry_set_plane_action	= gcnew object_collision_geometry_set_plane_action( "set truncated sphere plane", le->view_window() );
	le->get_input_engine()->register_action	( m_collision_geometry_set_plane_action, "Ctrl+LButton(View)" );
}

tool_misc::~tool_misc		( )
{
}

void tool_misc::on_new_project	( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	object_base^ o				= create_library_object_instance( "environment", scene, scene_view );
	project_item_object^ scene_item	= gcnew project_item_object( m_level_editor->get_project(), o );
	scene_item->assign_id		( 0 );
	scene_item->m_guid			= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( scene_item );
	o->set_visible				( true );
	o->set_name					( "environment", false );
	
	o							= create_library_object_instance( "sky", scene, scene_view );
	scene_item					= gcnew project_item_object( m_level_editor->get_project(), o );
	scene_item->assign_id		( 0 );
	scene_item->m_guid			= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( scene_item );
	o->set_visible				( true );
	o->set_name					( "sky", false );
	
	o							= create_library_object_instance( "camera", scene, scene_view );
	scene_item					= gcnew project_item_object( m_level_editor->get_project(), o );
	scene_item->assign_id		( 0 );
	scene_item->m_guid			= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( scene_item );
	o->set_visible				( true );
	o->set_name					( "camera", false );
}

object_base^ tool_misc::create_library_object_instance( System::String^ library_name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	lib_item^ li				= m_library->get_library_item_by_full_path( library_name, true );
	System::String^ raw			= gcnew System::String( (*( li->m_config ))["lib_name"] );
	object_base^ result			= create_raw_object( raw, scene, scene_view );
	result->load_props			( *li->m_config );
	result->set_library_name	( library_name );

	return						result;
}

object_base^ tool_misc::create_raw_object( System::String^ id, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	object_base^ result			= nullptr;
	
	if ( id == "base:track" )
	{
		result					= gcnew object_track( this, scene );
	}
	else if ( id == "base:camera" )
	{
		result					= gcnew object_camera( this );
	}
	else if ( id == "base:patrol_graph" )
	{
		result					= gcnew object_patrol_graph( this );
	}
	else if ( id == "base:human_npc" )
	{
		result					= gcnew object_human_npc( this, scene );
	}
	else if ( id == "base:timer" )
	{
		result					= gcnew object_timer( this );
	}
	else if ( id == "base:environment" )
	{
		result					= gcnew object_environment( this, scene_view );
	}
	else if ( id == "base:environment_settings" )
	{
		result					= gcnew object_environment_settings( this );
	}
	else if ( id == "base:sky" )
	{
		result					= gcnew object_sky( this, scene );
	}
	else if ( id == "base:decal" )
	{
		result					= gcnew object_decal( this, scene );
	}
	else if ( id == "base:collision_geometry" )
	{
		result					= gcnew object_collision_geometry( this );
	}
	else if ( id == "base:proximity_trigger" )
	{
		result					= gcnew object_proximity_trigger( this );
	}
	else if ( id == "base:values_storage" )
	{
		result					= gcnew object_values_storage( this );
	}
	else if ( id == "base:wire_set" )
	{
		result					= gcnew object_wire_set( this );
	}
	
	ASSERT						( result, "raw object not found", unmanaged_string( id ).c_str() );
	return						result;
}

object_base^ tool_misc::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	lib_item^ li				= m_library->extract_library_item( t );
	System::String^ raw			= gcnew System::String( (*( li->m_config ))["lib_name"] );
	object_base^ result			= create_raw_object( raw, scene, scene_view );
	result->load_props			( t );
	result->set_library_name	( li->full_path_name() );

	return						result;
}

void tool_misc::destroy_object	( object_base^ o )
{
	if ( o->get_visible() )
		o->set_visible			( false );

	delete						o;
}

void tool_misc::load_library	( )
{
	query_result_delegate* rq = NEW( query_result_delegate )( gcnew query_result_delegate::Delegate( this, &super::on_library_files_loaded ), g_allocator );
		resources::query_resource(
		"resources/library/misc/misc.library",
		resources::lua_config_class,
		boost::bind( &query_result_delegate::callback, rq, _1 ),
		g_allocator
	);

	load_animated_models_names	( );
}

tool_tab^ tool_misc::ui			( )
{
	return						m_tool_tab;
}

void tool_misc::on_library_loaded	( )
{
	super::on_library_loaded		( );
	m_tool_tab->fill_objects_list	( );
}

void tool_misc::load_animated_models_names			( )
{
	fs_iterator_delegate* animated_models_delegate	= NEW( fs_iterator_delegate )( gcnew fs_iterator_delegate::Delegate( this, &tool_misc::on_animated_models_fs_iterator_ready ), g_allocator );

	resources::query_vfs_iterator					(	
		"resources/animated_model_instances", 
		boost::bind( &fs_iterator_delegate::callback, animated_models_delegate, _1 ),
		g_allocator,
		xray::resources::recursive_false
	);
}

void tool_misc::on_animated_models_fs_iterator_ready( vfs::vfs_locked_iterator const& fs_it )
{
	process_recursive_names							( fs_it.children_begin() );

	project^ project								= get_level_editor()->get_project();
	project->select_objects							( project->selection_list(), enum_selection_method_set );
}

void tool_misc::process_recursive_names				( vfs::vfs_iterator const& in_it )
{
	vfs::vfs_iterator it							= in_it;
	
	while ( !it.is_end() )
	{
		unmanaged_string ext						= ".animated_model";
		u32 const ext_length						= strings::length( ext.c_str() );
		System::String^ name						= gcnew System::String( it.get_name() );
		
		if ( strstr( it.get_name(), ext.c_str() ) )
		{
			name									= name->Remove( name->Length - ext_length );
			m_animated_model_names->Add				( name );
		}
		++it;
	}
}

} // namespace editor
} // namespace xray