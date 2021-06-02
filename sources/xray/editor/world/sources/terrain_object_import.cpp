////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "terrain_quad.h"
#include "project.h"
#include <xray/macro_library_name.h>
#include "editor_cell_manager.h"
#include "level_editor.h"
#include "objects_library.h"
#include "tool_terrain.h"

#include <FreeImage/FreeImage.h>
#pragma comment(lib, XRAY_LIBRARY_NAME( FreeImage, lib ) )

namespace xray {
namespace editor {

extern bool g_block_visual_loading;

void terrain_core::do_import( terrain_import_export_settings^ settings )
{
	R_ASSERT(settings->m_options == terrain_import_export_settings::options::grayscale_heightmap);

	unmanaged_string fn		(settings->m_source_filename);
	FREE_IMAGE_FORMAT	fif = FreeImage_GetFIFFromFilename(fn.c_str());
	
	R_ASSERT			( fif==FIF_TIFF );
	FIBITMAP* fb		= FreeImage_Load(fif, fn.c_str());
	R_ASSERT			( FreeImage_GetImageType(fb) == FIT_FLOAT );
	FreeImage_FlipVertical( fb );

	u32 const bitmap_width	= FreeImage_GetWidth(fb);
	u32 const bitmap_height	= FreeImage_GetHeight(fb);
	u32 const bitmap_size = bitmap_width*bitmap_height;
	float* src_bits		= (float*)FreeImage_GetBits(fb);
	
	float const height_range	= settings->m_max_height-settings->m_min_height ;
	int const dim				= 65;
	
	System::Collections::Generic::List<terrain_node^> processed_nodes;
	g_block_visual_loading	= true;

	for(int ix = 0; ix<settings->size_x; ++ix )
		for(int iz = 0; iz<settings->size_z; ++iz )
		{
			terrain_node_key	key(settings->lt_x+ix, settings->lt_z-iz);
			u32 start_lt		= iz*( (dim-1)*bitmap_width) + ix*(dim-1);
			
			if(!m_nodes.ContainsKey(key))
			{
				if(settings->m_b_create_cell)
				{
					LOG_INFO	("import: new node [%d:%d]", key.x, key.z);
					create_node	( key );
				}else
				{
					LOG_INFO	("import: skip empty node [%d:%d]", key.x, key.z);
					continue;
				}
			}else
			{
				LOG_INFO("import: node exist [%d:%d]", key.x, key.z);
			}
			
			terrain_node^	terrain		= m_nodes[key];
			processed_nodes.Add			(terrain);

			terrain->load_vertices_sync( );

			vert_id_list^ vid_list		= gcnew vert_id_list;

			for(int _y=0; _y<dim; ++_y)
			{
				for(int _x=0; _x<dim; ++_x)
				{
					u32 idx				= terrain->vertex_id(_x, _y);
					terrain_vertex v	= terrain->m_vertices.get(idx);
					vid_list->Add		((u16)idx);

					u32 const offset	= (start_lt + bitmap_width*_y) +_x;
					R_ASSERT			(offset<bitmap_size);
					float* pixel_ptr	= src_bits + offset;
					float height		= *pixel_ptr;
					height				= (height * height_range) + settings->m_min_height;

					v.set_default		( );
					v.height			= height;
					terrain->m_vertices.set	( v, idx );
				}
			}

			get_project()->set_changed();

			terrain->unload_contents( false );
		}

	FreeImage_Unload		( fb );
	g_block_visual_loading	= false;

	// update editor cell manager
	editor_cell_manager^ m		= get_level_editor()->get_project()->m_editor_cell_manager;
	cell_keys_list^	active_list = m->get_loaded_keys_list( );
	
	for each( terrain_node^ n in processed_nodes )
	{
		cell_key k(n->m_tmp_key.x, n->m_tmp_key.z);
		if(active_list->Contains(k))
			n->load_contents();
	}
}

void terrain_core::reset_all_colors( )
{
	g_block_visual_loading	= true;
	lib_item^ li				= m_terrain_tool->m_library->get_library_item_by_full_path( "terrain_64x64", true );
	System::String^ ln			= gcnew System::String( (*(li->m_config))["lib_name"] );


	for each (terrain_node^	n in m_nodes.Values )
	{
		n->load_vertices_sync	( );
		n->m_used_textures.Clear( );

		n->load_props			( *li->m_config );

		u32 count = n->m_vertices.m_vertices.Count;
		for (u32 idx=0; idx<count; ++idx)
		{
			terrain_vertex v	= n->m_vertices.get(idx);
			float h				= v.height;
			v.set_default		( );
			v.height			= h;
			n->m_vertices.set	( v, idx );
		}

		n->unload_contents( false );
	}


	g_block_visual_loading		= false;
	m_used_textures.Clear		( );

	// update editor cell manager
	editor_cell_manager^ m		= get_level_editor()->get_project()->m_editor_cell_manager;
	cell_keys_list^	active_list = m->get_loaded_keys_list( );
	
	for each( terrain_node^ n in m_nodes.Values )
	{
		cell_key k(n->m_tmp_key.x, n->m_tmp_key.z);
		if(active_list->Contains(k))
			n->load_contents();
	}
}

void terrain_node::copy_from( terrain_node^ other )
{
	m_vertices.clear();
	m_vertices.m_vertices.AddRange( %other->m_vertices.m_vertices );
	
	m_vertices.m_dimension		= other->m_vertices.m_dimension;
	m_vertices.m_modified		= true;
	m_used_textures.Clear		();
	m_used_textures.AddRange	( %other->m_used_textures );
	m_cell_size					= other->m_cell_size;
	m_min_height				= other->m_min_height;
	m_max_height				= other->m_max_height;
}

void terrain_core::copy_node( terrain_node_key from_key, terrain_node_key to_key )
{
	if(!m_nodes.ContainsKey(from_key))
		return;

	if(!m_nodes.ContainsKey(to_key))
		create_node	( to_key );

	terrain_node^ src		= m_nodes[from_key];
	terrain_node^ dst		= m_nodes[to_key];
	g_block_visual_loading	= true;

	bool src_was_loaded		= !src->m_vertices.empty();
	bool dst_was_loaded		= !dst->m_vertices.empty();

	if(!src_was_loaded)
		src->load_contents		( );
	if(!dst_was_loaded)
		dst->load_contents		( );

	while(src->m_vertices.empty() || dst->m_vertices.empty())
	{
		resources::dispatch_callbacks	( );
		threading::yield				( 10 );
		LOG_ERROR("waiting for terrain node vertices");
	}

	dst->copy_from	( src );
	g_block_visual_loading = false;

	src->unload_contents		( false );

	dst->unload_contents	( false );
	if(dst_was_loaded)// force sync visual 
		dst->load_contents		( );
}
	

} // namespace editor
} // namespace xray