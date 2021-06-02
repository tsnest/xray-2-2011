////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_MANAGER_H_INCLUDED
#define TERRAIN_MANAGER_H_INCLUDED

#include "terrain_texture_pool.h"
#include "terrain_render_model.h"
#include <xray/render/core/vertex_buffer.h>

namespace xray {

namespace collision { 
	struct space_partitioning_tree;
} // collision

namespace render {

// forward declarations
class effect_terrain_NEW;
class terrain_visual;
struct terrain_data;

#define			TERRAIN_CELL_SIDE_NEGATIVE		1
#define			TERRAIN_CELL_SIDE_POSITIVE		2

extern const u32 terrain_vertex_size;
extern D3D_INPUT_ELEMENT_DESC	terrain_vertex_declaration[8];

void	fill_indexes	( math::uint2 size, u16* buffer);
u32	fill_vertices		( void* vertex_buffer, 
							 u32 vertex_conut, 
							 u32 offset, 
							 u32 vertex_row_size, 
							 float physical_size, 
							 float4x4 transform, 
							 render::terrain_data const* t_data, 
							 terrain_render_model::terrain_textures_type const& textures,
							 float* heightfield);



void extend_bounding_box	( render::terrain_data* buffer, 
								u32 const size, 
								float physical_size, 
								u32 vertex_row_size, 
								float4x4 const & world_transform, 
								math::aabb & bbox);

class terrain {
public:
	typedef render::vector<terrain_render_model_instance_ptr>	cells;
	typedef	u16			heightmap_element;
	typedef	u32			color_element;
	typedef	u32			params_element;
	typedef	u32			tex_ids_element;
	typedef	u32			tc_shift_element;


public:
								terrain			( );
								~terrain		( );
	bool						has_cell		( pcstr cell_id) const;
	terrain_render_model_instance_ptr get_cell			( pcstr cell_id);

	void 			add_cell			( terrain_render_model_instance_ptr v);
	void 			remove_cell			( terrain_render_model_instance_ptr v);

	// Editor specific functions
	void 		update_cell_buffer		( terrain_render_model_instance_ptr v, vectora<render::terrain_buffer_fragment> const& fragments, float4x4 const& transform );
	void		update_cell_aabb		( terrain_render_model_instance_ptr v, math::aabb bbox );
	void 		add_cell_texture 		( terrain_render_model_instance_ptr v, fs_new::virtual_path_string const & texture, u32 tex_user_id );
	void 		remove_cell_texture 	( terrain_render_model_instance_ptr v, u32 tex_user_id );
	void 		exchange_texture		( fs_new::virtual_path_string const& old_texture, fs_new::virtual_path_string const& new_texture );

// 	void 		select_editor_cells		( float4x4 const& mat_vp, render::vector<composite_render_model_instance_ptr>& editor_cells) const;
// 	void 		select_game_cells		( float4x4 const& mat_vp, render::vector<composite_render_model_instance_ptr>& game_cells) const;
	void 		select_cells			( float4x4 const& mat_vp, render::vector<terrain_render_model_instance_ptr>& game_cells) const;
	
	
private:
	void 			generate_grids			( );
	untyped_buffer_ptr 		create_vertex_buffer	( char* buffer, u32 buffer_size, u32 quad_count, float cell_size);
	untyped_buffer_ptr 		create_index_buffer		( char* buffer, u32 buffer_size, u32 quad_count, u32 join_side_x, u32 join_side_z);

public:
	cells									m_cells;

// 	cells									m_editor_cells;
// 	cells									m_game_cells;
	terrain_texture_pool					m_texture_pool;
	effect_terrain_NEW*						m_effect;
	vertex_buffer							m_intermediate_vertex_stream;
	ref_texture								m_intermediate_height_texture;
	ref_texture								m_intermediate_color_texture;
	ref_texture								m_intermediate_params_texture;
	ref_texture								m_intermediate_tex_ids_texture;
	ref_texture								m_intermediate_tc_shift_texture;
	static const math::uint2				m_cell_size;	
	static const DXGI_FORMAT				s_height_texture_format		= DXGI_FORMAT_R16_UNORM;
	static const DXGI_FORMAT				s_color_texture_format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT				s_params_texture_format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT				s_tex_ids_texture_format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT				s_tc_shift_texture_format	= DXGI_FORMAT_R16G16_UNORM;

	static const float						s_cell_size;
	static const u32						s_quads_count_0 = 64;
	static const u32						s_quads_count_1 = s_quads_count_0/2;


	collision::space_partitioning_tree*		m_collision_tree;

	// This geometry need to be static for all terrains
	untyped_buffer_ptr								m_grid_vertices_0;
	untyped_buffer_ptr								m_grid_vertices_1;

	untyped_buffer_ptr								m_grid_indices_0;
	untyped_buffer_ptr								m_grid_indices_1;

	ref_geometry							m_grid_geom_0;
	ref_geometry							m_grid_geom_1;

//	fixed_vector<untyped_buffer_ptr,5>				m_grid_indices_1;

// 	collision::space_partitioning_tree*		m_collision_tree_editor;
// 	collision::space_partitioning_tree*		m_collision_tree_game;

}; // class terrain

void fill_textures	(	render::terrain_data const* t_data, 
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
					 terrain::tex_ids_element*		tex_ids_buffer,
					 u32		tex_ids_buffer_row_pitch,
					 terrain::tex_ids_element*		tc_shift_ids_buffer,
					 u32		tc_shift_buffer_row_pitch);



} // namespace render
} // namespace xray

#endif // #ifndef TERRAIN_MANAGER_H_INCLUDED