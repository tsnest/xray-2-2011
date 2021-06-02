////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_WORLD_H_INCLUDED
#define XRAY_PARTICLE_WORLD_H_INCLUDED

namespace xray {
namespace particle {

struct preview_particle_emitter_info {
	fixed_string<64> emitter_name;
	float			 current_emitter_time;
	float			 emitter_duration;
	u32				 num_live_particles;
	u32				 num_current_max_particles;
	u32				 num_fire_events;
	bool			 enabled;
}; // struct preview_particle_emitter_info

class particle_system_instance;
class particle_emitter_instance;
struct render_particle_emitter_instance;

typedef vectora< render_particle_emitter_instance* >	render_particle_emitter_instances_type;

struct particle_emitter_instance_data { }; // struct particle_emitter_instance_data

typedef	resources::resource_ptr <
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_ptr;

class particle_system;

typedef	resources::resource_ptr <
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_instance_ptr;

struct XRAY_NOVTABLE world : public resources::unmanaged_resource {
	virtual	void	tick							( float time_delta, xray::math::float4x4 const& view_matrix ) = 0;
	virtual void	get_render_emitter_instances	( float4x4 const& view_proj_matrix, render_particle_emitter_instances_type& out_particle_emitter_instances ) = 0;
	virtual	void	add_particle_system_instance	( particle_system_instance* instance ) = 0;
	virtual	void	remove_particle_system_instance	( particle_system_instance* instance ) = 0;
	virtual	void	change_material					( particle_emitter_instance& instance, resources::unmanaged_resource_ptr const& material ) = 0;
	
	virtual	void	play							( particle_system_instance_ptr particle_system_instance, xray::math::float4x4 const& transform, bool use_transform, bool always_loop) = 0;
	virtual	void	stop							( particle_system_instance_ptr particle_system_instance, float time ) = 0;
	virtual	void	remove							( particle_system_instance_ptr particle_system_instance ) = 0;
	virtual	void	set_transform					( particle_system_instance_ptr particle_system_instance, math::float4x4 const& transform ) = 0;
	virtual	void	set_visible						( particle_system_instance_ptr particle_system_instance, bool is_visible ) = 0;
	virtual	void	set_paused						( particle_system_instance_ptr particle_system_instance, bool is_paused ) = 0;

#ifndef MASTER_GOLD
	virtual	particle_system* add					( configs::lua_config_value const& init_values, math::float4x4 const& transform ) = 0;
	virtual	void	remove							( particle_system** preview_particle_system ) = 0;
	virtual	void	update_preview					( particle_system** preview_particle_system, xray::configs::lua_config_value config_value ) = 0;
	virtual	void	set_transform					( particle_system** preview_particle_system, math::float4x4 const& transform ) = 0;
	virtual	void	set_looped						( particle_system** preview_particle_system, bool is_looped ) = 0;
	virtual	void	restart							( particle_system** preview_particle_system ) = 0;
	virtual	void	show							( particle_system** preview_particle_system ) = 0;
	virtual	u32		get_num_preview_particle_system_emitters	( particle_system** preview_particle_system ) = 0;
	virtual	void	gather_stats					( particle_system** preview_particle_system, preview_particle_emitter_info* out_info ) = 0;
#endif // #ifndef MASTER_GOLD

	virtual void	get_render_emitter_instances	( particle::particle_system_instance_ptr instance, render_particle_emitter_instances_type& out_particle_emitter_instances ) = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world );
}; // class world

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_WORLD_H_INCLUDED