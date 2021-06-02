////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_BEAM_EMITTER_INSTANCE_H_INCLUDED
#define PARTICLE_BEAM_EMITTER_INSTANCE_H_INCLUDED

#include "particle_emitter_instance.h"
#include "curve_line.h"

namespace xray {
namespace particle {

struct beamtrail_parameters;
class particle_action_beam;

class particle_beam_emitter_instance : public particle_emitter_instance {
public:
					particle_beam_emitter_instance	(
						particle_world& particle_world,
						particle_emitter& emitter,
						engine& engine,
						bool is_child_emitter_instance
					);
	virtual			~particle_beam_emitter_instance	( );
	virtual	void create_render_particle_emitter_instance ( engine& engine );

	virtual void	tick							( float time_delta, bool create_new_particles, float alpha );
	virtual u32		calc_num_new_particles			( float time_delta, bool allow_to_create_new );
	virtual u32		calc_num_max_particles			( float time_delta );
	virtual void	shrink_particles				( float time_delta, float limit_over_total, u32 num_need_particles );
	virtual bool	is_finished						( ) const;
//	virtual void	set_emitter						( particle_emitter* emitter );
	virtual	u32		get_num_sheets					( ) const;
	
private:
	void alloc_dynamic_data					  (u32 num_curves, u32 num_points);
	void free_dynamic_data					  ();
	void set_particles_positions			  (bool gen_new);
	void generate_offsets					  ();

	void apply_noise						  (u32 beam_index, xray::math::float3 const& up_vector, xray::math::float3 const& right_vector, float noise_power, u32 step, bool first_fill, base_particle* P, u32 num_particles);
	
private:
	enum {
		num_base_noise_positions = 15,
	};
	typedef math::float3 offsets_array_type[num_base_noise_positions];

private:
	math::float3			m_beams_source_position;
	math::float3			m_beams_target_position;
	math::float3			m_beams_direction;
	math::float3			m_beams_end_position;
	//beamtrail_parameters*	m_beamtrail_parameters;
	particle_action_beam*	m_particle_action_beam;
	curve_line_float3*		m_curve_line;
	offsets_array_type*		m_random_offsets;
	u32						m_num_curves;
	float					m_move_from_source_to_target;
	float					m_beam_pose_index;
	float					m_particle_beam_index;
	float					m_beam_lifetime;
	bool					m_beam_particles_allocated;
}; // class particle_beam_emitter_instance

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_BEAM_EMITTER_INSTANCE_H_INCLUDED