////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_ENTITY_TYPES_H_INCLUDED
#define PARTICLE_ENTITY_TYPES_H_INCLUDED

namespace xray {
namespace particle {

enum enum_particle_entity_type
{
	particle_entity_root_type = 0,
	particle_entity_particle_group_type,
	particle_entity_emitter_type,
	
	particle_action_initial_domain_type,
	particle_action_initial_color_type,
	particle_action_initial_velocity_type,
	particle_action_initial_size_type,
	particle_action_initial_rotation_type,
	particle_action_initial_rotation_rate_type,
	
	particle_action_billboard_type,
	particle_action_mesh_type,
	particle_action_trail_type,
	particle_action_beam_type,
	particle_action_decal_type,
	
	particle_action_target_color_type,
	particle_action_color_over_lifetime_type,
	
	particle_action_target_velocity_type,
	particle_action_velocity_over_lifetime_type,
	particle_action_random_velocity_type,
	
	particle_action_target_rotation_type,
	particle_action_rotation_over_lifetime_type,
	particle_action_rotation_over_velocity_type,
	
	particle_action_target_size_type,
	particle_action_size_over_lifetime_type,
	particle_action_size_over_velocity_type,
	
	particle_action_on_death_type,
	particle_action_on_birth_type,
	particle_action_on_collide_type,
	particle_action_on_play_type,
	
	particle_action_orbit_type,
	
	particle_action_acceleration_type,
	particle_action_kill_volume_type,
	// Append new action types here.
	
	
	particle_action_unsupported_type,
}; // enum enum_particle_entity_type

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_ENTITY_TYPES_H_INCLUDED