////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_EMITTER_INSTANCE_H_INCLUDED
#define PARTICLE_EMITTER_INSTANCE_H_INCLUDED

#include <xray/intrusive_list.h>
#include <xray/particle/base_particle.h>

namespace xray {
namespace render{
	struct material_effects_instance_cook_data;
}
namespace particle {

class particle_action_source;
class particle_system_instance;
class particle_system_instance_impl;
class particle_emitter;
struct render_particle_emitter_instance;
class particle_world;
struct billboard_parameters;
struct beamtrail_parameters;


class particle_system_instance;
typedef	resources::resource_ptr<
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_instance_ptr;

enum enum_particle_event;
enum enum_particle_data_type;

class particle_action_data_type;
class particle_event;

struct engine;

class particle_emitter_instance : private boost::noncopyable {
public:
					particle_emitter_instance	(
						particle_world& particle_world,
						particle_emitter& emitter,
						engine& engine,
						bool is_child_emitter_instance
					);
	virtual			~particle_emitter_instance	( );
	
	virtual	void create_render_particle_emitter_instance ( engine& engine );
			
			void	play_child					( particle_event* evt, xray::math::float4x4 const& transform );
			
	virtual void	tick						( float time_delta, bool create_new_particles, float alpha );
	virtual void	remove_overflowing_particles( );
	virtual u32		calc_num_new_particles		( float time_delta, bool allow_to_create_new );
	virtual u32		calc_num_max_particles		( float time_delta );
	virtual void	shrink_particles			( float time_delta, float limit_over_total, u32 num_need_particles );
	virtual bool	is_finished					( ) const;
	virtual	u32		get_num_sheets				( ) const { return 0; }

			void	change_material				( resources::unmanaged_resource_ptr const& material );
			float	get_max_particle_lifetime	() const;
			float	get_current_duration		() const { return m_current_duration; }
			u32		get_current_max_particles	() const;
			float	get_current_create_rate		() const { return m_current_create_rate; }
			
			float	get_emitter_time			() const { return m_emitter_time; }
			float	get_linear_emitter_time		() const;
			
			float	get_particle_system_time	() const; // TODO: return particle system time
			
	inline	render_particle_emitter_instance& render_instance ( ) const { return *m_render_instance; }

			void	set_transform				( xray::math::float4x4 const& transform );
			void	set_visible					( bool visible );
			bool	get_visible					( ) const;
			void	remove_particles			( u32 num = (u32)-1 );
			u32		remove_dead_particles		( );
			bool	is_emitter_finished			( ) const;
			void	append_particles			( float time_delta );
			void	process_event				( enum_particle_event const& event_type, xray::float3 const& location );
			void	load_material				( pcstr material_name );
			void	recalc_duration				( );
			void	update_render_buffers		( enum_particle_data_type const& datatype, bool use_subuv );

	inline	particle_action_data_type* get_data_type_action ( ) const { return m_data_type_action;}

private:
			void	on_material_loaded			( resources::queries_result& result );

	friend class particle_world;
	friend class particle_system_instance_impl;

public:
	math::float4x4					m_transform;
	math::aabb						m_aabbox;

protected:
	math::float4					m_instance_color;
	particle_list_type				m_particle_list;
	xray::particle::engine&			m_engine;

public:
	math::uint2						m_subuv_pos_uv;
	math::float2					m_subuv_size_uv;

public:
	resources::unmanaged_resource_ptr	m_material;
	
protected:
	particle_world&					m_particle_world;

public:
	particle_system_instance_ptr	m_particle_system_instance_ptr;
	particle_system_instance_impl*	m_particle_system_instance;
	
	render_particle_emitter_instance*	m_render_instance;

protected:
	particle_action_data_type*		m_data_type_action;
	
	xray::render::material_effects_instance_cook_data* m_cook_data_to_delete;
	
public:
	particle_emitter&				m_emitter;

protected:
	billboard_parameters*			m_billboard_parameters;
	beamtrail_parameters*			m_beamtrail_parameters;

public:
	particle_emitter_instance*		m_next;

public:
	u32								m_num_live_particles;

protected:
	u32								m_num_created_particles;

public:
	// Number of created particles in current loop.
	float							m_delay_time;

protected:
	float							m_emitter_time;

protected:
	float							m_emitter_time_numerator;
	
public:
	u32								m_current_loop;
	
private:
	float							m_time_to_create_new_one;

protected:
	u32								m_num_particles_to_create;
	u32								m_current_max_num_particles;
	
private:
	u32								m_current_calc_num_max_particles;
	//float							m_min_lifetime_numerator;
	
	float							m_create_rate;
	float							m_current_create_rate;
	
	float							m_current_duration;

public:
	float							m_subimage_index;
	
public:
	u32								m_max_num_particles;
	
public:
	bool							m_is_child_emitter_instance;
	bool							m_waiting_for_end;
	bool							m_delayed;
private:
	bool							m_visible;
	bool							m_particle_added;

protected:
	bool							m_world_space;
}; // class particle_emitter_instance

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_EMITTER_INSTANCE_H_INCLUDED