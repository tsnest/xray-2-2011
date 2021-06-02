////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_cooker.h"
#include "cell_manager.h"
#include <xray/fs/device_utils.h>

namespace stalker2 {

pcstr editor_project_ext		= ".xprj";
pcstr resources_converted_path	= "resources.converted/";
pcstr resources_path			= "resources/";

project_cooker::project_cooker( bool editor_present)
:super				( resources::game_project_class, reuse_true, use_any_thread_id ),
m_editor_present	( editor_present )
{}

void project_cooker::translate_query( resources::query_result_for_cook& parent )
{
	fs_new::virtual_path_string	project_name	= parent.get_requested_path();
	
#ifndef MASTER_GOLD	
	if(project_name[0]=='#' )
	{// given name is editor project config, load it directly
		resources::query_resource(
									project_name.c_str()+1,// skip first '#'
									resources::lua_config_class,
									boost::bind(&project_cooker::on_editor_project_loaded, this, _1, &parent ),
									g_allocator,
									0,
									& parent
								);
		return;
	}

	// compare source and converted resource file age
	fs_new::virtual_path_string		editor_proj_path;
	editor_proj_path.assignf		("%sprojects/%s/project.xprj", resources_path, project_name.c_str() );

	fs_new::virtual_path_string		game_proj_path;
	game_proj_path.assignf			("%sprojects/%s/project", resources_converted_path, project_name.c_str() );

	resources::request r[] ={
		{ editor_proj_path.c_str(),		resources::fs_iterator_class },
		{ game_proj_path.c_str(),		resources::fs_iterator_class },
	};
	resources::query_resources(
		r,
		boost::bind( &project_cooker::on_fs_iterators_ready,this, _1, &parent),
		g_allocator,
		NULL,
		&parent
		);
#else // ifndef MASTER_GOLD

// directly load game project
	fs_new::virtual_path_string			game_proj_path;
	game_proj_path.assignf			("%sprojects/%s/project", resources_converted_path, project_name.c_str() );

	resources::query_resource(
								game_proj_path.c_str(),
								resources::config_class,
								boost::bind(&project_cooker::on_game_project_loaded, this, _1, &parent ),
								g_allocator,
								0,
								& parent
								);
#endif
}

#ifndef MASTER_GOLD
void project_cooker::on_fs_iterators_ready( resources::queries_result& data, 
											resources::query_result_for_cook* parent )
{
	R_ASSERT						( data.is_successful() );
	// compare source and converted resource file age
	vfs::vfs_iterator it_editor_project		= data[0].get_result_iterator();
	vfs::vfs_iterator it_game_project		= data[1].get_result_iterator();

	fs_new::physical_path_info const& 	editor_project_info	=	resources::get_physical_path_info(it_editor_project);
	fs_new::physical_path_info const&	game_project_info	=	resources::get_physical_path_info(it_game_project);

	if(	!m_editor_present &&
		game_project_info.is_file() && // has converted entry
		editor_project_info.last_time_of_write() < game_project_info.last_time_of_write() )
	{// load game project
		resources::query_resource(	data[1].get_requested_path(),
									resources::config_class,
									boost::bind(&project_cooker::on_game_project_loaded, this, _1, parent ),
									g_allocator,
									0,
									parent
								);
	}else
	{// load xprj
		resources::query_resource(	data[0].get_requested_path(),
									resources::lua_config_class,
									boost::bind(&project_cooker::on_editor_project_loaded, this, _1, parent ),
									g_allocator,
									0,
									parent
								);
	}

}
#endif // #ifndef MASTER_GOLD

void project_cooker::on_game_project_loaded( resources::queries_result& data, 
											resources::query_result_for_cook* parent )
{
	R_ASSERT(data.is_successful());

	resources::query_result_for_user const& result	= data[0];

	configs::binary_config_ptr game_proj_ptr	= static_cast_resource_ptr<configs::binary_config_ptr>( result.get_unmanaged_resource() );

	game_project*	resource		= NEW(game_project)();
	resource->m_config				= game_proj_ptr;
	parent->set_unmanaged_resource	( resource, resources::nocache_memory, sizeof(game_project) );
	parent->finish_query			( result_success );
}



#ifndef MASTER_GOLD
void project_cooker::on_editor_project_loaded( resources::queries_result& data, 
												resources::query_result_for_cook* parent )
{
	R_ASSERT							( data.is_successful() );

	resources::query_result_for_user const& result	= data[0];

	configs::lua_config_ptr editor_proj_ptr	= static_cast_resource_ptr<configs::lua_config_ptr>( result.get_unmanaged_resource() );

	make_game_project					( editor_proj_ptr, parent );
}

typedef associative_vector<cell_key, cell_cooker_user_data*, vector> user_data_cont;

cell_cooker_user_data* get_cell(	user_data_cont& container, 
									cell_key const& k, 
									configs::lua_config_value& cells_root,
									u32& cell_idx,
									fs_new::virtual_path_string const& cell_prefix)
{
	cell_cooker_user_data* result = NULL;


	user_data_cont::iterator it = container.find( k );

	if( it!=container.end() )
	{ // insert new
		result	= it->second;
	}else
	{
		result				= NEW(cell_cooker_user_data)();
		result->key			= k;
		container[k]		= result;
		fixed_string<128>	buff;
		buff.assignf		("%s/%s.cell", cell_prefix.c_str(), k.to_string().c_str());
		result->config		= configs::create_lua_config	( buff.c_str() );
		cells_root[cell_idx] = buff.c_str					( );
		++cell_idx;
	}
	return result;
}


void process_patrol_path_behaviour( configs::lua_config_value const& patrol_config,
								   configs::lua_config_value& result_behaviours_config,
								   configs::lua_config_value& source_patrol_behaviour_config)
{
	configs::lua_config_value brain_behaviour_template = source_patrol_behaviour_config["behaviour_config"].copy( );

	configs::lua_config_iterator patrol_nodes_it	= patrol_config["graph"]["nodes"].begin();
	configs::lua_config_iterator patrol_nodes_it_e	= patrol_config["graph"]["nodes"].end();

	int index = 1;

	for ( ; patrol_nodes_it != patrol_nodes_it_e; ++patrol_nodes_it )
	{
		fs_new::virtual_path_string behaviour_name		= source_patrol_behaviour_config.get_field_id( );
		behaviour_name.appendf("point%d", index );

		configs::lua_config_value current_behaviour_config = result_behaviours_config[ behaviour_name.c_str( )].create_table();
		
		current_behaviour_config["behaviour_type"] = "npc_reach_location";		

		current_behaviour_config["behaviour_config"].assign_lua_value( brain_behaviour_template.copy( ) );

		current_behaviour_config["behaviour_config"]["goals"][0]["filter_sets"][0]["parameter0_filter"][0]["positions"][0]["target_point"].assign_lua_value( (*patrol_nodes_it)["position"] );

		current_behaviour_config["events"].create_table( );

		if ( !source_patrol_behaviour_config["events"].empty( ) )
		{
			current_behaviour_config["events"].assign_lua_value( source_patrol_behaviour_config["events"].copy() );
		}

		fixed_string<128> generated_event_handler_name = "event";
		generated_event_handler_name.appendf("%d", current_behaviour_config["events"].size() );

		bool need_to_switch_by_signal = false;

		if ( (*patrol_nodes_it).value_exists("signal") )
		{
			configs::lua_config_value event_handlers_config = current_behaviour_config["events"][(pcstr)generated_event_handler_name.c_str()];
			event_handlers_config["event_name"] = "on_location_reached";
			event_handlers_config["is_global"] = false;

			fixed_string<128> action_name = "action";
			action_name.appendf("%d", event_handlers_config["actions"].size( ) );

			configs::lua_config_value action_config = event_handlers_config["actions"][(pcstr)action_name.c_str()];
			action_config["action_type"]	= "raise_event";

			pcstr signal_name = (pcstr)(*patrol_nodes_it)["signal"];

			action_config["raised_event"]	= signal_name;
			action_config["is_global"]		= false;

			configs::lua_config_iterator source_event_handlers_it	= current_behaviour_config["events"].begin();
			configs::lua_config_iterator source_event_handlers_it_e	= current_behaviour_config["events"].end();
			for ( ; source_event_handlers_it != source_event_handlers_it_e; ++source_event_handlers_it )
			{
				configs::lua_config_value current_source_event_handler = *source_event_handlers_it;
				if ( strings::equal( current_source_event_handler["event_name"], signal_name ) )
				{
					configs::lua_config_iterator event_handler_actions_it	= current_source_event_handler["actions"].begin();
					configs::lua_config_iterator event_handler_actions_it_e	= current_source_event_handler["actions"].end();

					for ( ; event_handler_actions_it != event_handler_actions_it_e; ++event_handler_actions_it )
					{
						configs::lua_config_value current_action = *event_handler_actions_it;	
						if ( strings::equal( current_action["action_type"], "switch_to_behaviour" ) )
						{
							need_to_switch_by_signal = true;
							break;
						}						
					}
				}
				if ( need_to_switch_by_signal )
					break;
			}			
		}		

		if ( (*patrol_nodes_it).value_exists("edges") && !need_to_switch_by_signal )
		{
			configs::lua_config_value event_handlers_config = current_behaviour_config["events"][(pcstr)generated_event_handler_name.c_str()];
			event_handlers_config["event_name"] = "on_location_reached";
			event_handlers_config["is_global"] = false;
			
			fixed_string<128> action_name = "action";
			action_name.appendf("%d", event_handlers_config["actions"].size( ) );

			configs::lua_config_value action_config = event_handlers_config["actions"][(pcstr)action_name.c_str()];
			action_config["action_type"] = "filtered_switch_to_behaviour";
			
			configs::lua_config_iterator patrol_edges_it	= (*patrol_nodes_it)["edges"].begin();
			configs::lua_config_iterator patrol_edges_it_e	= (*patrol_nodes_it)["edges"].end();

			for ( ; patrol_edges_it != patrol_edges_it_e; ++patrol_edges_it )
			{
				fs_new::virtual_path_string dest_behaviour_name		= source_patrol_behaviour_config.get_field_id( );
				dest_behaviour_name.appendf("point%d", (int)( *patrol_edges_it )["destination_node_index"] + 1 );
				action_config["behaviours"][dest_behaviour_name.c_str()] = (int)( *patrol_edges_it )["probability"];
			}
		}
		index++;
	}

	source_patrol_behaviour_config.clear();
	source_patrol_behaviour_config["behaviour_type"] = "npc_patrol";
	source_patrol_behaviour_config["events"].create_table( );

	configs::lua_config_value event_handlers_config = source_patrol_behaviour_config["events"]["event0"];
	event_handlers_config["event_name"] = "on_started";
	event_handlers_config["is_global"] = false;

	configs::lua_config_value action_config = event_handlers_config["actions"]["action0"];
	action_config["action_type"]	= "switch_to_behaviour";

	fs_new::virtual_path_string start_behaviour_name		= source_patrol_behaviour_config.get_field_id( );
	start_behaviour_name.append( "point1" );

	action_config["switching_to_behaviour"]		= start_behaviour_name.c_str( );

	//result_behaviours_config.save_as("d:/aas", configs::target_default );
}

void process_scene( pcstr folder_name,
					configs::lua_config_value const& t_current,
					configs::lua_config_value const& t_objects,
					configs::lua_config_value& t_scenes )
{

	configs::lua_config_iterator jobs_it	= t_current["jobs"].begin();
	configs::lua_config_iterator jobs_it_e	= t_current["jobs"].end();

	for( ; jobs_it!=jobs_it_e; ++jobs_it )
	{
		if (  strings::equal( (*jobs_it)["job_type"], "npc" ) )
		{
			configs::lua_config_value current_job_behaviours = (*jobs_it)["behaviours"];
			configs::lua_config_value current_job_behaviours_copy = current_job_behaviours.copy( );
			configs::lua_config_value& behaviours_config = current_job_behaviours_copy;

			configs::lua_config_iterator behaviours_it		= current_job_behaviours.begin();
			configs::lua_config_iterator behaviours_it_e	= current_job_behaviours.end();

			for( ;behaviours_it!=behaviours_it_e; ++behaviours_it )
			{
				pcstr behaviour_type = (pcstr)(*behaviours_it)["behaviour_type"];
				//pcstr behaviour_name = (pcstr)behaviours_it.key( );
				if ( strings::equal( behaviour_type, "npc_patrol") )
				{
					configs::lua_config_value patrol_graph_config = t_objects[ (pcstr)(*behaviours_it)["patrol_graph_guid"] ];
					configs::lua_config_value patrol_behaviour_config = behaviours_config[ (pcstr)behaviours_it.key( )];
					process_patrol_path_behaviour( patrol_graph_config, behaviours_config, patrol_behaviour_config );					
				}
			}	
			current_job_behaviours.assign_lua_value( behaviours_config );			
		}
	}
	// write self
	int count						= t_scenes.size();
	t_scenes[count].assign_lua_value( t_current.copy() );
	fs_new::virtual_path_string full_name		= folder_name;
	full_name.append				((pcstr)t_current["name"]);

	t_scenes[count]["full_name"]	= full_name.c_str();
	t_scenes[count]["sub_scenes"].create_table( );
	// iterate sub-scenes
	configs::lua_config_iterator sub_it		= t_current["sub_scenes"].begin();
	configs::lua_config_iterator sub_it_e	= t_current["sub_scenes"].end();

	for(u32 i=0; sub_it!=sub_it_e; ++sub_it,++i)
	{
		pcstr sub_scene_id = *sub_it;
		full_name.append("/");
		process_scene		( full_name.c_str(),
								t_objects[sub_scene_id],
								t_objects,
								t_scenes
							);

		fs_new::virtual_path_string sub_scene_name			= full_name;
		sub_scene_name.append					( (pcstr)t_objects[sub_scene_id]["name"] );

		t_scenes[count]["sub_scenes"][i] = sub_scene_name.c_str();
	}

}

void process_object ( configs::lua_config_value const& current,
					 configs::lua_config_value const& t_objects,
					 configs::lua_config_value& t_scenes,
					 configs::lua_config_value& cells_root, 
					 u32& cell_idx,
					 user_data_cont& tmp_cells, 
					 pcstr cell_prefix,
					 pcstr folder_name)
{
	// not supported in game object(s)
	if(!current.value_exists("game_object_type"))
		return;

	pcstr name					= current["name"];
	
	fs::path_string full_name	= folder_name; 
	full_name.append			( name );
	
	pcstr game_object_type = current["game_object_type"];
	if(strings::equal(game_object_type, "scene"))
	{
		process_scene		( folder_name, current, t_objects, t_scenes );
	}else
	{
		math::aabb object_aabb(math::create_aabb_min_max(current["aabb_min"], current["aabb_max"]));
		cell_key k_min = cell_key::pick( object_aabb.min );
		cell_key k_max = cell_key::pick( object_aabb.max );
	
		ASSERT(k_min.x <= k_max.x && k_min.z <= k_max.z);

		cell_cooker_user_data*		current_cell = NULL;

		for (int kx=k_min.x; kx<=k_max.x; ++kx)
			for (int kz=k_min.z; kz<=k_max.z; ++kz)
			{
				cell_key			k( kx, kz );

				current_cell		= get_cell( tmp_cells, k, cells_root, cell_idx, cell_prefix );

				configs::lua_config_value t_objects = (*current_cell->config)["objects"];
				int count							= t_objects.size();
				t_objects[count].assign_lua_value	( current/*.copy()*/ );

				t_objects[count]["full_name"]		= full_name.c_str();
			}
	}
}

void resolve_job_resources_names( pcstr parent_composite_path, configs::lua_config_value& scene_config )
{
	configs::lua_config_iterator subscenes_it	= scene_config["sub_scenes"].begin( );
	configs::lua_config_iterator subscenes_it_e = scene_config["sub_scenes"].end( );

	for ( ; subscenes_it != subscenes_it_e; ++subscenes_it )
	{
		configs::lua_config_value v = *subscenes_it;
		resolve_job_resources_names( parent_composite_path, v );
	}
	
	configs::lua_config_iterator jobs_it		= scene_config["jobs"].begin( );
	configs::lua_config_iterator jobs_it_e		= scene_config["jobs"].end( );

	for ( ; jobs_it != jobs_it_e; ++jobs_it )
	{
		pcstr partial_job_resource_path = (*jobs_it)["job_resource"];
		if ( strings::length(partial_job_resource_path)!=0 )
		{
			fixed_string1024 job_resource_name = parent_composite_path;
			job_resource_name += partial_job_resource_path;
			(*jobs_it)["job_resource"] = job_resource_name.c_str( );
		}
	}
}

void process_folder( configs::lua_config_value const& t_folders,
						configs::lua_config_value const& t_objects,
						pcstr key,
						configs::lua_config_value& cells_root, 
						u32& cell_idx,
						user_data_cont& tmp_cells, 
						pcstr cell_prefix,
						pcstr folder_name,
						configs::lua_config_value& t_scenes )
{
	configs::lua_config_value const& t_root = t_folders[key];
	configs::lua_config_iterator it			= t_root["subitems"].begin();
	configs::lua_config_iterator it_e		= t_root["subitems"].end();
	
	for(; it!=it_e; ++it)
	{
		//configs::lua_config_value const& t_current	= *it;
		pcstr guid						= it.key();
		pcstr type						= *it;

		if(0==strings::compare(type, "folder"))
		{
			fixed_string1024 sub_folder_name = folder_name;
			sub_folder_name					+= (pcstr)t_folders[guid]["name"];
			sub_folder_name					+= "/";
			

			process_folder	(	t_folders,
								t_objects,
								guid,
								cells_root, 
								cell_idx, 
								tmp_cells, 
								cell_prefix, 
								sub_folder_name.c_str(),
								t_scenes );
		}else
		{ // 
			R_ASSERT(strings::equal(type, "object"));

			configs::lua_config_value const& t_current = t_objects[guid];
			pcstr tool_name			= t_current["tool_name"];

			if(0==strings::compare(tool_name, "terrain"))
			{ // process terrain here
				configs::lua_config_value const& t_nodes	= t_current["nodes"];
				configs::lua_config_iterator nodes_it		= t_nodes.begin();
				configs::lua_config_iterator nodes_end		= t_nodes.end();

				for( ;nodes_it!=nodes_end; ++nodes_it)
				{
					configs::lua_config_value node	= *nodes_it;
					math::int2 ik					= node["key"];
					cell_key k						( ik.x, ik.y );
					cell_cooker_user_data* cell		= get_cell( tmp_cells, k, cells_root, cell_idx, cell_prefix );

					configs::lua_config_value t_terrain = (*cell->config)["terrain"];
					int count							= t_terrain.size();
					ASSERT_U							(count==0);
					t_terrain.assign_lua_value			( node/*.copy()*/ );
				
				}
			}else
			if(0==strings::compare(tool_name, "composite"))
			{				
 				process_object		(	t_current, 
 					t_objects, 
 					t_scenes, 
 					cells_root, 
 					cell_idx, 
 					tmp_cells, 
 					cell_prefix, 
 					folder_name );

				configs::lua_config_iterator objects_it			= t_current["objects"].begin();
				configs::lua_config_iterator objects_it_e		= t_current["objects"].end();
				
				for(; objects_it!=objects_it_e; ++objects_it)
				{
					configs::lua_config_value t_current_object = *objects_it;

					fixed_string1024 sub_folder_name	= folder_name;
					sub_folder_name						+= (pcstr)t_current["name"];	
					sub_folder_name						+= "/";	
					
					pcstr current_game_object_type		= t_current_object["game_object_type"];

					if (strings::equal( current_game_object_type, "scene" ) )
					{
						resolve_job_resources_names		( sub_folder_name.c_str( ), t_current_object );
						fixed_string1024 scene_name 	= sub_folder_name;
						scene_name						+= (pcstr)t_current_object["name"];
						
						configs::lua_config_value scenes_to_start = t_scenes.get_parent()["start"]["scenes_to_start"];
						scenes_to_start[scenes_to_start.size()]		= scene_name.c_str();
					}

					process_object	( t_current_object, 
										t_objects, 
										t_scenes, 
										cells_root, 
										cell_idx, 
										tmp_cells, 
										cell_prefix, 
										sub_folder_name.c_str() );
				}
			}else
			{
				process_object		(	t_current, 
										t_objects, 
										t_scenes, 
										cells_root, 
										cell_idx, 
										tmp_cells, 
										cell_prefix, 
										folder_name );
			}
		}// object
	} //  for
}


void project_cooker::make_game_project( configs::lua_config_ptr const& editor_config, 
									   resources::query_result_for_cook* parent_query )
{
	fs_new::virtual_path_string project_name		= parent_query->get_requested_path();
	fs_new::virtual_path_string cell_prefix			= project_name;

	game_project*	project				= NEW(game_project)();
	parent_query->set_unmanaged_resource( project, resources::unmanaged_memory, 
											sizeof(game_project) );

	configs::lua_config_ptr	game_config_lua	= configs::create_lua_config( project_name.c_str() );

	
	configs::lua_config_value game_project_root			= ( *game_config_lua );
	configs::lua_config_value t_scenes_root				= game_project_root["scenes"];
	configs::lua_config_value const& editor_project_root= ( *editor_config );

	// startup camera
	game_project_root["camera"].assign_lua_value	( editor_project_root["camera"]/*.copy()*/ );

	//cells subdivision
	configs::lua_config_value t_dst_cells_root		= game_project_root["cells"];
	configs::lua_config_value const& t_folders		= editor_project_root["folders"];
	configs::lua_config_value const& t_objects		= editor_project_root["objects"];

	timing::timer t;
	t.start();
	user_data_cont							tmp_cells;
	u32 cell_idx = 0;
	process_folder						(	t_folders, 
											t_objects,
											"00000000-0000-0000-0000-000000000000",
											t_dst_cells_root, 
											cell_idx, 
											tmp_cells, 
											cell_prefix.c_str(), 
											"",
											t_scenes_root 
											);

	LOG_INFO("make game project take %f sec", t.get_elapsed_sec());
	configs::lua_config_value beh			= game_project_root["start"];
	beh["scenes_to_start"][beh["scenes_to_start"].size()] = "root_scene";

	project->m_config						= game_config_lua->get_binary_config();
	
	// save game project to lua file
	fs_new::virtual_path_string		path_to_save;
	path_to_save.appendf			( "%sprojects/%s/project", resources_converted_path, project_name.c_str() );
	fs_new::native_path_string		path_to_save_disk_path;
	bool need_save_to_file			= !m_editor_present;


	if(need_save_to_file)
	{
		bool convert_result = resources::convert_virtual_to_physical_path( &path_to_save_disk_path, path_to_save, resources::sources_mount );
		R_ASSERT						( convert_result );
		fs_new::synchronous_device_interface const & device	=	get_synchronous_device();
		fs_new::create_folder_r			( device, path_to_save_disk_path.c_str(), false );
		game_config_lua->save_as_binary	( path_to_save_disk_path.c_str() );

#if 1
		fs_new::native_path_string	lua_fn	= path_to_save_disk_path;
		lua_fn							+= ".lua";
		game_config_lua->save_as		( lua_fn.c_str(), configs::target_sources );
#endif
	}

	if( !tmp_cells.empty() )
	{
		fs_new::native_path_string			dir_path;
		fs_new::get_path_without_last_item	( &dir_path, path_to_save_disk_path.c_str() );

		vector< fixed_string<256> >			cell_names;
		u32 const requests_size				= tmp_cells.size();
		buffer_vector< resources::creation_request > cell_requests( ALLOCA( sizeof( resources::creation_request ) * requests_size ), requests_size );
		cell_names.resize					( requests_size );
		user_data_cont::iterator itc		= tmp_cells.begin();
		user_data_cont::iterator itc_e		= tmp_cells.end();

		fixed_string<128>					guid;
		if ( editor_project_root.value_exists("guid") )
		{
			guid.assignf			("_%s", (pcstr)editor_project_root["guid"]);
		}
		else
			guid					= "";

		for( u32 idx=0 ;itc!=itc_e; ++itc,++idx)
		{
			cell_cooker_user_data* ud		= itc->second;
			cell_key const& k				= itc->first;

			fixed_string<256>& cell_name	= cell_names[idx];
			cell_name.assignf				("%s/%s.cell", cell_prefix.c_str(), k.to_string().c_str() );

			cell_requests.push_back			( resources::creation_request( cell_name.c_str(), const_buffer( (void*)ud, sizeof(cell_cooker_user_data*) ), resources::game_cell_class ) );
			
			ud->config->get_root()["guid"]	= guid.c_str();
			
			if ( need_save_to_file )
			{
				fs_new::native_path_string fn;
				fn.assignf_with_conversion	( "%s/%s.cell", dir_path.c_str(), k.to_string().c_str() );
				ud->config->save_as_binary	( fn.c_str() );

#if 1
				fn.assignf_with_conversion	( "%s/%s.lua_cell", dir_path.c_str(), k.to_string().c_str() );
				ud->config->save_as			( fn.c_str(), configs::target_sources );
#endif
			 }
		}

		resources::query_create_resources(
			&cell_requests[0],
			cell_requests.size(),
			boost::bind( &project_cooker::on_cells_loaded, this, _1, parent_query, project ),
			g_allocator,
			NULL,
			parent_query
			);
	}else
	{	// no cells, only terrain
		parent_query->finish_query	( result_success );
	}

}


void project_cooker::on_cells_loaded(	resources::queries_result& data,
										resources::query_result_for_cook* parent_query,
										game_project*	project )
{
	R_ASSERT					( data.is_successful() );
	// do nothing, resource will be cached by game resource manager and queried again by cell_manager

	parent_query->set_unmanaged_resource( project, resources::nocache_memory, sizeof(game_project) );

	u32 cell_count				= data.size();

	for( u32 i=0; i<cell_count; ++i )
	{
		resources::query_result_for_user& q = data[i];
		const_buffer user_data		= q.creation_data_from_user();

		cell_cooker_user_data* ud	= (cell_cooker_user_data*)user_data.c_ptr();
		// just for check
		game_cell_ptr cell_ptr		= static_cast_resource_ptr<game_cell_ptr>(q.get_unmanaged_resource());
		project->m_cells[ud->key]	= cell_ptr;
		DELETE						( ud );
	}

	parent_query->finish_query	( result_success );
}
#endif // MASTER_GOLD

void project_cooker::delete_resource( resources::resource_base * resource )
{
	DELETE( resource );
}

} // namespace stalker2
