////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain.h"
#include <xray/render/core/resource_manager.h>
#include "effect_terrain_new.h"
#include <xray/collision/space_partitioning_tree.h>
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

namespace xray {
namespace render {

const float terrain::s_cell_size = 64.f;


inline u8 pack_u4u4( u8 high, u8 low)
{
	return (high<<4)+(low&0x0F);
}

const u32 terrain_vertex_size = 60;
D3D_INPUT_ELEMENT_DESC	terrain_vertex_declaration[8] =
{
	{"POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",	1, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 28,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,	0, 32, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",1, DXGI_FORMAT_R32G32_FLOAT,	0, 40, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",2, DXGI_FORMAT_R32G32_FLOAT,	0, 48, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",3, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 56, 	D3D_INPUT_PER_VERTEX_DATA, 0}
};	

const u32 terrain_grid_vertex_size = 8;
D3D_INPUT_ELEMENT_DESC	terrain_grid_vertex_declaration[1] =
{
	{"POSITION",0, DXGI_FORMAT_R32G32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
};	


const math::uint2	terrain::m_cell_size( 64+1, 64+1);

terrain::terrain	( ) :
	m_texture_pool					( 2048, 16 ),
	m_intermediate_vertex_stream	( 1024*1024 )
{

	m_intermediate_height_texture	= resource_manager::ref().find_texture( "$user$inter_terr_height");
	m_intermediate_color_texture	= resource_manager::ref().find_texture( "$user$inter_terr_color");
	m_intermediate_params_texture	= resource_manager::ref().find_texture( "$user$inter_terr_params");
	m_intermediate_tex_ids_texture	= resource_manager::ref().find_texture( "$user$inter_terr_tex_ids");
	m_intermediate_tc_shift_texture	= resource_manager::ref().find_texture( "$user$inter_terr_tc_shift");

	if( !m_intermediate_height_texture)
		m_intermediate_height_texture	= resource_manager::ref().create_texture2d( "$user$inter_terr_height", m_cell_size.x, m_cell_size.y, NULL, s_height_texture_format, D3D_USAGE_STAGING, 1);

	if( !m_intermediate_color_texture)
		m_intermediate_color_texture	= resource_manager::ref().create_texture2d( "$user$inter_terr_color", m_cell_size.x, m_cell_size.y, NULL, s_color_texture_format, D3D_USAGE_STAGING, 1);

	if( !m_intermediate_params_texture)
		m_intermediate_params_texture	= resource_manager::ref().create_texture2d( "$user$inter_terr_params", m_cell_size.x, m_cell_size.y, NULL, s_params_texture_format, D3D_USAGE_STAGING, 1);

	if( !m_intermediate_tex_ids_texture)
		m_intermediate_tex_ids_texture	= resource_manager::ref().create_texture2d( "$user$inter_terr_tex_ids", m_cell_size.x, m_cell_size.y, NULL, s_tex_ids_texture_format, D3D_USAGE_STAGING, 1);

	if( !m_intermediate_tc_shift_texture)
		m_intermediate_tc_shift_texture	= resource_manager::ref().create_texture2d( "$user$inter_terr_tc_shift", m_cell_size.x, m_cell_size.y, NULL, s_tc_shift_texture_format, D3D_USAGE_STAGING, 1);

	m_effect	= NEW( effect_terrain_NEW);

 	m_collision_tree	=	&(*(collision::new_space_partitioning_tree( render::g_allocator, 1.f, 1024 )));

	generate_grids	();
}

terrain::~terrain()
{
	collision::delete_space_partitioning_tree ( m_collision_tree);

// 	collision::destroy( m_collision_tree_editor);
// 	collision::destroy( m_collision_tree_game);

	DELETE		( m_effect );
}

void terrain::add_cell( terrain_render_model_instance_ptr v )
{
	cells& working_set			= m_cells;

	R_ASSERT( std::find(working_set.begin(), working_set.end(), v)==working_set.end());
	working_set.push_back		( v );

	collision::space_partitioning_tree* working_tree = m_collision_tree;

	working_tree->insert		( &v->m_collision_object, v->transform() );
}

void terrain::remove_cell( terrain_render_model_instance_ptr v)
{
	cells&	working_set			= m_cells;

	cells::iterator it			= std::find( working_set.begin(), working_set.end(), v );

	ASSERT( it != working_set.end());
		
	collision::space_partitioning_tree* working_tree = m_collision_tree;

	working_tree->erase			( &(*it)->m_collision_object);
	
	working_set.erase			( it );
}

void terrain::update_cell_buffer( terrain_render_model_instance_ptr v, xray::vectora<render::terrain_buffer_fragment> const& fragments, float4x4 const& )
{
	terrain_render_model_instance* instance	= static_cast_checked<terrain_render_model_instance*>(v.c_ptr());
	terrain_render_model*	terrain_model = instance->m_terrain_model.c_ptr();

	vectora<render::terrain_buffer_fragment>::const_iterator	it_frag	= fragments.begin();
		
	it_frag	= fragments.begin();

	u32 heightmap_row_pitch;
	heightmap_element*	heightmap_buffer	= (heightmap_element*)m_intermediate_height_texture->map2D( D3D_MAP_WRITE, 0, heightmap_row_pitch);

	u32 color_map_row_pitch;
	color_element*		color_buffer		= (color_element*)m_intermediate_color_texture->map2D( D3D_MAP_WRITE, 0, color_map_row_pitch);

	u32 params_map_row_pitch;
	params_element*		params_buffer		= (params_element*)m_intermediate_params_texture->map2D( D3D_MAP_WRITE, 0, params_map_row_pitch);

	u32 tex_ids_map_row_pitch;
	params_element*		tex_ids_buffer		= (params_element*)m_intermediate_tex_ids_texture->map2D( D3D_MAP_WRITE, 0, tex_ids_map_row_pitch);

	u32 tc_shift_map_row_pitch;
	params_element*		tc_shifts_buffer	= (params_element*)m_intermediate_tc_shift_texture->map2D( D3D_MAP_WRITE, 0, tc_shift_map_row_pitch);

	fill_textures( &it_frag->buffer[0],
					it_frag->rect.width(),
					it_frag->rect.height(),
					terrain_model->textures(),
					heightmap_buffer, 
					heightmap_row_pitch,
					color_buffer,
					color_map_row_pitch,
					params_buffer,
					params_map_row_pitch,
					tex_ids_buffer,
					tex_ids_map_row_pitch,
					tc_shifts_buffer,
					tc_shift_map_row_pitch);

	m_intermediate_height_texture->unmap2D		( 0);
	m_intermediate_color_texture->unmap2D		( 0);
	m_intermediate_params_texture->unmap2D		( 0);
	m_intermediate_tex_ids_texture->unmap2D		( 0);
	m_intermediate_tc_shift_texture->unmap2D	( 0);

	resource_manager::ref().copy2D( terrain_model->height_texture(), it_frag->rect.left, it_frag->rect.top, &*m_intermediate_height_texture, 0, 0, it_frag->rect.width(), it_frag->rect.height(), 0, 0);
	resource_manager::ref().copy2D( terrain_model->color_texture(), it_frag->rect.left, it_frag->rect.top, &*m_intermediate_color_texture, 0, 0, it_frag->rect.width(), it_frag->rect.height(), 0, 0);
	resource_manager::ref().copy2D( terrain_model->params_texture(), it_frag->rect.left, it_frag->rect.top, &*m_intermediate_params_texture, 0, 0, it_frag->rect.width(), it_frag->rect.height(), 0, 0);
	resource_manager::ref().copy2D( terrain_model->tex_ids_texture(), it_frag->rect.left, it_frag->rect.top, &*m_intermediate_tex_ids_texture, 0, 0, it_frag->rect.width(), it_frag->rect.height(), 0, 0);
	resource_manager::ref().copy2D( terrain_model->tc_shift_texture(), it_frag->rect.left, it_frag->rect.top, &*m_intermediate_tc_shift_texture, 0, 0, it_frag->rect.width(), it_frag->rect.height(), 0, 0);
}

void terrain::update_cell_aabb( terrain_render_model_instance_ptr const v, math::aabb bbox)
{
	collision::space_partitioning_tree*	working_tree = m_collision_tree;

	v->m_terrain_model->m_aabbox		= bbox;
	working_tree->move					( &v->m_collision_object, v->transform() );
}

void terrain::add_cell_texture( terrain_render_model_instance_ptr model, fs_new::virtual_path_string const & texture, u32 tex_user_id)
{
	terrain_render_model_ptr v	= model->m_terrain_model;
	int tex_pool_id			= m_texture_pool.add_texture( texture);
	ASSERT					( tex_pool_id >= 0);
	ASSERT					( std::find( v->m_textures.begin(), v->m_textures.end(), tex_pool_id) == v->m_textures.end());

	v->m_textures.resize( math::max( tex_user_id+1, v->m_textures.size()) , -1 );
	v->m_textures[tex_user_id] = tex_pool_id;
}

void terrain::remove_cell_texture( terrain_render_model_instance_ptr model, u32 tex_user_id)
{
	terrain_render_model_ptr v	= model->m_terrain_model;

	int tex_pool_id		= -1;
	ASSERT( v->m_textures.size()> tex_user_id);
	tex_pool_id = v->m_textures[tex_user_id]; 

	// Mark as unused 
	ASSERT( tex_pool_id >=0);
	v->m_textures[tex_user_id] = -1;

	// Remove void tail
	int i = int(v->m_textures.size()-1);
	for( ; i >= 0; --i)
		if( v->m_textures[i] != -1)
			break;

	v->m_textures.resize(i+1);

	ASSERT( tex_pool_id>=0);
	if( tex_pool_id <0)
		return;

	// Remove texture from the pool if it isn't used any more by other cells.
	cells::iterator			it = m_cells.begin();
	cells::const_iterator	en = m_cells.end();

	for( ; it!= en; ++it)
		 if( v->m_textures.end() != std::find( v->m_textures.begin(), v->m_textures.end(), tex_pool_id))
			return;

	m_texture_pool.remove_texture( tex_pool_id);
}

void terrain::exchange_texture	( fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const & new_texture)
{
	m_texture_pool.exchange_texture( old_texture, new_texture);
}

terrain_render_model_instance_ptr terrain::get_cell( pcstr cell_id)
{
	cells::const_iterator it		= m_cells.begin();
	cells::const_iterator it_e		= m_cells.end();
	
	// need optimization later
	for( ; it!= it_e; ++it)
	{
		if((*it)->m_terrain_model->m_name == cell_id)
			return (*it);
	}

	return 0;
}

bool terrain::has_cell( pcstr cell_id) const
{
	cells::const_iterator it		= m_cells.begin();
	cells::const_iterator it_e		= m_cells.end();
	
	// need optimization later
	for( ; it!= it_e; ++it)
	{
		if((*it)->m_terrain_model->m_name == cell_id)
			return true;
	}

	return false;
}

u32 fill_vertices( void* vertex_buffer, 
						 u32 vertex_count, 
						 u32 offset, 
						 u32 vertex_row_size, 
						 float physical_size, 
						 float4x4 transform, 
						 render::terrain_data const* t_data, 
						 terrain_render_model::terrain_textures_type const& textures,
						 float* heightfield)
{
	ASSERT( vertex_count <= vertex_row_size*vertex_row_size);

	float inv_row_size = 1.f/vertex_row_size;

	render::terrain_data const* src_ptr = t_data;
	char*				dst_ptr = (char*)vertex_buffer;
	for( u32 i = 0; i < vertex_count; ++i, ++src_ptr)
	{
		float z = (float)math::floor((i+offset)*inv_row_size);
		float x = (i+offset)-z*vertex_row_size;

		x*= physical_size/(vertex_row_size-1);
		z*= -physical_size/(vertex_row_size-1);
		
		if(heightfield)
			heightfield[i]		= src_ptr->height;

// 		heightmap_buffer[i]		= (u16)((src_ptr->height+64.f)*(65536.f/128.f));
// 
// 		color_buffer[i]			= src_ptr->color;
// 
// 		params_buffer[i]		= math::color_rgba( pack_u4u4(src_ptr->alpha0, src_ptr->alpha1), pack_u4u4( src_ptr->alpha2, u8(textures[src_ptr->tex_id0])), pack_u4u4(u8(textures[src_ptr->tex_id1]), u8(textures[src_ptr->tex_id2])), 0);

		// writing position 
		*(float3*) dst_ptr	= transform.transform_position( float3( x, src_ptr->height, z));
		dst_ptr+= sizeof(float3);

		// writing normal 
		*(float3*) dst_ptr	= float3( 0, 1, 0);
		dst_ptr+= sizeof(float3);

		
		u32 quantizer = 64;
		u32 c0 = math::color_rgba( (src_ptr->alpha0/quantizer)*quantizer, (src_ptr->alpha1/quantizer)*quantizer, (src_ptr->alpha2/quantizer)*quantizer, 0);
		*(u32*) dst_ptr = c0;
		dst_ptr+= sizeof(u32);

//		u32			color_aaa	= src_ptr->color;
//		u8 const	tex_id2		= u8(textures[src_ptr->tex_id2]);

		// writing diffuse color
		//color_aaa = (color_aaa&0x00FFFFFF)|(tex_id2<<24);
//.		*(u32*) dst_ptr		= math::color_argb( (color_aaa>>24)&0xff, (color_aaa)&0xff, (color_aaa>>8)&0xff, (color_aaa>>16)&0xff );
		*(u32*) dst_ptr		= src_ptr->color;
		dst_ptr+= sizeof(u32);

		// Tex0 coordinates
		*(float2*) dst_ptr  = src_ptr->tex_shift;
		dst_ptr+= sizeof(float2);

		// Tex1 coordinates
		*(float2*) dst_ptr  = float2( 0,0);
		dst_ptr+= sizeof(float2);

		// Tex2 coordinates
		*(float2*) dst_ptr  = float2( 0,0);
		dst_ptr+= sizeof(float2);

		// Writing texture indices
		u32 tex_indices = math::color_rgba( u32(textures[src_ptr->tex_id0]), u32(textures[src_ptr->tex_id1]), u32(textures[src_ptr->tex_id2]), 0u) ;
		*(u32*) dst_ptr		= tex_indices;
		dst_ptr+= sizeof(u32);
	}

	return vertex_count;
}

void fill_textures(	render::terrain_data const* t_data, 
				  	u32		size_x, 
				  	u32		size_y, 
					//u32 	start_x, 
					//u32 	start_y, 
					//float	physical_size, 
					terrain_render_model::terrain_textures_type const& textures,
					terrain::heightmap_element*		heightmap_buffer,
					u32		heightmap_buffer_row_pitch,
					terrain::color_element*		color_buffer,
					u32		color_buffer_row_pitch,
					terrain::params_element*		params_buffer,
					u32		params_buffer_row_pitch,
					terrain::tex_ids_element*	tex_ids_buffer,
					u32		tex_ids_buffer_row_pitch,
					terrain::tc_shift_element*	tc_shift_buffer,
					u32		tc_shift_buffer_row_pitch
					)
{
	for( u32 y = 0; y < size_y; ++y )
	{
		terrain::heightmap_element* heightmap_row	= (terrain::heightmap_element*)	(((u8*)heightmap_buffer)	+ (y)*heightmap_buffer_row_pitch);
		terrain::color_element*		colormap_row	= (terrain::color_element*)		(((u8*)color_buffer)		+ (y)*color_buffer_row_pitch);
		terrain::params_element*	paramsmap_row	= (terrain::params_element*)	(((u8*)params_buffer)		+ (y)*params_buffer_row_pitch);
		terrain::tex_ids_element*	tex_ids_row		= (terrain::tex_ids_element*)	(((u8*)tex_ids_buffer)		+ (y)*tex_ids_buffer_row_pitch);
		terrain::tc_shift_element*	tc_shift_row	= (terrain::tc_shift_element*)	(((u8*)tc_shift_buffer)		+ (y)*tc_shift_buffer_row_pitch);

		for( u32 x = 0; x < size_x; ++x )
		{
			const float max_height = 256.f;
			render::terrain_data const * src_ptr =  t_data+y*size_x + x;
			heightmap_row	[x]	= (terrain::heightmap_element)((src_ptr->height+max_height/2)*(65536.f/max_height));
			colormap_row	[x]	= math::color_rgba( math::color_get_B(src_ptr->color), math::color_get_G(src_ptr->color), math::color_get_R(src_ptr->color), math::color_get_A(src_ptr->color));
			paramsmap_row	[x]	= math::color_rgba( u32(src_ptr->alpha2), u32(src_ptr->alpha1), u32(src_ptr->alpha0), 0u);
			tex_ids_row		[x] = math::color_rgba( u32(textures[src_ptr->tex_id2]), u32(textures[src_ptr->tex_id1]), u32(textures[src_ptr->tex_id0]), 0u);

			const float	max_shift = 64.f;
			u32	X = (u16)((src_ptr->tex_shift.x + max_shift)*( 65536.f/(2*max_height) ));
			u32	Z = (u16)((src_ptr->tex_shift.y + max_shift)*( 65536.f/(2*max_height) ));

			tc_shift_row	[x] = (Z << 16) | (X&0xFFFF);
		}
	}
}


void fill_indexes( math::uint2 size, u16* buffer)
{
	u32 ptr = 0;
	for( u32 i = 0; i< size.y-1; ++i)
	for( u32 j = 0; j< size.x-1; ++j)
	{
		buffer[ptr] = u16(i*size.x+j);
		ptr++;
		buffer[ptr] = u16(i*size.x+j+1);
		ptr++;
		buffer[ptr] = u16((i+1)*size.x+j);
		ptr++;

		buffer[ptr] = u16(i*size.x+j+1);
		ptr++;
		buffer[ptr] = u16((i+1)*size.x+j+1);
		ptr++;
		buffer[ptr] = u16((i+1)*size.x+j);
		ptr++;
	}
}

void terrain::select_cells	(float4x4 const & mat_vp, render::vector<terrain_render_model_instance_ptr>& cells) const
{
	math::frustum view_frustum (mat_vp);	

	cells.clear();

	collision::objects_type query_result(render::g_allocator);
	m_collision_tree->cuboid_query( u32(-1), view_frustum, query_result);

	cells.reserve( cells.size() + query_result.size());

	collision::objects_type::const_iterator end = query_result.end();
	for( collision::objects_type::iterator it	= query_result.begin(); it != end; ++it)
	{
		terrain_render_model_instance* instance = ((collision_object<terrain_render_model_instance>*)(*it))->owner();
		cells.push_back( instance );
	}
}

// void terrain::select_game_cells	( float4x4 const & mat_vp, render::vector<composite_render_model_instance_ptr> & game_cells) const
// {
// 	math::frustum view_frustum (mat_vp);	
// 
// 	game_cells.clear();
// 
// 	collision::objects_type query_result(render::g_allocator);
// 	m_collision_tree_game->cuboid_query( u32(-1), view_frustum, query_result);
// 
// 	game_cells.reserve( game_cells.size() + query_result.size());
// 
// 	collision::objects_type::const_iterator end = query_result.end();
// 	for( collision::objects_type::iterator it = query_result.begin(); it != end; ++it)
// 	{
// 		composite_render_model_instance* instance		= ((collision_object<composite_render_model_instance>*)(*it))->owner();
// 		game_cells.push_back		( instance );
// 	}
// }

// terrain cook
void extend_bounding_box( render::terrain_data* buffer, u32 const size, float physical_size, u32 vertex_row_size, float4x4 const & world_transform, math::aabb & bbox)
{
	float inv_row_size = 1.f/vertex_row_size;

	for( u32 i = 0; i < size; ++i)
	{
		float z = (float)math::floor((i+0)*inv_row_size);
		float x = (i+0)-z*vertex_row_size;

		x*= physical_size/(vertex_row_size-1);
		z*= -physical_size/(vertex_row_size-1);

		// writing position 
		float3 tmp_point	= world_transform.transform_position( float3( x, buffer[i].height, z));
		bbox.modify( tmp_point);
	}
}

void terrain::generate_grids			( )
{
	// vertices buffer size load 0
	u32 vbuffer_size_max = math::sqr(s_quads_count_0+1)*sizeof(float2);
	u32 ibuffer_size_max = s_quads_count_0*s_quads_count_0*2*3*sizeof(u16);
	u32 buffer_size = math::max( vbuffer_size_max, ibuffer_size_max);

	char* buffer 	= static_cast<char*>(ALLOCA(buffer_size));

	m_grid_vertices_0	= create_vertex_buffer( buffer, buffer_size, s_quads_count_0, s_cell_size);
	m_grid_vertices_1	= create_vertex_buffer( buffer, buffer_size, s_quads_count_1, s_cell_size);

	m_grid_indices_0	= create_index_buffer		( buffer, buffer_size, s_quads_count_0, TERRAIN_CELL_SIDE_NEGATIVE|TERRAIN_CELL_SIDE_POSITIVE, TERRAIN_CELL_SIDE_NEGATIVE|TERRAIN_CELL_SIDE_POSITIVE);
	m_grid_indices_1	= create_index_buffer		( buffer, buffer_size, s_quads_count_1, 0, 0);

	m_grid_geom_0		= resource_manager::ref().create_geometry( terrain_grid_vertex_declaration, terrain_grid_vertex_size, *m_grid_vertices_0, *m_grid_indices_0);
	m_grid_geom_1		= resource_manager::ref().create_geometry( terrain_grid_vertex_declaration, terrain_grid_vertex_size, *m_grid_vertices_1, *m_grid_indices_1);
}

untyped_buffer_ptr 		terrain::create_index_buffer		( char* buffer, u32 buffer_size, u32 quad_count, u32 join_side_x, u32 join_side_z)
{
	u16* indices		= (u16*)buffer;
	u32 ibuffer_size	= quad_count*quad_count*2*3*sizeof(u16);

	ibuffer_size		-= (join_side_x&TERRAIN_CELL_SIDE_NEGATIVE) ? (quad_count/2)*3*sizeof(u16) : 0;
	ibuffer_size		-= (join_side_x&TERRAIN_CELL_SIDE_POSITIVE) ? (quad_count/2)*3*sizeof(u16) : 0;
	ibuffer_size		-= (join_side_z&TERRAIN_CELL_SIDE_NEGATIVE) ? (quad_count/2)*3*sizeof(u16) : 0;
	ibuffer_size		-= (join_side_z&TERRAIN_CELL_SIDE_POSITIVE) ? (quad_count/2)*3*sizeof(u16) : 0;

	R_ASSERT_U			( ibuffer_size <= buffer_size);

	u16* quad_ind_start = indices;
	for( u32 z = 0; z < (int)quad_count; ++z)
		for( u32 x = 0; x < (int)quad_count; ++x)
		{
			// Snake style.
			u32 _x = (z%2) == 0 ? x : quad_count - 1 - x;

			u32 flip = _x%2^z%2;

			if( !((join_side_z&TERRAIN_CELL_SIDE_POSITIVE) && (_x%2) && (z == quad_count - 1)) )
			{
				u32 expand_triangle_x	= ((join_side_z&TERRAIN_CELL_SIDE_POSITIVE) && (z == quad_count - 1)) ? 1 : 0;
				u32 expand_triangle_z_0 = ((join_side_x&TERRAIN_CELL_SIDE_NEGATIVE) && (_x == 0) && ((z+1)%2)) ? 1 : 0;
				u32 expand_triangle_z_1 = ((join_side_x&TERRAIN_CELL_SIDE_POSITIVE) && (_x == quad_count - 1) && ((z+1)%2)) ? 1 : 0;
				
				*quad_ind_start = (u16)((z+1+expand_triangle_z_1)*(quad_count+1) + (_x+1+expand_triangle_x));
				++quad_ind_start;
				*quad_ind_start = (u16)((z+1+expand_triangle_z_0)*(quad_count+1) + _x);
				++quad_ind_start;
				*quad_ind_start = (u16)(z*(quad_count+1) + (_x+flip));
				++quad_ind_start;
			}

			if( !((join_side_z&TERRAIN_CELL_SIDE_NEGATIVE) && (_x%2) && z == 0) 
				&& !((join_side_x&TERRAIN_CELL_SIDE_NEGATIVE) && (z%2) && _x == 0)
				&& !((join_side_x&TERRAIN_CELL_SIDE_POSITIVE) && (z%2) && (_x == quad_count - 1)))
			{
				u32 expand_triangle_x = (join_side_z&TERRAIN_CELL_SIDE_NEGATIVE && z == 0) ? 1 : 0;
								
				*quad_ind_start = (u16)(z*(quad_count+1) + (_x+1+expand_triangle_x));
				++quad_ind_start;
				*quad_ind_start = (u16)((z+1)*(quad_count+1) + (_x+1-flip));
				++quad_ind_start;
				*quad_ind_start = (u16)(z*(quad_count+1) + _x);
				++quad_ind_start;
			}
		}

	return resource_manager::ref().create_buffer( ibuffer_size, indices, enum_buffer_type_index, false);
}

untyped_buffer_ptr 		terrain::create_vertex_buffer	( char* buffer, u32 buffer_size, u32 quad_count, float cell_size)
{
	u32 vbuffer_size = math::sqr(quad_count+1)*sizeof(float2);
	R_ASSERT_U( vbuffer_size <= buffer_size);

	float2* vertices = (float2*)buffer;

	for( u32 z = 0; z < quad_count+1; ++z)
		for( u32 x = 0; x < quad_count+1; ++x)
			vertices[ z*(quad_count+1)+x] = float2( (float)x*cell_size/quad_count, -(float)z*cell_size/quad_count);

	return resource_manager::ref().create_buffer( vbuffer_size, vertices, enum_buffer_type_vertex, false);
}


} // namespace render
} // namespace xray
