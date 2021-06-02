////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_scripting.h"
#include "scripting_tool_tab.h"
#include "object_scene.h"
#include "object_job.h"
#include "object_logic.h"
#include "level_editor.h"
#include "editor_world.h"
#include "project_items.h"
#include "project.h"
#include "logic_event.h"

namespace xray{
namespace editor{

tool_scripting::tool_scripting(level_editor^ le)
:super(le, "scripting")
{
	default_image_index			= xray::editor_base::script_object_tree_icon;

	job_resource_type_by_job_type = gcnew Generic::Dictionary< String^, String^>( );
	job_resource_type_by_job_type->Add("camera",	"xray.editor.object_camera");
	job_resource_type_by_job_type->Add("timing",	"xray.editor.object_timer");
	job_resource_type_by_job_type->Add("lights",	"xray.editor.object_light");
	job_resource_type_by_job_type->Add("triggers",	"xray.editor.object_proximity_trigger");
	job_resource_type_by_job_type->Add("composite", "xray.editor.object_composite");
	job_resource_type_by_job_type->Add("values_storage", "xray.editor.object_values_storage");
	job_resource_type_by_job_type->Add("npc", "xray.editor.object_human_npc");

	behaviour_types_by_object_types = gcnew Generic::Dictionary< String^, Generic::List<String^>^>( );
	Generic::List<String^>^ available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("camera_track");
	available_behaviours->Add("camera_from_object");
	available_behaviours->Add("camera_free_fly");
	behaviour_types_by_object_types->Add("camera", available_behaviours);

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("active_camera");	
	behaviour_types_by_object_types->Add("camera_director", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("event_generator");	
	behaviour_types_by_object_types->Add("timing", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("light_static");
	available_behaviours->Add("light_anim");	
	behaviour_types_by_object_types->Add("lights", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("proximity_trigger_default");
	behaviour_types_by_object_types->Add("triggers", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("composite_default");
	behaviour_types_by_object_types->Add("composite", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("values_storage_default");
	behaviour_types_by_object_types->Add("values_storage", available_behaviours );

	available_behaviours  = gcnew Generic::List<String^>( );
	available_behaviours->Add("npc_play_animation");
	available_behaviours->Add("npc_patrol");
	behaviour_types_by_object_types->Add("npc", available_behaviours );


	load_library				( );	
}

tool_tab^ tool_scripting::ui()
{
	return nullptr;
}

void tool_scripting::on_library_object_created( project_item_object^ )
{	 	
}

object_base^ tool_scripting::load_object( configs::lua_config_value const& t, render::scene_ptr const& , render::scene_view_ptr const&  )	
{
	object_base^ result				= nullptr;
	System::String^ library_name	= gcnew System::String(t["lib_name"]);

	if( library_name->StartsWith("base:") )
	{
		result						= create_raw_object( library_name );
		result->load_props			( t );
		result->set_library_name	( library_name );
	}else
	{
		lib_item^ li				= m_library->extract_library_item( t );
		System::String^ ln			= gcnew System::String((*(li->m_config))["lib_name"]);
		result						= create_raw_object( ln );
		result->load_props			( *li->m_config );
		result->load_props			( t );
		result->set_library_name	( li->full_path_name() );
	}


	return						result;
}

object_base^ tool_scripting::create_raw_object(System::String^ id )
{
	object_logic^ result = nullptr;
	if ( id == "base:scene" )
	{
		result = gcnew object_scene( this, false ); 
	}else 
	if ( id == "base:root_scene" )
	{
		result = gcnew object_scene( this, true ); 
	}else 
	if ( id == "base:job" )
	{
		result = gcnew object_job( this ); 
	}

	return result;
}

object_base^ tool_scripting::create_library_object_instance( System::String^ library_name, render::scene_ptr const& , render::scene_view_ptr const& )
{
	lib_item^ li				= m_library->get_library_item_by_full_path( library_name, true );
	System::String^ ln			= gcnew System::String((*(li->m_config))["lib_name"]);
	object_base^ result			= create_raw_object( ln );
	result->load_props			( *li->m_config );
	result->set_library_name	( library_name );

	return						result;
}

void tool_scripting::destroy_object( object_base^ o )						
{
//	ASSERT					(m_object_list->Contains(o));
	if(o->get_visible())
		o->set_visible			( false );

//	m_object_list->Remove		( o );
	delete						o;
}

void tool_scripting::load_library						( )
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &super::on_library_files_loaded), g_allocator);
	resources::query_resource(
		"resources/library/scripting/scripting.library",
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void tool_scripting::on_library_loaded					( )
{
	super::on_library_loaded( );
//	m_tool_tab->fill_objects_list( );
}

void tool_scripting::fill_tree_view(tree_node_collection^ nodes)
{
	nodes->Clear			( );

//	m_library->fill_tree	( nodes, xray::editor_base::npc_tree_icon, true, nullptr );
}

void tool_scripting::on_new_project( render::scene_ptr const& , render::scene_view_ptr const& )
{
	object_base^ o				= create_raw_object( "base:root_scene" );
	o->set_library_name			( "base:root_scene" );
	o->set_name					( "root_scene", false );
	
	o->load_defaults			( );

	project_item_object^ scene_item	= gcnew project_item_object( m_level_editor->get_project(), o );
	scene_item->assign_id		( 0 );
	scene_item->m_guid			= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( scene_item );
	m_level_editor->get_project()->root_scene = scene_item;
	o->set_visible				( true );	
	safe_cast<object_scene^>( o )->initialize( );

}


}//namespace editor
}//namespace xray
