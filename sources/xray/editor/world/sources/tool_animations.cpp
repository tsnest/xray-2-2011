////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "tool_animations.h"

namespace xray {
namespace editor {

tool_animations::tool_animations	( level_editor^ le ) :
	super							( le, "animations_list" )
{
	load_library					( );
}

tool_animations::~tool_animations	( )
{
}

object_base^ tool_animations::create_library_object_instance( System::String^ library_name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	NOT_IMPLEMENTED				( library_name, scene, scene_view );
	return						nullptr;
}

object_base^ tool_animations::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	NOT_IMPLEMENTED				( t, scene, scene_view );
	return						nullptr;
}

void tool_animations::destroy_object( object_base^ o )
{
	NOT_IMPLEMENTED					( o );
}

void tool_animations::load_library	( )
{
	fs_iterator_delegate* q			= NEW( fs_iterator_delegate )( gcnew fs_iterator_delegate::Delegate(this, &tool_animations::on_fs_iterator_ready ), g_allocator );
	resources::query_vfs_iterator(
		"resources/animations", 
		boost::bind( &fs_iterator_delegate::callback, q, _1 ), 
		g_allocator, 
		resources::recursive_true
	);
}

void tool_animations::on_fs_iterator_ready	( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names					( fs_it.children_begin(), "" );
}

void tool_animations::process_recursive_names( vfs::vfs_iterator const & in_it, System::String^ path )
{
	vfs::vfs_iterator it				= in_it;
	/*
	pcstr const clip_name_prefix		= ".clip";
	u32 const clip_name_prefix_length	= strings::length( clip_name_prefix );

	while ( !it.is_end() )
	{
		pcstr clip_name					= it.get_name();
		bool bclip						= strstr( clip_name, clip_name_prefix ) != 0;
		
		if ( it.is_folder() || bclip )
		{
			System::String^ name		= gcnew System::String( it.get_name() );
			System::String^ full_path	= path;
			
			if ( full_path->Length )
				full_path				+= "/";

			if ( bclip )
			{
				name					= name->Remove( name->Length - clip_name_prefix_length );
				full_path				+= name;
				m_library->add_simple	( full_path );
			}
			else if ( it.is_folder() )
			{
				full_path				+= name;
				process_recursive_names	( it.children_begin(), full_path );
			}
		}
		++it;
	}
	*/
	while ( !it.is_end() )
	{
		if ( it.is_folder() )
		{
			System::String^ folder_name = gcnew System::String( it.get_name() );
			if ( folder_name == "controllers" || folder_name == "skeletons" || folder_name == "viewer_scenes" )
			{
				++it;
				continue;
			}

			process_recursive_names( it.children_begin(), path + folder_name + "/" );
		}
		else
		{
			System::String^ file_extension = System::IO::Path::GetExtension( gcnew System::String( it.get_name() ) );
			if ( file_extension == ".clip" || file_extension == ".anim_collection" )
			{
				System::String^ file_name = System::IO::Path::GetFileNameWithoutExtension( gcnew System::String( it.get_name() ) );
				m_library->add_simple	( path + file_name );
			}
		}
		++it;
	}
}

tool_tab^ tool_animations::ui	( )
{
	NOT_IMPLEMENTED				( );
	return						nullptr;
}

} // namespace editor
} // namespace xray