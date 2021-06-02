////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_GRASS_H_INCLUDED
#define XRAY_RENDER_GRASS_H_INCLUDED

#include <xray/render/core/res_geometry.h>
#include <xray/render/core/shader_constant_defines.h>
#include <xray/render/core/shader_constant_host.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "material_effects.h"
//#include <xray/resources_fs.h>

namespace xray {
namespace render {

class grass_world;
struct grass_cell;

class renderer_context;

// TODO: public render_object_interface
struct grass_cell_mesh
{
	grass_cell_mesh(grass_world* g);
	
	void render		(renderer_context* context, u32 stage_index);

	ref_geometry	geometry;
	u32				vertex_count;
	u32				index_count;
	u32				primitive_count;
	s32				layer_index;
private:
	grass_world*	m_grass_world;
};

struct grass_cell
{
	static const u32 cell_size_xy = 20; // 50 meters
	
	grass_cell(grass_world* g);
	~grass_cell();
	
	render::vector<grass_cell_mesh*> meshes;
	render::vector<grass_cell_mesh*> meshes_lod_1;
	
	math::float2	position;
	math::float2	height_min_max;
	grass_cell*		next_visible_cell;
	
	grass_world*	get_grass_world() const { return m_grass_world; }
private:
	grass_world*	m_grass_world;
}; // struct grass_cell

struct base_grass_mesh_desc
{
	math::float3 position;
	float		 rotation;
	math::float2 size;
	math::float4 uv;		// uv map
};

struct grass_layer
{
	material_effects layer_effects;
	math::float4	 wind_data;		// x - weight
	
	// TODO: Add material name or material?
	// TODO: Add triangle mesh template.
	
}; // struct grass_layer

class grass_world
{
public:
	typedef render::vector<grass_cell*>		cells_array_type;
	typedef render::vector<grass_layer*>	grass_layer_array_type;
	
	grass_world();
	~grass_world();
	grass_layer* add_grass_layer				();
	grass_cell* add_cell						(float pos_x, float pos_y, float height_min, float height_max);
	grass_cell* get_visible_cells				(math::float4x4 const& view_proj_matrix) const;
	void		render							(renderer_context* context, u32 stage_index) const;
	grass_cell_mesh* create_base_grass_mesh		(s32 layer_index, render::vector<base_grass_mesh_desc> desc);
	grass_cell_mesh* create_base_grassbillboard_mesh(s32 layer_index, render::vector<base_grass_mesh_desc> desc);
	
	void initialize_test_grass					();
	void test_grass_data_loaded					(xray::resources::queries_result& result);
	grass_layer*								get_layer(u32 layer_index);
private:
	shader_constant_host*						m_view_to_local_parameter;
	cells_array_type							m_cells;
	grass_layer_array_type						m_grass_layers;
}; // class grass_world

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_GRASS_H_INCLUDED