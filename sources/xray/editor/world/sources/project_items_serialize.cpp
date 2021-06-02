////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_items.h"
#include "object_base.h"
#include "project.h"
#include "tool_base.h"
#include "tool_scripting.h"
#include "lua_config_value_editor.h"
#include "object_composite.h"
#include "object_scene.h"
#include "object_job.h"

namespace xray{
namespace editor{

void project_item_base::load_base(	configs::lua_config_value const& cfg, 
									bool load_ids )
{
	if(load_ids)
		assign_id		( cfg["id"] );
	else
		assign_id		( 0 );
}

void project_item_base::save( configs::lua_config_value& cfg, bool save_ids, bool save_guid )
{
	XRAY_UNREFERENCED_PARAMETERS( save_guid );
	if(save_ids)
		cfg["id"] = id();

//	if(save_guid)
//		set_c_string(cfg["_guid"], m_guid.ToString());
}

void project_item_object::save( configs::lua_config_value& cfg, bool save_ids, bool save_guid )
{
	configs::lua_config_value current = cfg["objects"][unmanaged_string(m_guid.ToString()).c_str()];
	super::save				( current, save_ids, save_guid );
	m_object_base->save		( current );
}

void project_item_object::load( configs::lua_config_value const& root, 
								pcstr table_id,
								string_list^ loaded_names, 
								item_id_list^ loaded_ids, 
								bool load_ids,
								bool load_guid )
{
	XRAY_UNREFERENCED_PARAMETERS( loaded_names, loaded_ids );
	configs::lua_config_value current	= root["objects"][table_id];

	ASSERT							(m_object_base==nullptr, "The item allready has an object");

	super::load_base				( current, load_ids );

	if( load_guid )
	{
		System::String^ guid_str	= gcnew System::String ( table_id );
		m_guid						= System::Guid(guid_str);
	}else
	{
		m_guid						= System::Guid::NewGuid();
	}

	m_object_base					= create_object( m_project->get_level_editor(), current );

	m_object_base->owner_project_item	= this;
}

void project_item_object::load_content( configs::lua_config_value const& root, 
										pcstr table_id,
										string_list^ loaded_names, 
										item_id_list^ loaded_ids, 
										bool load_ids,
										bool load_guid,
										bool fill_tree )
{
	XRAY_UNREFERENCED_PARAMETERS(fill_tree);

	object_composite^ oc = dynamic_cast<object_composite^>(m_object_base);

	if ( oc && oc->is_edit_mode() )
	{
		object_composite^ oc = safe_cast<object_composite^>(m_object_base);

		R_ASSERT( strings::compare( root["objects"][table_id]["type"], "object") == 0 );

		project_item_object^ item	= gcnew project_item_object( m_project );
		item->load					( root, table_id, loaded_names, loaded_ids, load_ids, load_guid );

		oc->edit_add_object			( item->get_object() );
		
		if( loaded_names )
			loaded_names->Add	( item->get_full_name() );

		if(loaded_ids)
			loaded_ids->Add		( item->id() );

	}else if ( m_object_base->GetType() == object_scene::typeid )
	{
		configs::lua_config_value const& current = root["objects"][table_id];
		project_item_object^ item	= ( safe_cast< object_scene^ >( m_object_base )->add_child_from_cfg( System::Guid::NewGuid().ToString(), current ))->owner_project_item;

		item->fill_tree_view				( m_object_base->owner_project_item->m_tree_node_->nodes );

		if( loaded_names )
			loaded_names->Add	( item->get_full_name() );

		if(loaded_ids)
			loaded_ids->Add		( item->id() );
	}else
	{
		if( m_parent->get_object() )
		{
			project_item_object^ po		= safe_cast<project_item_object^>(m_parent);
			po->load_content			( root, table_id, loaded_names, loaded_ids, load_ids, load_guid, true );
		}else
		{
			configs::lua_config_value const& entrypoints	= root["entrypoint"];
			project_item_folder^ pg		= safe_cast<project_item_folder^>(m_parent);
			pg->load_content			( root, entrypoints, table_id, loaded_names, loaded_ids, load_ids, load_guid, true );
		}
	}	
}

void project_item_folder::save( configs::lua_config_value& cfg, bool save_ids, bool save_guid )
{
	System::String^ table = m_guid.ToString();

	configs::lua_config_value current = cfg["folders"][unmanaged_string(table).c_str()];

	super::save			( current, save_ids, save_guid );
	set_c_string		( current["name"], m_name );

	if(m_pivot!=NULL)
		current["pivot"] = *m_pivot;

	current["lock"] = m_locked;

	for each (project_item_folder^ pg in m_folders)
	{
		current["subitems"][unmanaged_string(pg->m_guid.ToString()).c_str()]	= "folder";
		pg->save				( cfg, save_ids, save_guid );
	}

	for each (project_item_object^ pi in m_objects)
	{
		current["subitems"][unmanaged_string(pi->m_guid.ToString()).c_str()]	= "object";
		pi->save				( cfg, save_ids, save_guid );
	}
}

void  project_item_folder::load( configs::lua_config_value const& root, 
								pcstr table_id,
								string_list^ loaded_names, 
								item_id_list^ loaded_ids, 
								bool load_ids,
								bool load_guid )
{
	configs::lua_config_value current	= root["folders"][table_id];
	super::load_base			( current, load_ids );

	if( load_guid )
	{
		System::String^ guid_str	= gcnew System::String ( table_id );
		m_guid						= System::Guid(guid_str);
	}else
	{
		m_guid						= System::Guid::NewGuid();
	}

	m_name						= gcnew System::String( current["name"] );
	
	if(current.value_exists("pivot"))
	{
		if(m_pivot==NULL)
			m_pivot = NEW(math::float3)();
		
		*m_pivot = current["pivot"];
	}

	if(current.value_exists("lock"))
		m_locked = current["lock"];

	configs::lua_config_value	subitems				= current["subitems"];
	configs::lua_config_value::const_iterator	it		= subitems.begin( );
	configs::lua_config_value::const_iterator	it_e	= subitems.end( );
	for(; it!=it_e; ++it)
		load_content			( root, subitems, (pcstr)it.key(), loaded_names, loaded_ids, load_ids, load_guid, false );

	if( loaded_names )
		loaded_names->Add( get_full_name() );
	
	if(loaded_ids)
		loaded_ids->Add			( id() );
}

void project_item_folder::load_content(	configs::lua_config_value const& root, 
										configs::lua_config_value const& current,
										pcstr table_id,
										string_list^ loaded_names, 
										item_id_list^ loaded_ids, 
										bool load_ids,
										bool load_guid,
										bool fill_tree )
{
	if( strings::compare( current[table_id], "folder") == 0 )
	{
		project_item_folder^ folder	= gcnew project_item_folder( m_project );
		folder->load				( root, table_id, loaded_names, loaded_ids, load_ids, load_guid );
		add							( folder );
		
		if(fill_tree)
			folder->fill_tree_view( m_tree_node_->nodes );
	}else 
	if( strings::compare( current[table_id], "object") == 0 )
	{
		project_item_object^ item	= gcnew project_item_object( m_project );
		item->load					( root, table_id, loaded_names, loaded_ids, load_ids, load_guid );
		add							( item );
		item->m_object_base->set_visible( true );
		m_project->on_object_load	( item->m_object_base, root["objects"][table_id] );

		if(fill_tree)
			item->fill_tree_view( m_tree_node_->nodes );

		if( loaded_names )
			loaded_names->Add	( item->get_full_name() );
		
		if(loaded_ids)
			loaded_ids->Add		( item->id() );
	}
}

} // namespace editor
} // namespace xray
