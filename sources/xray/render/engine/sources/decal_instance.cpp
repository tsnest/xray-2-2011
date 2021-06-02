////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "decal_instance.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_texture.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/resource_manager.h>
#include "material.h"
#include <xray/collision/space_partitioning_tree.h>
#include "material_manager.h"
#include <xray/collision/api.h>
#include "material_effects.h"
#include <xray/geometry_primitives.h>
#include "geometry_type.h"
#include "renderer_context.h"
#include "effect_decal_mask.h"

namespace xray {
namespace render {


decal_shader_constants_and_geometry::decal_shader_constants_and_geometry()
{
	m_world_to_decal_parameter						= backend::ref().register_constant_host("world_to_decal", rc_float);
	m_eye_ray_corner_parameter						= backend::ref().register_constant_host("s_eye_ray_corner", rc_float);
	m_decal_tangent_to_view_space_matrix_parameter	= backend::ref().register_constant_host("decal_tangent_to_view_space_matrix", rc_float);
	m_decal_angle_parameters						= backend::ref().register_constant_host("decal_angle_parameters", rc_float);
	create_decal_geometry							();
}

void decal_shader_constants_and_geometry::set(xray::render::renderer_context* context,
											  math::float4x4 const&	world_to_decal_matrix,
											  math::float4x4 const&	decal_tangent_to_view_space_matrix,
											  float alpha_angle,
											  float clip_angle,
											  math::float3 const& decal_width_height_far_distance,
											  math::float4x4 const& decal_transform)
{
	float3 const* const eye_rays	= context->get_eye_rays();
	backend::ref().set_ps_constant	(m_world_to_decal_parameter,						math::transpose(world_to_decal_matrix));
	backend::ref().set_ps_constant	(m_decal_tangent_to_view_space_matrix_parameter,	math::transpose(decal_tangent_to_view_space_matrix));
	backend::ref().set_ps_constant	(m_decal_angle_parameters,							float4(alpha_angle, clip_angle, 0.0f, 0.0f));
	backend::ref().set_ps_constant	(m_eye_ray_corner_parameter,						((float4*)eye_rays)[0]);
	
	// TODO: optimize or remove it
	float3 const scale					= decal_width_height_far_distance;
	float3 const decal_direction		= math::normalize_safe(decal_transform.k.xyz(), float3(0.0f, 0.0f, 1.0f));
	float3 const position				= decal_transform.c.xyz() + decal_direction * scale.z;
	float3 const angles					= decal_transform.get_angles_xyz();
	
	float4x4 const geom_world_matrix	= 
		math::create_scale(scale) * 
		math::create_rotation(angles) * 
		math::create_translation(position);
	
	//context->set_w						(geom_world_matrix);
}

void decal_shader_constants_and_geometry::set_geometry()
{
	m_decal_geometry->apply();
}

void decal_shader_constants_and_geometry::create_decal_geometry()
{
	m_decal_vertex_buffer			= resource_manager::ref().create_buffer(
		geometry_utils::cube_solid::vertex_count*sizeof(float3),
		geometry_utils::cube_solid::vertices,
		enum_buffer_type_vertex,
		false
	);
	
	u32 const buffer_size			= geometry_utils::cube_solid::index_count*sizeof(u16);
	u16* const indices				= static_cast<u16*>(ALLOCA(buffer_size));
	std::copy						(
		geometry_utils::cube_solid::faces, 
		geometry_utils::cube_solid::faces + geometry_utils::cube_solid::index_count, 
		indices
	);
	m_decal_index_buffer			= resource_manager::ref().create_buffer(
		geometry_utils::cube_solid::index_count*sizeof(u16),
		indices,
		enum_buffer_type_index,
		false
	);
	
	D3D_INPUT_ELEMENT_DESC layout[]	= {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0 }
	};
	
	m_decal_geometry				= resource_manager::ref().create_geometry(
		layout,
		sizeof(float3),
		*m_decal_vertex_buffer,
		*m_decal_index_buffer
	);
}



decal_instance::decal_instance	(collision::space_partitioning_tree* tree, 
								 decal_properties const& properties, 
								 u32 const id):
	m_collision_tree			(tree),
	m_collision_geometry		(0),
	m_collision_object			(0),
	m_id						(id),
	m_next						(0)
{
	set_properties				(properties);
	
	render::material_effects_instance_ptr& ptr	= (render::material_effects_instance_ptr&)m_properties.material;
	
	if (ptr.c_ptr())
		material_manager::ref().add_material_effects(
		ptr,
		ptr->get_material_name().c_str()
	);
}

decal_instance::~decal_instance()
{
	remove_collision			();
	
	render::material_effects_instance_ptr& ptr		= (render::material_effects_instance_ptr&)m_properties.material;
	material_manager::ref().remove_material_effects	(ptr);
}

void decal_instance::remove_collision	()
{
	if (m_collision_tree && m_collision_object)
		m_collision_tree->erase			(m_collision_object);

	collision::delete_object			(g_allocator, m_collision_object);
	collision::delete_geometry_instance	(g_allocator, m_collision_geometry);
}

void decal_instance::set_materail_effects(resources::unmanaged_resource_ptr const& in_ptr)
{
	render::material_effects_instance_ptr& ptr_old	= (render::material_effects_instance_ptr&)m_properties.material;
	
	material_manager::ref().remove_material_effects	(ptr_old);
	
	m_properties.material		= in_ptr;
	
	render::material_effects_instance_ptr& ptr	= (render::material_effects_instance_ptr&)m_properties.material;
	
	material_manager::ref().add_material_effects(
		ptr,
		ptr->get_material_name().c_str()
	);
}

void decal_instance::set_properties(decal_properties const& in_properties)
{
	if (in_properties.material.c_ptr() && m_properties.material.c_ptr())
	{
		material_effects_instance_ptr this_ptr	= static_cast_resource_ptr<material_effects_instance_ptr>(m_properties.material);
		material_effects_instance_ptr other_ptr = static_cast_resource_ptr<material_effects_instance_ptr>(in_properties.material);
		
		if (this_ptr->get_material_name() != other_ptr->get_material_name())
		{
			set_materail_effects		(in_properties.material);
		}
	}
	else
	{
		m_properties.material			= in_properties.material;
		if (m_properties.material)
			set_materail_effects		(in_properties.material);
	}
	
	m_properties.transform							= in_properties.transform; // no scale
	m_properties.width_height_far_distance			= in_properties.width_height_far_distance;
	m_properties.alpha_angle						= in_properties.alpha_angle;
	m_properties.clip_angle							= in_properties.clip_angle;
	m_properties.projection_on_terrain_geometry		= in_properties.projection_on_terrain_geometry;
	m_properties.projection_on_static_geometry		= in_properties.projection_on_static_geometry;
	m_properties.projection_on_speedtree_geometry	= in_properties.projection_on_speedtree_geometry;
	m_properties.projection_on_skeleton_geometry	= in_properties.projection_on_skeleton_geometry;
	m_properties.projection_on_particle_geometry	= in_properties.projection_on_particle_geometry;
	
	remove_collision					();
	float4x4 m							= m_properties.transform;
	m.set_scale							(float3(1.0f, 1.0f, 1.0f));
	m_collision_geometry				= &*collision::new_box_geometry_instance(g_allocator, math::create_scale( 2.0f * in_properties.width_height_far_distance ));
	m_collision_object					= &*collision::new_collision_object(
		g_allocator,
		1,
		m_collision_geometry,
		this
	);
	float4x4 new_transform				= m_properties.transform;
	new_transform.set_scale				( max( m_properties.transform.get_scale(), 2*float3( 1.f, 1.f, 1.f ) ) );
	m_collision_tree->insert			(m_collision_object, new_transform );
}

decal_properties const& decal_instance::get_properties() const
{
	return m_properties;
}

material_effects const& decal_instance::get_effects() const
{
	material_effects_instance_ptr ptr = static_cast_resource_ptr<material_effects_instance_ptr>(m_properties.material);
	
	// TODO:
	if (ptr.c_ptr() == NULL)
	{
		return material::nomaterial_material(decal_vertex_input_type);
	}
	
	return ptr->get_material_effects();
}


static math::float4x4 get_decal_view_matrix(decal_instance* decal)
{
	float4x4 decal_world_matrix				= decal->get_properties().transform;
	decal_world_matrix.set_scale			(float3(1.0f, 1.0f, 1.0f));
	
	float3 const scale						= decal->get_properties().width_height_far_distance;//decal->get_properties().transform.get_scale();
	float3 const decal_direction			= math::normalize_safe(decal_world_matrix.k.xyz(), float3(0.0f, 0.0f, 1.0f));
	decal_world_matrix.c					= float4(decal_world_matrix.c.xyz() - decal_direction * scale.z, decal_world_matrix.c.w);
	
	float4x4 decal_view_matrix				= math::float4x4().identity();
	decal_view_matrix.try_invert			(decal_world_matrix);
	
	return decal_view_matrix;
}

static math::float4x4 get_world_to_decal_matrix(decal_instance* decal)
{
	float4x4 decal_world_matrix				= decal->get_properties().transform;
	decal_world_matrix.set_scale			(float3(1.0f, 1.0f, 1.0f));
	
	float3 const scale						= decal->get_properties().width_height_far_distance;//decal->get_properties().transform.get_scale();
	float3 const decal_direction			= math::normalize_safe(decal_world_matrix.k.xyz(), float3(0.0f, 0.0f, 1.0f));
	decal_world_matrix.c					= float4(decal_world_matrix.c.xyz() - decal_direction * scale.z, decal_world_matrix.c.w);
	float4x4 const decal_projection_matrix	= math::create_orthographic_projection(
		2.0f * scale.x, 
		2.0f * scale.y, 
		0.01f, 
		2.0f * scale.z
	);
	
	float4x4 decal_view_matrix				= math::float4x4().identity();
	decal_view_matrix.try_invert			(decal_world_matrix);
	float4x4 const world_to_decal_matrix	= decal_view_matrix * decal_projection_matrix;
	
	return world_to_decal_matrix;
}

void decal_instance::render_geometry()
{
	decal_shader_constants_and_geometry::ref().set_geometry();
	
	backend::ref().render_indexed			(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 
		geometry_utils::cube_solid::index_count,
		0,
		0
	);
}

void decal_instance::render(xray::render::renderer_context* context, 
							enum_render_stage_type stage_type)
{
	ASSERT(stage_type == decals_accumulate_render_stage || stage_type == forward_render_stage);

	for (u32 pass_index = 0; pass_index < 2; pass_index++)
	{
		get_effects().m_effects[stage_type]->apply(pass_index);
		
		decal_shader_constants_and_geometry::ref().set(
			context, 
			get_world_to_decal_matrix(this), 
			get_decal_view_matrix(this), 
			get_properties().alpha_angle,
			get_properties().clip_angle,
			get_properties().width_height_far_distance,
			get_properties().transform
		);
		
		render_geometry();
	}
}


static bool is_progect_decal_on_type(decal_instance* decal, enum_geometry_type in_type)
{
	switch (in_type)
	{
	case terrain_geometry_type:		return decal->get_properties().projection_on_terrain_geometry;
	case static_geometry_type:		return decal->get_properties().projection_on_static_geometry;
	case speedtree_geometry_type:	return decal->get_properties().projection_on_speedtree_geometry;
	case skeleton_geometry_type:	return decal->get_properties().projection_on_skeleton_geometry;
	case particle_geometry_type:	return decal->get_properties().projection_on_particle_geometry;
	default: NODEFAULT( return false; );
	};
}

u32 decal_instance::draw(xray::render::renderer_context* context,
						  ref_effect opaque_geometry_mask_effect,
						  enum_render_stage_type stage_type)
{
	u32 num_draw_calls						= 0;
	
	decal_instance* decal					= this;
	
	bool const valid_decal					= decal->get_effects().stage_enable[stage_type] &&
											  decal->get_effects().m_effects[stage_type] &&
											  decal->get_properties().material.c_ptr();
	if (valid_decal)
	{
		num_draw_calls						= 1;
		
		float3 const scale					= decal->get_properties().width_height_far_distance;
		float3 const decal_direction		= math::normalize_safe(decal->get_properties().transform.k.xyz(), float3(0.0f, 0.0f, 1.0f));
		float3 const position				= decal->get_properties().transform.c.xyz() + decal_direction * scale.z;
		float3 const angles					= decal->get_properties().transform.get_angles_xyz();
		
		float4x4 const geom_world_matrix	= math::create_scale(scale) * math::create_rotation(angles) * math::create_translation(position);
		
		context->set_w						(geom_world_matrix);
		
		bool const project_on_all			= decal->get_properties().is_project_on_all();
		
		if (project_on_all)
		{
			opaque_geometry_mask_effect->apply(effect_decal_mask::all_types);
			decal->render_geometry();
			decal->render(context, stage_type);
		}
		else if (decal->get_properties().is_project_on_something())
		{
			for (u32 geom_type_index = 1; geom_type_index < num_geometry_types + 1; geom_type_index++)
			{
				if (!is_progect_decal_on_type(decal, (enum_geometry_type)geom_type_index))
					continue;
				opaque_geometry_mask_effect->apply(effect_decal_mask::specific_type);
				backend::ref().set_stencil_ref	(all_geometry_type + geom_type_index);
				decal->render_geometry();
				decal->render(context, stage_type);
				
				num_draw_calls++;
			}
		}
	}
	return num_draw_calls;
}

} // namespace render
} // namespace xray
