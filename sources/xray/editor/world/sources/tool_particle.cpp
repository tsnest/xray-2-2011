////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_particle.h"
#include "object_particle.h"

#include "solid_visual_tool_tab.h"
#include "level_editor.h"

namespace xray{
namespace editor{

tool_particle::tool_particle( level_editor^ le )
:super(le, "particle")
{
	default_image_index		= xray::editor_base::particle_tree_icon;
	m_tool_tab 				= gcnew solid_visual_tool_tab(this);
}

tool_particle::~tool_particle( )
{
}

void tool_particle::initialize( )
{
	load_library();
};

editor::tool_tab^ tool_particle::ui( )
{
	return m_tool_tab;
}

void tool_particle::load_library( )
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &tool_particle::on_library_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	"resources/particles", 
		boost::bind(&fs_iterator_delegate::callback, q, _1), g_allocator, xray::resources::recursive_true);
}

object_base^ tool_particle::create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER		( scene_view );

	return create_raw_object		( name, scene );
}

object_base^ tool_particle::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	System::String^ library_name	= gcnew System::String(t["lib_name"]);
	object_base^ result				= create_library_object_instance( library_name, scene, scene_view );
	result->load_props				( t );
	return result;
}

object_base^ tool_particle::create_raw_object( System::String^ id, render::scene_ptr const& scene )
{
	object_particle^ result			=  gcnew object_particle( this, scene );

	int del_ind						= id->LastIndexOf("/");
	if( del_ind > 0 )
		result->set_name			( id->Substring(del_ind+1), true );
	else
		result->set_name			( id, true);

	result->set_library_name		( id );

	return							result;
}

void tool_particle::destroy_object( object_base^ o )
{
	if(o->get_visible())
		o->set_visible(false);

	delete o;
}

void tool_particle::process_recursive_names( vfs::vfs_iterator const & in_it, System::String^	path )
{
	vfs::vfs_iterator it				= in_it;
	while( !it.is_end() )
	{
		pcstr model_name_prefix			= ".particle";
		int model_name_prefix_len		= strings::length(model_name_prefix);
		System::String^ name			= gcnew System::String(it.get_name());
		System::String^ full_name		= path;
		
		if(full_name->Length)
			full_name					+= "/";
		
		pcstr found						= strstr(it.get_name(), model_name_prefix);
		
		if(found && xray::strings::compare(found, model_name_prefix)==0)
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


void tool_particle::on_library_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names	( fs_it.children_begin(), "" );
	m_tool_tab->fill_objects_list( get_selected_library_name() );
	
	m_b_ready					= true;
	m_level_editor->tool_loaded	( this );

}

} // namespace editor
} // namespace xray
