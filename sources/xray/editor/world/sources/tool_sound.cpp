////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_sound.h"
#include "level_editor.h"
#include "project_items.h"
#include "editor_world.h"
#include "object_sound.h"
#include "volumetric_sound.h"
#include <xray/sound/world_user.h>
#include "sound_tool_tab.h"
#include "actions.h"

namespace xray {
namespace editor {

tool_sound::tool_sound( level_editor^ le )
:super( le, "sound" )
{
	default_image_index	= xray::editor_base::node_sound_file;

	editor_base::input_engine^ input_engine = le->get_input_engine();
	editor_base::gui_binder^	gui_binder	= le->get_gui_binder();

	System::String^ action_name		= gcnew System::String("debug stream writing");
	input_engine->register_action	( gcnew action_sound_debug_stream_writer( action_name, get_level_editor() ), "");
	gui_binder->register_image		( "base", action_name, editor_base::terrain_debug_mode );

	action_name						= "dump debug stream writing";
	input_engine->register_action	( gcnew action_sound_debug_stream_writer_dump	( action_name, get_level_editor() ), "");
	gui_binder->register_image		( "base", action_name, editor_base::pin );

	m_tool_tab 				= gcnew sound_tool_tab(this, le->get_editor_world());
	m_single_sounds			= gcnew System::Collections::ArrayList( );
	m_volumetric_sounds		= gcnew System::Collections::ArrayList( );
	m_composite_sounds		= gcnew System::Collections::ArrayList( );
	m_sound_collections		= gcnew System::Collections::ArrayList( );

	load_library		( );
}

tool_sound::~tool_sound( )
{}

object_base^ tool_sound::create_library_object_instance( System::String^ library_name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );

	lib_item^ li				= m_library->get_library_item_by_full_path( library_name, true );
	System::String^ raw			= gcnew System::String((*(li->m_config))["lib_name"]);
	object_base^ result			= create_raw_object(raw, scene);
	result->load_props			( *li->m_config );
	result->set_library_name	( library_name );

	return						result;
}

object_base^ tool_sound::create_raw_object(System::String^ id, render::scene_ptr const& scene)
{
	object_base^ result;
	if ( id == "base:sound" )
		result					= gcnew object_sound(this, scene );
	else if ( id == "base:volumetric_sound" )
		result					= gcnew volumetric_sound(this, scene );

	R_ASSERT(result, "raw object not found", unmanaged_string(id).c_str());

	return result;
}

object_base^ tool_sound::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );
	lib_item^ li				= m_library->extract_library_item( t );
	System::String^ raw			= gcnew System::String((*(li->m_config))["lib_name"]);
	object_base^ result			= create_raw_object( raw, scene );
	result->load_props			( t );
	result->set_library_name	( li->full_path_name() );

	return						result;
}

void tool_sound::destroy_object( object_base^ o )
{
	if(o->get_visible())
		o->set_visible			( false );

	delete						o;
}

void tool_sound::load_library( )
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &super::on_library_files_loaded), g_allocator);
		resources::query_resource(
		"resources/library/sound/sound.library",
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

	fs_iterator_delegate* single_sounds_delegate = NEW(fs_iterator_delegate)
	(
		gcnew fs_iterator_delegate::Delegate( this, &tool_sound::on_single_sounds_library_fs_iterator_ready ),
		g_allocator
	);

	resources::query_vfs_iterator 
	(	
		"resources/sounds/single", 
		boost::bind( &fs_iterator_delegate::callback, single_sounds_delegate, _1 ),
		g_allocator,
		xray::resources::recursive_true
	);

	fs_iterator_delegate* composite_sounds_delegate = NEW(fs_iterator_delegate)
	(
		gcnew fs_iterator_delegate::Delegate( this, &tool_sound::on_composite_sounds_library_fs_iterator_ready),
		g_allocator
	);

	resources::query_vfs_iterator 
	(	
		"resources/sounds/composite", 
		boost::bind( &fs_iterator_delegate::callback, composite_sounds_delegate, _1 ),
		g_allocator,
		xray::resources::recursive_true
	);

	fs_iterator_delegate* sound_collections_delegate = NEW(fs_iterator_delegate)
	(
		gcnew fs_iterator_delegate::Delegate( this, &tool_sound::on_sound_collections_library_fs_iterator_ready),
		g_allocator
	);

	resources::query_vfs_iterator 
	(	
		"resources/sounds/collections", 
		boost::bind( &fs_iterator_delegate::callback, sound_collections_delegate, _1 ),
		g_allocator,
		xray::resources::recursive_true
	);

	fs_iterator_delegate* volumetric_sounds_delegate = NEW(fs_iterator_delegate)
	(
		gcnew fs_iterator_delegate::Delegate( this, &tool_sound::on_volumetric_sounds_library_fs_iterator_ready ),
		g_allocator
	);

	resources::query_vfs_iterator 
	(	
		"resources/sounds/volumetric",
		boost::bind(&fs_iterator_delegate::callback, volumetric_sounds_delegate, _1),
		g_allocator,
		xray::resources::recursive_true
	);
}

tool_tab^ tool_sound::ui( )
{
	return m_tool_tab;
}

void tool_sound::on_library_loaded( )
{
	super::on_library_loaded	( );
	m_tool_tab->fill_objects_list( get_selected_library_name() );

//	sound::world_user& wu	= m_level_editor->get_editor_world().engine().get_sound_world().get_editor_world_user();
//	wu.set_active_sound_scene( *m_sound_scene, 0, 0 );
}

void tool_sound::process_recursive_names( vfs::vfs_iterator const& in_it, System::String^ path, System::Collections::ArrayList^ file_names, System::String^ extention )
{
	vfs::vfs_iterator it				= in_it;
	while( !it.is_end() )
	{
		unmanaged_string ext				= extention;
		int ext_len							= strings::length(ext.c_str());
		System::String^ name				= gcnew System::String(it.get_name());
		System::String^ full_name			= path;
		
		if(full_name->Length)
			full_name					+= "/";


		if(strstr(it.get_name(), ext.c_str()))
		{
			name						= name->Remove(name->Length-ext_len);
			full_name					+= name;
			file_names->Add				( full_name );
		}
		else if(it.is_folder())
		{
			full_name					+= name;
			process_recursive_names		( it.children_begin(), full_name, file_names, extention );
		}
		++it;
	}

}

void tool_sound::on_single_sounds_library_fs_iterator_ready( vfs::vfs_locked_iterator const& fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "", m_single_sounds, ".wav" );
}

void tool_sound::on_composite_sounds_library_fs_iterator_ready	( vfs::vfs_locked_iterator const& fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "", m_composite_sounds, ".composite_sound" );
}

void tool_sound::on_sound_collections_library_fs_iterator_ready	( vfs::vfs_locked_iterator const& fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "", m_sound_collections, ".sound_collection" );
}

void tool_sound::on_volumetric_sounds_library_fs_iterator_ready( vfs::vfs_locked_iterator const& fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "", m_volumetric_sounds, ".volumetric_sound" );
}

} // namespace editor
} // namespace xray
