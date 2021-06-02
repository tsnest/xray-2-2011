////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_speedtree.h"
#include "object_speedtree.h"

#include "solid_visual_tool_tab.h"
#include "level_editor.h"
#include "editor_world.h"

namespace xray{
namespace editor{

tool_speedtree::tool_speedtree( level_editor^ le )
:super(le, "speedtree")
{
	default_image_index		= xray::editor_base::node_speedtree;
	m_tool_tab 				= gcnew solid_visual_tool_tab(this);
}

tool_speedtree::~tool_speedtree( )
{
}

editor::tool_tab^ tool_speedtree::ui( )
{
	return m_tool_tab;
}

void tool_speedtree::initialize( )
{
	load_library();
};

void tool_speedtree::load_library( )
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &tool_speedtree::on_library_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	"resources/speedtree", 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

object_base^ tool_speedtree::create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETERS(&scene_view);
	return create_raw_object		( name, scene );
}

object_base^ tool_speedtree::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	System::String^ library_name	= gcnew System::String(t["lib_name"]);
	object_base^ result				= create_library_object_instance( library_name, scene, scene_view );
	result->load_props				( t );
	return result;
}

object_base^ tool_speedtree::create_raw_object( System::String^ id, render::scene_ptr const& scene )
{
	object_speedtree^ result		=  gcnew object_speedtree( this, scene );

	int del_ind						= id->LastIndexOf("/");
	if( del_ind > 0 )
		result->set_name			( id->Substring(del_ind+1), true );
	else
		result->set_name			( id, true);

	result->set_library_name		( id );

	return							result;
}

void tool_speedtree::destroy_object( object_base^ o )
{
	if(o->get_visible())
		o->set_visible(false);

	delete o;
}

void tool_speedtree::process_recursive_names( vfs::vfs_iterator const & in_it, 
											System::String^	path )
{
	vfs::vfs_iterator it				= in_it;
	while( !it.is_end() )
	{
		pcstr model_name_prefix			= ".srt";
		int model_name_prefix_len		= strings::length(model_name_prefix);
		System::String^ name			= gcnew System::String(it.get_name());
		System::String^ full_name		= path;
		
		if(full_name->Length)
			full_name					+= "/";


		if(strstr(it.get_name(), model_name_prefix))
		{
			name						= name->Remove(name->Length-model_name_prefix_len);
			full_name					+= name;
			m_library->add_simple		( full_name );
		}else
		if(it.is_folder())
		{
			full_name					+= name;
			process_recursive_names		( it.children_begin(), full_name );
		}
		++it;
	}

}

void tool_speedtree::on_library_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "" );
	m_tool_tab->fill_objects_list( get_selected_library_name() );
	
	m_b_ready					= true;
	m_level_editor->tool_loaded			( this );
}

void tool_speedtree::item_double_clicked( lib_item_base^ item )
{
	xray::editor_base::tool_window^ ed = get_level_editor()->get_editor_world().create_model_editor();
	ed->Show( name(), item->full_path_name() );
}

} // namespace editor
} // namespace xray
