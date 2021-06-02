////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_RENDER_MESH_H_INCLUDED
#define TERRAIN_RENDER_MESH_H_INCLUDED

#include <xray/render/facade/common_types.h>
#include <xray/render/core/untyped_buffer.h>
#include "render_model.h"
#include <xray/physics/collision_shapes.h>

namespace xray {
namespace render {

class terrain;
class res_texture;
struct effect_loader;

class terrain_render_model : public render_model 
{
	friend class terrain;
	friend struct terrain_mesh_cook;

	typedef	u16			heightmap_element;
	typedef	u32			color_element;
	typedef	u32			params_element;
	typedef	u32			tex_ids_element;
	typedef	u32			tc_shift_element;

public:
	typedef	fixed_vector<int, render::terrain_texture_max_count>	terrain_textures_type;

public:
									terrain_render_model	( );
	virtual							~terrain_render_model	( );
			void					load					( memory::chunk_reader& r, render::terrain& terrain );
	inline	math::aabb const&		aabb					( )		{ return m_aabbox;}
	inline	float					physical_size			( )		{ return m_physical_size;}
	inline	u32						vertex_row_size			( )		{ return m_vertex_row_size;}
	inline	terrain_textures_type const& textures			( )		{ return m_textures;}
	inline	render_geometry const & render_geometry			( )		{ return m_render_geometry;}
			res_effect* 			effect					( );

	inline	collision::geometry_instance*		collision_geom		( )		{ return m_collision_geom; }
	inline	physics::bt_collision_shape_ptr		bt_collision_shape	( )		{ return m_bt_collision_shape; }

	inline	float4x4 const &		transform				( )		{ return m_transform;}
	inline	res_texture*			height_texture			( )		{ return &*m_height_texture;}
	inline	res_texture*			color_texture			( )		{ return &*m_color_texture;}
	inline	res_texture*			params_texture			( )		{ return &*m_params_texture;}
	inline	res_texture*			tex_ids_texture			( )		{ return &*m_tex_ids_texture;}
	inline	res_texture*			tc_shift_texture		( )		{ return &*m_tc_shift_texture;}
private:
			void					create_maps				( math::uint2 size, heightmap_element * heightmap_buffer, color_element * color_buffer, params_element * params_buffer, tex_ids_element* tex_ids_buffer, tc_shift_element* tc_shift_buffer);
			void					load_from_reader		( memory::reader& r, render::terrain& terrain );

private:
	terrain_textures_type			m_textures;
	u32 							m_vertex_row_size;
	float 							m_physical_size;
	fixed_string<16>				m_name;
	render::render_geometry			m_render_geometry;
	untyped_buffer_ptr 				m_vb;
	ref_effect						m_effect;
	effect_loader*					m_effect_loader;
	ref_texture						m_height_texture;
	ref_texture						m_color_texture;
	ref_texture						m_params_texture; // alphas
	ref_texture						m_tex_ids_texture; // Texture indexes
	ref_texture						m_tc_shift_texture;

	float4x4							m_transform;
//	math::aabb							m_aabbox;
	float*								m_heightfield;
	collision::geometry_instance*		m_collision_geom;
	physics::bt_collision_shape_ptr		m_bt_collision_shape;
};

typedef	resources::resource_ptr<
	terrain_render_model,
	resources::unmanaged_intrusive_base
> terrain_render_model_ptr;

class terrain_render_model_instance : public render_model_instance_impl
{
	typedef render_model_instance_impl	super;
public:
							terrain_render_model_instance	( )	{}
	virtual math::aabb		get_aabb						( ) {return m_terrain_model->aabb();}

	terrain_render_model_ptr	m_terrain_model;
};

typedef	resources::resource_ptr <
	terrain_render_model_instance,
	resources::unmanaged_intrusive_base
> terrain_render_model_instance_ptr;

} // namespace render 
} // namespace xray 

#endif // #ifndef TERRAIN_RENDER_MESH_H_INCLUDED
