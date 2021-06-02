////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_INSTANCE_IMPL_H_INCLUDED
#define PARTICLE_SYSTEM_INSTANCE_IMPL_H_INCLUDED

#include "particle_system_instance.h"

namespace xray {
namespace particle {

class particle_system;
struct world;
struct particle_system_lod;

class particle_system_instance_impl : public particle_system_instance {
public:
	particle_system_instance_impl			(particle_world& particle_world);
	virtual ~particle_system_instance_impl	();
	
	bool tick								(float time_delta);
	void set_template						(u32 lod_index, particle_system_ptr templ);
	
	virtual bool is_finished				();
	
	void apply_lod							(u32 lod_index);
	void play_impl							();
	void play_impl							(xray::math::float4x4 const& transform);
	void stop_impl							(float time_to_stop);
	
	void set_pause							(bool pause);
	
	void set_visible						(bool visible);
	bool get_visible						() const {return m_visible;}

	void set_ticked							(bool ticked);
	bool get_ticked							() const;

	void process_lods_lerping				(float time_delta);
	
	void add_emitter_instance				(u32 lod_index, particle_emitter_instance* new_instance);
	void set_transform						(xray::math::float4x4 const& transform);
	
	u32  calc_num_new_particles				(float time_delta);
	u32  calc_num_max_particles				(float time_delta);
	
	u32	 remove_dead_particles				();
	void remove_overflowing_particles		();
	void shrink_particles					(float time_delta, float limit_over_total, u32 num_need_particles);
	
	void check_lods							(xray::math::float3 const& view_location);
	
	void remove_emitter_instances			();
	
	void remove_particles					();

	void pin								();
	void unpin								();

	float get_particle_system_time			() const { return m_particle_system_time;}
	
protected:
	friend class particle_world;
	
//!	virtual u32 get_particle_emitter_info_count();
//!	virtual void get_particle_emitter_info(particle_emitter_info* out_info_array);

protected:
	xray::math::float4x4					m_transform;
	
public:
	// Lods.
	particle_system_instance_impl*			m_next;	
	u32										m_current_lod;
	u32										m_old_lod;
	u32										m_num_lods;
	
	float									m_particle_system_time;
	// 0.0f -> 1.0f, from previous lod to current.
	float									m_lods_lerp_alpha;
	bool									m_lerped;
	bool									m_use_lods;
	bool									m_always_looping;
	
	bool									m_child_played;
	
protected:
	
	// Used for saving reference in particle_world.
	particle_system_instance_ptr			self_ptr;
	
	bool									m_pinned;
	bool									m_is_playing;
	bool									m_no_more_create;
	bool									m_paused;
	bool									m_visible;
	bool									m_ticked;
}; // class particle_system_instance

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_INSTANCE_IMPL_H_INCLUDED