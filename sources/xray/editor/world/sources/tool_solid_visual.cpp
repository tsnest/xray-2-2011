////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_solid_visual.h"
#include "object_solid_visual.h"
#include "object_skeleton_visual.h"
#include "solid_visual_tool_tab.h"
#include "level_editor.h"
#include "editor_world.h"

namespace xray{
namespace editor{

pcstr static_model_ext		= ".model";
pcstr skinned_model_ext		= ".skinned_model";
pcstr composite_model_ext	= ".composite_model";

int const static_model_type		= 1;
int const skinned_model_type	= 2;
int const composite_model_type	= 3;

tool_solid_visual::tool_solid_visual( level_editor^ le )
:super						( le, "solid_visual" ),
m_models_loaded				( false ),
m_composite_models_loaded	( false )
{
	default_image_index	= xray::editor_base::node_mesh;
	m_tool_tab 			= gcnew solid_visual_tool_tab(this);
}

tool_solid_visual::~tool_solid_visual( )
{
}

editor::tool_tab^ tool_solid_visual::ui( )
{
	return m_tool_tab;
}

void tool_solid_visual::initialize( )
{
	load_library();
};

void tool_solid_visual::load_library( )
{
	fs_iterator_delegate* q = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &tool_solid_visual::on_models_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	"resources/models", 
									boost::bind(&fs_iterator_delegate::callback, q, _1), 
									g_allocator, 
									resources::recursive_true);

	fs_iterator_delegate* q2 = NEW(fs_iterator_delegate)(gcnew fs_iterator_delegate::Delegate(this, &tool_solid_visual::on_composite_models_fs_iterator_ready), g_allocator);
	resources::query_vfs_iterator(	"resources/composite_models", 
									boost::bind(&fs_iterator_delegate::callback, q2, _1), 
									g_allocator, 
									resources::recursive_true);
}

object_base^ tool_solid_visual::create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );

	return create_raw_object(name, scene);
}

object_base^ tool_solid_visual::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	System::String^ library_name	= gcnew System::String(t["lib_name"]);
	object_base^ result				= create_library_object_instance(library_name, scene, scene_view);
	result->load_props				( t );
	return result;
}


object_base^ tool_solid_visual::create_raw_object( System::String^ id, render::scene_ptr const& scene )
{
	lib_item^ li = m_library->get_library_item_by_full_path( id, false );

	object_base^ base = nullptr;
	
	bool is_static = (li==nullptr) || li->m_item_type!=skinned_model_type;

	if( !is_static )
	{
		base					=  gcnew object_skeleton_visual(this, scene);
	}else
	{
		base					= gcnew object_solid_visual(this, scene);
	}

	base->set_library_name		( id );

	int del_ind					= id->LastIndexOf("/");
	if( del_ind > 0 )
	{
		R_ASSERT(" how do we raise it ??");
		base->set_name			(id->Substring(del_ind+1), true);
	}else
		base->set_name			(id, true);

	return		base;
}

void tool_solid_visual::destroy_object( object_base^ o )
{
	if(o->get_visible())
		o->set_visible(false);

	delete o;
}

int get_image_index( int const item_type )
{
	switch(item_type)
	{
	case static_model_type:
		return xray::editor_base::node_mesh;
		break;
	case skinned_model_type:
		return xray::editor_base::node_joint;
		break;
	case composite_model_type:
		return xray::editor_base::composite_tree_icon;
		break;
	default:
		NODEFAULT();
		return 0;
	}
}

void tool_solid_visual::process_recursive_names( vfs::vfs_iterator const & in_it, 
												System::String^	path/*, 
												System::String^ tag,
												int image_index*/)
{
	vfs::vfs_iterator it				= in_it;

	while( !it.is_end() )
	{
		pcstr p = it.get_name	( );

		u32 src_len							= strings::length(p);
		int item_type = 0;
		if(strings::ends_with( p, src_len, static_model_ext, strings::length(static_model_ext) ) )
			item_type = static_model_type;
		else
		if(strings::ends_with( p, src_len, skinned_model_ext, strings::length(skinned_model_ext) ) )
			item_type = skinned_model_type;
		else
		if(strings::ends_with( p, src_len, composite_model_ext, strings::length(composite_model_ext) ) )
			item_type = composite_model_type;

		if( it.is_folder() || item_type!=0 )
		{
			System::String^ name			= gcnew System::String(it.get_name());
			System::String^ full_path		= path;
			
			if(full_path->Length)
				full_path					+= "/";
			
			if( item_type!=0 )
 			{
				if(	!it.is_expanded() || 
					it.is_archive_check_overlapped() || 
					it.find_child("composite_render") ||
					it.find_child("render"))
				{
					name					= name->Remove(name->LastIndexOf('.'));
					full_path				+= name;
					lib_item^ itm			= m_library->add_simple( full_path );
					itm->m_item_type		= item_type;
					itm->m_image_index		= get_image_index( item_type );
				}
			}else
			if(it.is_folder())
			{
				full_path						+= name;
				process_recursive_names			( it.children_begin(), full_path );
			}
		}
		++it;
	}
}


void tool_solid_visual::on_models_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names			( fs_it.children_begin(), "" );
	m_models_loaded					= true;
	
	if( m_composite_models_loaded )
	{
		m_tool_tab->fill_objects_list( get_selected_library_name() );
		m_b_ready					= true;
		m_level_editor->tool_loaded	( this );
	}
}

void tool_solid_visual::on_composite_models_fs_iterator_ready( vfs::vfs_locked_iterator const & fs_it )
{
	process_recursive_names			( fs_it.children_begin(), "" );
	m_composite_models_loaded		= true;
	
	if( m_models_loaded )
	{
		m_tool_tab->fill_objects_list( get_selected_library_name() );
		m_b_ready					= true;
		m_level_editor->tool_loaded	( this );
	}
}

void tool_solid_visual::item_double_clicked( lib_item_base^ item )
{
	lib_item^ li = safe_cast<lib_item^>(item);
	xray::editor_base::tool_window^ ed = get_level_editor()->get_editor_world().create_model_editor();

	System::String^ item_type_str = item_type_to_str(li->m_item_type);

	ed->Show( item_type_str, item->full_path_name() );
}

System::String^  tool_solid_visual::item_type_to_str( int const item_type )
{
	if(item_type==static_model_type)
		return "solid_visual";
	else
	if(item_type==skinned_model_type)
		return "skinned_visual";
	else
	if(item_type==composite_model_type)
		return "composite_visual";
	else
	{
		UNREACHABLE_CODE();	
		return nullptr;
	}
}

} // namespace editor
} // namespace xray
