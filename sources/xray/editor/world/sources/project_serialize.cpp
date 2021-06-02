////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project.h"
#include "level_editor.h"
#include "project_tab.h"
#include "editor_world.h"
#include "object_base.h"
#include "object_scene.h"
#include "window_ide.h"

#pragma managed( push, off )
#	include <xray/core_entry_point.h>
#	include <xray/fs/device_utils.h>
#	include <xray/ai_navigation/sources/graph_generator.h>
#	include <xray/ai_navigation/world.h>
#pragma managed( pop )

namespace xray{
namespace editor{

void project::load_project( System::String^ project_name )
{
	R_ASSERT				(empty());

	m_root_folder			= gcnew project_item_folder( this );

	m_project_name			= project_name;
	m_root_folder->set_name	( System::IO::Path::GetFileNameWithoutExtension(project_name) );

	setup_intermediate_path	( gcnew execute_delegate( this, &project::load_project_impl ));

	set_unchanged			( );
}

void project::set_unchanged		( )
{
	if ( m_changed )
		m_level_editor->ide()->Text		= m_project_name + " - X-Ray Editor" ;
	m_changed				= false;
}

void project::load_project_impl( )
{
	m_project_loading_timer			= NEW(timing::timer)();
	m_project_loading_timer->start	( );

	fs_new::virtual_path_string pth;
	pth.assignf				("resources/projects/%s/project.xprj", unmanaged_string(m_project_name).c_str());

	m_level_editor->get_editor_world().get_ai_navigation_world().load_navmesh	( unmanaged_string(m_project_name).c_str() );

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &project::on_project_config_loaded), g_allocator);

	resources::query_resource(
		pth.c_str(), 
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator );
}

struct wait_helper
{
	wait_helper(resources::mount_ptr * out_ptr, execute_delegate^ d): m_out_ptr(out_ptr), m_execute_on_complete(d){}

	void mount_cb( resources::mount_ptr result )
	{
		* m_out_ptr				= result;
		if(m_execute_on_complete)
		{
			execute_delegate^ d = m_execute_on_complete;
			d();
		}
		DELETE (this);
	}
	resources::mount_ptr *		m_out_ptr;
	gcroot<execute_delegate^>	m_execute_on_complete;
};

void project::setup_intermediate_path( execute_delegate^ execute_on_complete )
{
	pcstr udp = core::user_data_directory( );
	
	m_project_intermediate_path = System::String::Format("{0}/editor_runtime/{1}_{2}",
		gcnew System::String(udp), 
		project_name(),
		gcnew System::String(session_id()) );

	unmanaged_string intermediate_path(m_project_intermediate_path);
	fs_new::synchronous_device_interface const & device	=	resources::get_synchronous_device();
	fs_new::create_folder_r		( device, intermediate_path.c_str(), true );
	fs_new::virtual_path_string				dest_path;
	dest_path.assignf_with_conversion	("resources/projects/%s", unmanaged_string(project_name()).c_str());

	wait_helper* helper = NEW(wait_helper)( m_project_mount, execute_on_complete );

	resources::query_mount_physical(dest_path.c_str(), 
								    fs_new::native_path_string::convert(intermediate_path.c_str()), 
									"project-mount",
									fs_new::watcher_enabled_true, 
									boost::bind(&wait_helper::mount_cb, helper, _1),
									g_allocator,
									resources::recursive_true);
	//while(!helper.b_complete)
	//{
	//	if ( threading::g_debug_single_thread )
	//		resources::tick ();
	//	threading::yield(10);
	//	resources::dispatch_callbacks();
	//}
}

void project::load_project_config( configs::lua_config_value const& cfg )
{
	if(!cfg["camera"].empty())
	{
		float3 p	=	cfg["camera"]["position"];
		float3 d	=	cfg["camera"]["direction"];
		
		m_level_editor->view_window()->set_camera_props(p, d);
		
		if( cfg["camera"].value_exists("distance") )
		{
			float dist	=	cfg["camera"]["distance"];
			m_level_editor->view_window()->set_view_point_distance( dist );
		}
	}

	configs::lua_config_iterator bk_it		= cfg["map_backgrounds"].begin();
	configs::lua_config_iterator bk_it_e	= cfg["map_backgrounds"].end();

	for(; bk_it!=bk_it_e; ++bk_it)
	{
		configs::lua_config_value t		= *bk_it;
		
		project_background_item^ bkg	= gcnew project_background_item;
		bkg->m_texture_name				= gcnew System::String((pcstr)t["texture"]);
		bkg->m_position_lt				= System::Drawing::Point((int)t["px"], (int)t["pz"]);
		if(t.value_exists("scale"))
			bkg->m_scale					= t["scale"];
		else
			bkg->m_scale				= 1;

		m_map_backgrounds.Add			( bkg );
	}

	set_focused_folder					( m_root_folder );

	load_items_config					( cfg, false, true );
}

void project::load_items_config( configs::lua_config_value const& cfg, bool load_ids, bool load_guid )
{
	m_link_tmp_list				= gcnew temporary_link_list;
	m_paste_tmp_list			= gcnew temporary_paste_list;

	m_root_folder->load			(	cfg, 
									"00000000-0000-0000-0000-000000000000", // root
									nullptr, 
									nullptr, 
									load_ids, 
									load_guid );

	//for( int i  = 0; i < m_link_tmp_list->Count; ++i )
	//{
	//	object_base^ src_obj = get_item_by_path( m_link_tmp_list[i]->src_name );

	//	ASSERT( src_obj );
	//	if( src_obj )
	//		m_link_storage->create_link( src_obj, m_link_tmp_list[i]->src_member, m_link_tmp_list[i]->dst_obj, m_link_tmp_list[i]->dst_member );
	//}

	delete						m_link_tmp_list;
	m_link_tmp_list				= nullptr;

	delete						m_paste_tmp_list;
	m_paste_tmp_list			= nullptr;

	fill_tree_view				( m_project_tab->treeView->nodes );
}

void project::save_project( System::String^ project_name )
{
	m_root_folder->set_name			( System::IO::Path::GetFileNameWithoutExtension(project_name) );
	
	bool bchanged = m_project_name!=project_name;

	m_project_name					= project_name;

	System::String^ path			= "resources/projects/";
	path							+= project_name;

	xray::ai::navigation::graph_generator* graph_generator = get_level_editor()->get_editor_world().get_ai_navigation_world().get_graph_generator();
	graph_generator->save_geometry	( unmanaged_string( path + "/navmesh.lua" ).c_str() );

	path							+= "/project.xprj";

	save_impl						( );

	(*m_config)->save_as			( unmanaged_string(path).c_str(), configs::target_sources );
	
	if(bchanged)
		setup_intermediate_path		( nullptr );
	
	set_unchanged					( );
}

void project::save_intermediate( )
{
	// setup intermediate project path for data
	m_intermediate_save			= true;
	save_impl					( );
	m_intermediate_save			= false;
	// setup restore project path
}

void project::save_impl( )
{
	configs::lua_config_value& cfg		= (*m_config)->get_root();
	cfg.clear							( );
	float3 p, d;
	m_level_editor->view_window()->get_camera_props	( p, d );
	cfg["camera"]["position"]			= p;
	cfg["camera"]["direction"]			= d;
	cfg["camera"]["distance"]			= m_level_editor->view_window()->view_point_distance( );

	configs::lua_config_value t	= cfg["map_backgrounds"];
	u32 idx=0;
	for each (project_background_item^ itm in m_map_backgrounds)
	{
		configs::lua_config_value t_item	=	t[idx];
		t_item["texture"]					= unmanaged_string(itm->m_texture_name).c_str();
		t_item["px"]						= itm->m_position_lt.X;
		t_item["pz"]						= itm->m_position_lt.Y;
		t_item["scale"]						= itm->m_scale;
		++idx;
	}

	if(m_intermediate_save)
		cfg["guid"] = session_id();

//.	configs::lua_config_value objects_tree_table = cfg["project"]["objects_tree"];
	m_root_folder->save			( cfg, false, true ); 

}

void project::on_object_load( object_base^ object, configs::lua_config_value const& t )
{
	XRAY_UNREFERENCED_PARAMETERS( object, t );
	//System::String^ orig_name = gcnew System::String(t["name"]);

	//// This part is needed only for paste operation. So it can be skiped for other cases.
	//paste_temporary_data paste_data;
	//paste_data.object		= object;
	//paste_data.orig_name	= object->m_owner_project_item->get_full_path() + orig_name;
	//m_paste_tmp_list->Add	( %paste_data );

	//configs::lua_config_value const& cfg_links = t["links"];

	//configs::lua_config_value::const_iterator	i = cfg_links.begin( );
	//configs::lua_config_value::const_iterator	e = cfg_links.end( );

	//for ( ; i != e; ++i )
	//{
	//	configs::lua_config_value const& t	= *i;
	//	System::String^ src_full_name			= gcnew System::String(t["src_name"]);

	//	link_temporary_data^ tmp_link = gcnew link_temporary_data();
	//	tmp_link->src_name		= src_full_name;
	//	tmp_link->src_member	= gcnew System::String(t["src_member"]);
	//	tmp_link->dst_obj		= object;
	//	tmp_link->dst_member	= gcnew System::String(t["member"]);
	//	m_link_tmp_list->Add(tmp_link);

	//	//m_link_storage->create_link( src_obj, gcnew System::String(t["src_member"]), object, gcnew System::String(t["member"]) );
	//}
}

void project::on_object_save( object_base^ object, configs::lua_config_value const& t)
{
	XRAY_UNREFERENCED_PARAMETERS( object, t );
	//configs::lua_config_value cfg_links = t["links"];

	//System::Collections::ArrayList^ links = m_link_storage->get_links( nullptr, nullptr, object, nullptr );
	//for( int i = 0; i < links->Count; ++i )
	//{
	//	System::String^ path = ((property_link^)links[i])->m_src_object->m_owner_project_item->get_full_name();
	//	unmanaged_string path_str_src(path);
	//	cfg_links[i]["src_name"] = path_str_src.c_str();

	//	unmanaged_string src_property_name(((property_link^)links[i])->m_src_property_name);
	//	cfg_links[i]["src_member"] = src_property_name.c_str();

	//	unmanaged_string dst_property_name(((property_link^)links[i])->m_dst_property_name);
	//	cfg_links[i]["member"] = dst_property_name.c_str();
	//}
}

void load_items_list(	project^ p, 
						configs::lua_config_value& root, 
						string_list^ loaded_names,
						item_id_list^ loaded_ids,
						bool use_absolute_path,
						bool load_ids,
						bool load_guid )
{
	
	

	configs::lua_config_value const& entrypoints	= root["entrypoint"];

	configs::lua_config_value::const_iterator	it = entrypoints.begin( );
	configs::lua_config_value::const_iterator	it_e = entrypoints.end( );

	for ( ;it != it_e; ++it )
	{
		pcstr guid						= (pcstr)it.key();

		pcstr tool_name = (pcstr)root["objects"][guid]["tool_name"];
		bool pasted_object_is_logic_object = gcnew System::String(tool_name) == "scripting";
		project_item_base^ selected_item = ( p->selection_list()->Count != 0 ) ? p->selection_list()[0] : nullptr;
		
		if ( selected_item == nullptr )
		{
			if ( pasted_object_is_logic_object )
				selected_item = p->root_scene;
			else
				selected_item = p->root();			
		}
		else
		{
			bool pasted_to_object_is_logic_object = selected_item->get_object() != nullptr && dynamic_cast< object_logic^ >(selected_item->get_object() ) != nullptr;
			if ( pasted_object_is_logic_object )
			{
				if ( !pasted_to_object_is_logic_object )
					selected_item = p->root_scene;
			}
			else
			{
				if ( pasted_to_object_is_logic_object )
				{
					selected_item = p->root();
				}
			}
		}


// 		if ( pasted_object_is_logic_object )
// 		{
// 			selected_item				=	(	selected_item != nullptr && 
// 												selected_item->get_object() != nullptr && 
// 												dynamic_cast< object_logic^ >(selected_item->get_object() ) != nullptr
// 											) ? selected_item : p->root_scene;		
// 		}
// 		else
// 		{
// 			if ( selected_item == nullptr )
// 			{
// 				selected_item = p->root();
// 			}
// 			else if ( selected_item->get_object() == nullptr )
// 			{
// 
// 			}
// 			else if ( dynamic_cast< object_logic^ >(selected_item->get_object() ) != nullptr )
// 			{
// 				selected_item = p->root();
// 			}
			//selected_item				= ( selected_item != nullptr && ( selected_item->get_object() == nullptr || selected_item->get_object()->GetType() != object_logic::typeid ) ) ? selected_item : p->root();
		//}

		project_item_base^ parent		= selected_item;
		
		if( use_absolute_path )
		{
			System::String^	path		= gcnew System::String( root["full_path"][guid] );// if deleted
			parent						=  p->find_by_path( path, true );
		}

		if(parent->get_object())
		{
			project_item_object^ po		= safe_cast<project_item_object^>(parent);
			po->load_content			( root, guid, loaded_names, loaded_ids, load_ids, load_guid, true );
		}else
		{
			project_item_folder^ pg		= safe_cast<project_item_folder^>(parent);
			pg->load_content			( root, entrypoints, guid, loaded_names, loaded_ids, load_ids, load_guid, true );
		}
	}
}

void project::load_settings( )
{
	RegistryKey^ product_key	= base_registry_key::get();
	RegistryKey^ editor_key 	= get_sub_key(product_key, "project");

	track_active_item			= System::Convert::ToBoolean(editor_key->GetValue("track_active_item", true));

	editor_key->Close			();
	product_key->Close			();
}

void project::save_settings( )
{
	RegistryKey^ product_key	= base_registry_key::get();
	RegistryKey^ editor_key		= get_sub_key(product_key, "project" );

	editor_key->SetValue		( "track_active_item", track_active_item );

	editor_key->Close			();
	product_key->Close			();
}

} // namespace editor
} // namespace xray
