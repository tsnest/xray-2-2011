////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "tool_terrain.h"
#include "level_editor.h"
#include "editor_world.h"
#include "project_items.h"
#include "project.h"
#include "lua_config_value_editor.h"
#include "editor_cell_manager.h"

#pragma managed( push, off )
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray {
namespace render { struct terrain_base; }

namespace editor {

terrain_core::terrain_core( tool_terrain^ tool_terrain, 
						   render::scene_ptr const& scene, 
						   render::scene_view_ptr const& scene_view ) 
:super			( tool_terrain ),
m_terrain_tool	( tool_terrain ),
m_scene			( NEW(render::scene_ptr) (scene) ),
m_scene_view	( NEW(render::scene_view_ptr) (scene_view) )
{
	node_size_		=	64;
	image_index		=	0;
}

terrain_core::~terrain_core( )
{
	DELETE			( m_scene_view );
	DELETE			( m_scene );
}

void terrain_core::set_visible( bool bvisible )
{
	super::set_visible			(bvisible);
	for each( key_value_pair e in m_nodes )
		e.Value->set_visible	(bvisible);
}

void terrain_core::load_defaults( )
{
	super::load_defaults	( );
	create_node				( terrain_node_key(0, 0)  );
	create_node				( terrain_node_key(0, 1) );

	create_node				( terrain_node_key(-1, 0) );
	create_node				( terrain_node_key(-1, 1) );
}

void terrain_core::load_props( configs::lua_config_value const& t )
{
	configs::lua_config_value t_objects = t["nodes"];

	configs::lua_config::const_iterator it	 = t_objects.begin();
	configs::lua_config::const_iterator it_e = t_objects.end();

	for(; it!=it_e; ++it)
	{
		xray::configs::lua_config_value current = *it;
		object_base^ o				= m_terrain_tool->load_object(current, *m_scene, *m_scene_view);
		terrain_node^ node			= safe_cast<terrain_node^>(o);

		ASSERT						(!m_nodes.ContainsKey(node->m_tmp_key));
		project_item_object^ item	= gcnew project_item_object( get_project(), node );
		item->assign_id				(0);
		add_node_internal			(node->m_tmp_key, node);
	}
}

void terrain_core::save( configs::lua_config_value t )
{
	t["type"]			= "object";
	set_c_string		(t["name"], get_name());
	set_c_string		(t["lib_name"], get_library_name());
	set_c_string		(t["tool_name"], owner_tool()->name());


	// prepare "terrain" folder
	System::String^ path			= gcnew System::String(get_project()->save_project_resources_path().c_str());
	path							+= "/terrain";
	path							= System::IO::Path::GetFullPath(path)->Replace("\\", "/");
	

	if(!System::IO::Directory::Exists(path))
		System::IO::Directory::CreateDirectory(path);
	
	configs::lua_config_value t_objects = t["nodes"];
	
	int i=0;

	for each( key_value_pair e in m_nodes )
	{
		terrain_node^ n						= e.Value;
		configs::lua_config_value current	= t_objects[i];
		n->save_config						( current );
		
		n->save_vertices		( path + System::String::Format("/{0}_{1}.terr", n->m_tmp_key.x, n->m_tmp_key.z),
									false );
		++i;
	}
}

void terrain_core::set_transform( float4x4 const& )
{
	super::set_transform(float4x4().identity());
}

void terrain_core::create_node( terrain_node_key key )
{
	ASSERT				(!m_nodes.ContainsKey(key));

	terrain_node^ node			= safe_cast<terrain_node^>(m_terrain_tool->create_library_object_instance("terrain_64x64", *m_scene, *m_scene_view));
	project_item_object^ item	= gcnew project_item_object( get_project(), node );
	item->assign_id				( 0 );

	add_node_internal			( key, node );
}

void terrain_core::clear_node( terrain_node_key key )
{
	ASSERT					( m_nodes.ContainsKey(key) );

	remove_node_internal	( key );
}

void terrain_core::destroy_all( )
{
	System::Collections::Generic::List<terrain_node_key> lst = m_nodes.Keys;

	for each(terrain_node_key key in lst)
		remove_node_internal(key);

	ASSERT(m_nodes.Count==0);
}

void terrain_core::update( )
{
	if(m_active_updating_nodes.Count)
		return;

	while(m_update_queries.Count)
	{
		if( update_vertices_impl(0))
			m_update_queries.RemoveAt(0);
		else
			break;
	}

	// draw selection
	if(m_selected_quad_id_list.Count!=0)
	{
		xray::render::debug_vertices_type	vertices	( g_allocator );
		xray::render::debug_indices_type	indices		( g_allocator ); 

		math::float3 cp, cd;
		m_owner_tool->get_level_editor()->view_window()->get_camera_props( cp, cd );

		key_vert_id_dict::Enumerator^ e		= m_selected_quad_id_list.GetEnumerator();
		while(e->MoveNext())
		{
			if(!m_nodes.ContainsKey(e->Current.Key))
			{
				m_selected_quad_id_list.Clear();
				break;
			}

			terrain_node^ curr_terrain		= m_nodes[e->Current.Key];
			vert_id_list^	vlist			= e->Current.Value;

			terrain_quad					quad;
			for each (u16 quad_idx in vlist)
			{
				if(!curr_terrain->m_vertices.empty() && curr_terrain->get_quad(quad, quad_idx))
					quad.debug_draw				( vertices, indices, 0xff7fff7f, cp );
			}
		}

		get_editor_renderer().debug().draw_triangles( *m_scene, vertices, indices );
	}
}

void terrain_core::command_update_cb( u32 arg )
{
	ASSERT	(m_active_updating_nodes.Contains(arg));

	m_active_updating_nodes.Remove	(arg);
}

void terrain_core::add_node_internal( terrain_node_key key, terrain_node^ node )
{
	m_nodes[key]					= node;
	node->m_tmp_key					= key;

	float sz					= (float)node_size_;
	float4x4 m					= create_translation(float3(key.x*sz, 0.0f, key.z*sz));
	node->set_transform_internal( m );

	get_project()->m_editor_cell_manager->register_terrain_node( node );

	if(get_visible()!=node->get_visible())
		node->set_visible			(get_visible());
}

void terrain_core::remove_node_internal(terrain_node_key key)
{
	terrain_node^ node				= m_nodes[key];

	get_project()->m_editor_cell_manager->unregister_terrain_node( node );

	m_terrain_tool->destroy_object	( node );
	m_nodes.Remove					( key );
}

float terrain_core::get_height( float3 const& position_global )
{
	terrain_node_key picked		= pick_node(position_global);

	if(!m_nodes.ContainsKey(picked))
		return 0.0f;

	terrain_node^ terrain		= m_nodes[picked];
	if(terrain->m_vertices.empty())
		return 0.0f;

	float4x4					inv_transform;
	inv_transform.try_invert	(terrain->get_transform());
	float3 position_local		= inv_transform.transform_position(position_global);

	float res					= terrain->get_height_local	(position_local);
	ASSERT						( math::valid(res) );
	return res;
}

void terrain_core::select_vertices( float3 const& point, 
									float const radius, 
									modifier_shape t, 
									key_vert_id_dict^ dest_list )
{
	TerrainNodesList		keys_list;
	terrain_node_key picked = pick_node(point);

	int r					= math::ceil(radius/(float)node_size_)+1;
	for(int ix = picked.x-r; ix<=picked.x+r; ++ix)
	{
		for(int iz = picked.z-r; iz<=picked.z+r; ++iz)
		{
			terrain_node_key	key(ix, iz);
			if(m_nodes.ContainsKey(key))
			{
				terrain_node^ curr_terrain		= m_nodes[key];
				if(curr_terrain->m_vertices.empty())
					continue;

				vert_id_list^ list				= gcnew vert_id_list;
				curr_terrain->get_vertices_g	(point, radius, t, list);
				if(list->Count)
					dest_list->Add				(key, list);
				else
					delete list;
			}
		}
	}
}

void terrain_core::select_quads( float3 const& point, 
								float const radius, 
								modifier_shape t, 
								key_vert_id_dict^ dest_list )
{
	TerrainNodesList		keys_list;
	terrain_node_key picked = pick_node(point);

	int r					= math::ceil(radius/(float)node_size_)+1;

	for(int ix = picked.x-r; ix<=picked.x+r; ++ix)
	{
		for(int iz = picked.z-r; iz<=picked.z+r; ++iz)
		{
			terrain_node_key	key(ix, iz);

			if(m_nodes.ContainsKey(key))
			{
				terrain_node^ curr_terrain		= m_nodes[key];
				vert_id_list^ list				= gcnew vert_id_list;
				curr_terrain->get_quads_g		( point, radius, t, list );
			
				if(list->Count)
					dest_list->Add				(key, list);
				else
					delete list;
			}
		}
	}
}

void terrain_core::add_texture( System::String^ texture_name )
{
	if(!m_used_textures.Contains(texture_name))
		m_used_textures.Add	(texture_name);
}

void terrain_core::change_texture( System::String^ old_texture_name, System::String^ new_texture_name )
{
	ASSERT(m_used_textures.Contains(old_texture_name));
	ASSERT(!m_used_textures.Contains(new_texture_name));

	int idx					= m_used_textures.IndexOf(old_texture_name);
	m_used_textures[idx]	= new_texture_name;

	fs_new::virtual_path_string	old_ts(unmanaged_string(old_texture_name).c_str());
	fs_new::virtual_path_string	new_ts(unmanaged_string(new_texture_name).c_str());

	get_editor_renderer().terrain_exchange_texture( *m_scene, old_ts, new_ts);

	for each( key_value_pair e in m_nodes )
	{
		int idx = e.Value->m_used_textures.IndexOf(old_texture_name);
		if(idx!=-1)
			e.Value->m_used_textures[idx] = new_texture_name;
	}
}

void terrain_core::sync_visual_vertices( terrain_node^ terrain, vert_id_list^ list )
{
	if(list->Count==0)
		return;

	update_verts_query^ q = gcnew update_verts_query;
	q->id					= terrain->id();
	q->list					= gcnew vert_id_list;
	q->list->AddRange		( list );

	m_update_queries.Add	( q );
}

bool terrain_core::update_vertices_impl( u32 idx )
{
	update_verts_query^ q			= m_update_queries[idx];

	terrain_node^ terrain			= safe_cast<terrain_node^>(m_owner_tool->get_level_editor()->object_by_id(q->id));
	
	if(!terrain->get_model()) // resource not ready
		return false;

	vert_id_list^ vid_list			= q->list;

	m_active_updating_nodes.Add		(q->id);

	math::rectangle<math::uint2>	rect;
	rect.min						= math::uint2(65,65);
	rect.max						= math::uint2(0,0);

	for each (u16 id in vid_list)
	{
		int x,z;
		terrain->vertex_xz(id, x, z);
		rect.min.x	= math::min(rect.min.x, (u32)x);
		rect.min.y	= math::min(rect.min.y, (u32)z);

		rect.max.x	= math::max(rect.max.x, (u32)x);
		rect.max.y	= math::max(rect.max.y, (u32)z);
	}
	
	rect.max.x +=1;
	rect.max.y +=1;

	vectora<render::terrain_buffer_fragment>fragments(debug::g_mt_allocator);

	fragments.push_back						(render::terrain_buffer_fragment(&debug::g_mt_allocator));
	render::terrain_buffer_fragment& buff	= fragments.back();
	buff.rect								= rect;

	terrain->m_vertices.export_vertices		( rect, buff.buffer);


	command_finished_delegate* del = NEW (command_finished_delegate)(
		gcnew command_finished_delegate::Delegate(this, &terrain_core::command_update_cb),
		g_allocator
	);

	render::editor::on_command_finished_functor_type functor	( boost::bind( &command_finished_delegate::callback, del, q->id) );

	get_editor_renderer().terrain_update_cell_buffer(
		*m_scene,
		terrain->get_model()->m_render_model,
		fragments,
		terrain->get_transform(),
		functor
	);

	return true;
}

vert_id_list^ get_or_create( key_vert_id_dict% dict, terrain_node_key% key )
{
	if(!dict.ContainsKey(key))
		dict[key]		= gcnew vert_id_list;
	
	return dict[key];
}

key_vert_id_dict^ terrain_core::selected_quads( )
{
	return %m_selected_quad_id_list;
}

void terrain_core::select_quads( key_vert_id_dict^ list, enum_selection_method method )
{
	key_vert_id_dict::Enumerator^ e		= list->GetEnumerator();

	if(method==enum_selection_method_set)
	{
		m_selected_quad_id_list.Clear		( );

		while(e->MoveNext())
		{
			vert_id_list^ src_list		= e->Current.Value;

			vert_id_list^ dst_list		= get_or_create (m_selected_quad_id_list, e->Current.Key);
			dst_list->AddRange			( src_list );
		}
	}else
	if(method==enum_selection_method_add)
	{
		while(e->MoveNext())
		{
			vert_id_list^ src_list		= e->Current.Value;

			vert_id_list^ dst_list		= get_or_create(m_selected_quad_id_list, e->Current.Key);
			for each (u16 idx in src_list)
			{
				if(!dst_list->Contains(idx))
					dst_list->Add( idx );
			}
		}
	}else 
	if(method==enum_selection_method_subtract)
	{
		while(e->MoveNext())
		{
			vert_id_list^ src_list		= e->Current.Value;

			if(m_selected_quad_id_list.ContainsKey(e->Current.Key))
			{
				vert_id_list^ dst_list		= m_selected_quad_id_list[e->Current.Key];
				for each (u16 idx in src_list)
				{
					if(dst_list->Contains(idx))
						dst_list->Remove( idx );
				}
			}
		}
	}else
		UNREACHABLE_CODE();

}

} // namespace editor
} // namespace xray
