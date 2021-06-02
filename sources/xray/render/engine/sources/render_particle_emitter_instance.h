////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED
#define RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED

#include <xray/particle/render_particle_emitter_instance.h>
#include <xray/particle/base_particle.h>
#include <xray/render/core/vertex_buffer.h>
#include <xray/render/core/index_buffer.h>
#include "material_effects.h"

namespace xray {

namespace particle {

	class particle_emitter_instance;
	enum enum_particle_render_mode;
	enum enum_particle_vertex_type
	{
		particle_vertex_type_billboard,
		particle_vertex_type_billboard_subuv,
		particle_vertex_type_trail,
		particle_vertex_type_beam,
		particle_vertex_type_decal,
		particle_vertex_type_unknown,
	};
} // namespace particle

namespace render {

class res_geometry;
class resource_intrusive_base;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

struct material_effects_instance;
typedef	resources::resource_ptr<
	material_effects_instance, 
	resources::unmanaged_intrusive_base
> material_effects_instance_ptr;

class render_particle_emitter_instance : public particle::render_particle_emitter_instance {
public:
			render_particle_emitter_instance	(
				particle::world& particle_world,
				particle::particle_emitter_instance& particle_emitter_instance,
				particle::particle_list_type const& particle_list,
				particle::billboard_parameters* billboard_parameters,
				particle::beamtrail_parameters* beamtrail_parameters,
				particle::enum_particle_locked_axis locked_axis,
				particle::enum_particle_screen_alignment screen_alignment,
				math::float4x4 const& transform,
				math::float4 const& instance_color
			);

	virtual ~render_particle_emitter_instance	( );

	void	check_resize_render_buffers			( );
	
	void	render_sprites						( );
	void	render_subuv_sprites				( );
	void	render_trails						( math::float3 const& view_location, particle::base_particle* start_particle, u32 num_particles );
	void	render_beams						( math::float3 const& view_location, u32 num_particles );
	void	render_meshes						( );
	void	render								( math::float3 const& view_location, u32 const num_particles );
	
	void	draw_debug							( math::float4x4 const& view_matrix, particle::enum_particle_render_mode debug_mode );
	
	xray::render::enum_vertex_input_type		get_vertex_input_type ( );
	
	virtual	void	update_render_buffers		(
						particle::enum_particle_data_type datatype,
						bool use_subuv,
						u32 in_num_max_particles,
						u32 beamtrail_parameters_num_sheets
					);
	virtual	void	change_material				(
						resources::unmanaged_resource_ptr const& material
					);
	virtual	void	set_transform				( math::float4x4 const& transform );
	virtual	void	set_aabb					( math::aabb const& bbox );
	
	inline	particle::enum_particle_locked_axis			locked_axis		( ) const { return m_locked_axis; }
	inline	particle::enum_particle_screen_alignment	screen_alignment( ) const { return m_screen_alignment; }
	inline	math::float4x4 const&						transform		( ) const { return m_transform; }
	inline	particle::world&							particle_world	( ) const { return m_particle_world; }

	u32		get_num_particles					( ) const;
	bool	is_sprite_based_geometry			( ) const;
	
	inline	particle::enum_particle_vertex_type vertex_type ( ) const { return m_vertex_type; }
			pcstr material_name					( ) const;
	inline	particle::particle_emitter_instance& particle_emitter_instance( ) const { return m_particle_emitter_instance; }
	
	material_effects&					get_material_effects();
	
	math::aabb const&  get_aabb					( ) const;
	
private:
	material_effects							m_material_effects;
	material_effects_instance_ptr				m_material_effects_ptr;
	math::aabb									m_bbox;

private:
	math::float4x4								m_transform;
	ref_geometry								m_particle_sprite_geometry;
	ref_geometry								m_subuv_particle_sprite_geometry;
	ref_geometry								m_particle_beamtrail_geometry;
	uninitialized_reference< vertex_buffer >	m_vertices;
	uninitialized_reference< index_buffer >		m_indices;
	particle::particle_emitter_instance&		m_particle_emitter_instance;
	particle::particle_list_type const&			m_particle_list;
	particle::world&							m_particle_world;
	particle::billboard_parameters*				m_billboard_parameters;
	particle::beamtrail_parameters*				m_beamtrail_parameters;
	math::float4 const&							m_instance_color;
	particle::enum_particle_vertex_type			m_vertex_type;
	particle::enum_particle_locked_axis			m_locked_axis;
	particle::enum_particle_screen_alignment	m_screen_alignment;
	u32											m_num_vertices;
	u32											m_num_indices;
	u32											m_max_particles;
}; // class render_particle_emitter_instance

} // namespace render
} // namespace xray

#endif // #ifndef RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED
