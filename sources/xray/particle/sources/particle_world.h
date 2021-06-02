////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_WORLD_H_INCLUDED
#define PARTICLE_WORLD_H_INCLUDED

#include <xray/particle/world.h>
#include <xray/memory_fixed_size_allocator.h>
#include "particle_system_instance_impl.h"

namespace xray {
namespace particle {

extern command_line::key		s_test_particles_key;

struct engine;
class particle_system;
struct base_particle;

class particle_world:
	public particle::world,
	private boost::noncopyable
{
	typedef xray::intrusive_list<particle_system_instance_impl, particle_system_instance_impl*, &particle_system_instance_impl::m_next> particle_instances_list_type;
	
	
public:
					particle_world						(engine& engine);
	virtual			~particle_world						();
	virtual	void	tick								( float time_delta, xray::math::float4x4 const& view_matrix );
	virtual	void	add_particle_system_instance		(particle_system_instance* instance);
	virtual	void	remove_particle_system_instance		(particle_system_instance* instance);
	virtual	void	change_material						( particle_emitter_instance& instance, resources::unmanaged_resource_ptr const& material );
	virtual void	get_render_emitter_instances		(float4x4 const& view_proj_matrix, render_particle_emitter_instances_type& out_particle_emitter_instances);
	
	virtual	void	play							( particle_system_instance_ptr particle_system_instance, xray::math::float4x4 const& transform, bool use_transform, bool always_loop );
	virtual	void	stop							( particle_system_instance_ptr particle_system_instance, float time );
	virtual	void	remove							( particle_system_instance_ptr particle_system_instance );
	virtual	void	set_transform					( particle_system_instance_ptr particle_system_instance, math::float4x4 const& transform );
	virtual	void	set_visible						( particle_system_instance_ptr particle_system_instance, bool is_visible );
	virtual	void	set_paused						( particle_system_instance_ptr particle_system_instance, bool is_paused );
	
#ifndef MASTER_GOLD
	virtual	particle_system* add					( configs::lua_config_value const& init_values, math::float4x4 const& transform );
	virtual	void	remove							( particle_system** preview_particle_system );
	virtual	void	update_preview					( particle_system** preview_particle_system, xray::configs::lua_config_value config_value );
	virtual	void	set_transform					( particle_system** preview_particle_system, math::float4x4 const& transform );
	virtual	void	set_looped						( particle_system** preview_particle_system, bool is_looped );
	virtual	void	restart							( particle_system** preview_particle_system );
	virtual	void	show							( particle_system** preview_particle_system );
	virtual	u32		get_num_preview_particle_system_emitters	( particle_system** preview_particle_system );
	virtual	void	gather_stats					( particle_system** preview_particle_system, preview_particle_emitter_info* out_info );
#endif // #ifndef MASTER_GOLD

	virtual void	get_render_emitter_instances	( particle::particle_system_instance_ptr instance, render_particle_emitter_instances_type& out_particle_emitter_instances );
	
	inline	engine&	engine								( ) const { return m_engine; }

public:
	base_particle*	allocate_particle					();
			void	deallocate_particle					(base_particle* P);
			void	set_transform_to_particle_instance	(particle_system_instance* instance, xray::math::float4x4 const& transform);
			u32		remove_dead_particles				();
			void	remove_overflowing_particles		();
			void	shrink_particles					(float time_delta,float limit_over_total, u32 num_need_particles);
			u32		calc_num_new_particles				(float time_delta);
			u32		calc_num_max_particles				(float time_delta);
			void	check_lods							(xray::math::float3 const& view_location);
			void	set_max_particles					(u32 num_max_particles);
			void	on_material_loaded					( particle_emitter_instance& emitter_instance ); // m_data, m_material 
	particle_emitter_instance* create_emitter_instance	( particle_emitter& emitter, bool is_child_emitter_instance = false );
	
private:
	typedef memory::fixed_size_allocator<
		base_particle,
		threading::mutex
	>								particle_allocator_type;
	friend class particle_world_cooker;

private:
	particle_allocator_type			m_allocator;
	particle_instances_list_type	m_ticked_instances_list;
	particle::engine&				m_engine;
	u32								m_num_particles;

public:
	u32								m_max_particles;
}; // class particle_world

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_WORLD_H_INCLUDED