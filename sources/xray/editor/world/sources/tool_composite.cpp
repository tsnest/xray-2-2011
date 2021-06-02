////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_composite.h"
#include "object_composite.h"
#include "level_editor.h"
#include "project.h"
#include "project_items.h"
#include "compound_tool_tab.h"
#include "editor_world.h"
#include "lua_config_value_editor.h"

namespace xray{
namespace editor{

tool_composite::tool_composite( editor::level_editor^ le )
:super(le, "composite")
{
	default_image_index		= xray::editor_base::composite_tree_icon;
	m_tool_tab 				= gcnew compound_tool_tab(this);
	load_library			( );
	m_backup_cfg			= NEW(configs::lua_config_ptr)();
	(*m_backup_cfg)			= configs::create_lua_config( );
}


tool_composite::~tool_composite( )
{
	DELETE				( m_backup_cfg );
}

editor::tool_tab^ tool_composite::ui( )
{
	return m_tool_tab;
}

pcstr tool_composite::get_library_module_name( )
{
	return "library/composite/composite.library";
}

void tool_composite::save_library( )
{
	fs_new::virtual_path_string name	= "resources/";
	name					+= get_library_module_name();
	m_library->save_as		( name.c_str() );
}

void tool_composite::load_library()
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &super::on_library_files_loaded), g_allocator);
	fs_new::virtual_path_string res_name = "resources/";
	res_name					+= get_library_module_name();

	resources::query_resource(
		res_name.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

object_base^ tool_composite::create_library_object_instance( System::String^ name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, &scene_view );

	lib_item^ li				= m_library->get_library_item_by_full_path( name, true );
	object_composite^ result	= gcnew object_composite(this);

	result->load_from_library	( *li->m_config );
	result->set_library_name	( name );
	if(li->m_name!="#empty")
		result->set_name			( li->m_name, false );
	else
		result->set_name			( "from_selection", false );

	return						result;
}

object_base^ tool_composite::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, &scene_view );

	lib_item^ li				= m_library->extract_library_item( t );

	object_composite^ result	= gcnew object_composite(this);

	if(li)
	{	
		result->load_from_library	( *li->m_config );
		result->load_props			( t );
		result->set_library_name	( li->full_path_name() );
	}

	return						result;
}

void tool_composite::destroy_object( object_base^ o )
{
	object_composite^ oc		= safe_cast<object_composite^>(o);
	oc->destroy_contents		( );
	delete o;
}

void tool_composite::on_library_loaded( )
{
	super::on_library_loaded	( );
	m_tool_tab->fill_objects_list( );
}

void tool_composite::fill_tree_view( tree_node_collection^ nodes )
{
	nodes->Clear				( );
	m_library->fill_tree		( nodes, this, true, nullptr );
	//xray::editor_base::npc_tree_icon
}

bool tool_composite::valid_library_name( System::String^ s)
{
	if(s==nullptr || s->Length==0)
		return false;
	
	return true;
}

bool tool_composite::commit_compound_object( object_composite^ o )
{
	System::String^ selected	= nullptr;
	if( editor_base::resource_chooser::choose( this->name(), 
												o->get_library_name(), 
												nullptr, 
												selected,
												false)
		)
	{
		lib_item^ li			= m_library->get_library_item_by_full_path( selected, false );
		if(li)
		{
			::DialogResult result = get_level_editor()->ShowMessageBox( "Overwrite '" + selected + "' library item ?",
																		MessageBoxButtons::YesNo,
																		MessageBoxIcon::Exclamation );

			if( result == ::DialogResult::No )
				return			false;
		}else
			li					= m_library->new_lib_item( selected );
		
		(*li->m_config).clear	( );
		o->save_as_library		( *li->m_config );

		set_c_string			( (*li->m_config)["name"], li->full_path_name() );

		on_commit_library_item	( li );
		save_library			( );
		o->set_library_name		( selected );
		on_library_changed		( li->m_guid );
		return					true;
	}
	return false;
}

void tool_composite::get_library_item_details(System::String^ library_item_name, compound_details_list^ dest )
{
	lib_item^ li				= m_library->get_library_item_by_full_path( library_item_name, true );
	
	configs::lua_config_iterator it		= (*li->m_config)["objects"].begin();
	configs::lua_config_iterator it_e	= (*li->m_config)["objects"].end();
	for(; it!=it_e; ++it)
	{
		configs::lua_config_value t = *it;
		dest->Add( gcnew System::String((pcstr)t["name"]) );
		dest->Add( gcnew System::String((pcstr)t["tool_name"]) );
	}
}

void tool_composite::current_edit_object::set( object_composite^ o )
{
	if(m_current_edit_object)
	{ // save changes to current edit object confirmation
		m_current_edit_object->switch_to_edit_mode( false );
	}

	m_current_edit_object						= o;
	
	if(m_current_edit_object)
	{
		m_current_edit_object->switch_to_edit_mode	( true );

		configs::lua_config_value t = (*m_backup_cfg)->get_root()["backup"];
		t.clear						( );
		m_current_edit_object->save	( t );
	}
}

void tool_composite::on_library_changed( System::Guid lib_guid )
{
	project_items_list^ all_objects = m_level_editor->get_project()->get_all_items();

	for each( project_item_base^ o in all_objects )
	{
		if(nullptr==o->get_object() || o->get_object()->owner_tool()!=this)
			continue;

		object_composite^ oc = safe_cast<object_composite^>(o->get_object());
		if( oc!=m_current_edit_object && oc->get_library_guid()==lib_guid )
			oc->re_load_lib( );
	}
}

void tool_composite::undo_current_edit_object( )
{
	R_ASSERT						( m_current_edit_object!=nullptr );
	object_composite^ o				= m_current_edit_object;
	current_edit_object				= nullptr;

	configs::lua_config_value t		= (*m_backup_cfg)->get_root()["backup"];
	R_ASSERT						( t.size()!=0 );
	o->re_load_lib					( );
	o->load_props					( t );
	t.clear							( );
}

bool tool_composite::has_uncommited_changes( System::String^% reason )
{
	if(current_edit_object!=nullptr)
	{
		reason += "\n tool [Composite] have current edit object";
		return true;
	}else
		return false;
}

void tool_composite::on_project_clear( )
{
	m_tool_tab->on_project_clear();
}

} // namespace editor
} // namespace xray
