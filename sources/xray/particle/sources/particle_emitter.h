////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_EMITTER_H_INCLUDED
#define PARTICLE_EMITTER_H_INCLUDED

#include "curve_line.h"
#include "color_matrix.h"

namespace xray {
namespace particle {

class particle_system;
class particle_action;
class particle_emitter_instance;
class particle_world;
class particle_action_source;
class particle_action_data_type;
class particle_event;

struct burst_entry
{
	float	time;
	s32		count;
	s32		count_variance;
}; // struct burst_entry

class particle_emitter:	public boost::noncopyable {
public:
					particle_emitter	( );
			void	set_defaults		( );
			void	add_action			( particle_action* action );
			void	remove_action		( particle_action* action );
	
	template< class ConfigValueType >
	inline	void	load				( ConfigValueType const& prop_config );
	
			void	set_source_action	( particle_action_source* source_act );
			void	set_data_type_action( particle_action_data_type* data_type_act );
	
	inline	particle_action_source*	get_source_action		( ) const { return m_source_action;}
	inline	particle_action_data_type* get_data_type_action ( ) const { return m_data_type_action;}
	inline	bool	get_world_space		( ) const { return m_world_space;}
	inline	u32		get_num_loops		( ) const { return m_num_loops; }
	inline	u32	 get_num_max_particles	( ) const { return m_max_num_particles; }
	inline	float	get_duration		( ) const { return m_duration; }

	inline bool		get_enable			( ) const { return m_visibility;}

			u32		save_binary			( mutable_buffer& buffer, bool calc_size );
			void	load_binary			( mutable_buffer& buffer );
			void	load_material		( pcstr material_name );
			void	on_material_loaded	( resources::queries_result& result );

private:
	friend class particle_system_cooker;
	friend class particle_world;
	friend class particle_system;
	friend class particle_emitter_instance;
	friend class particle_beam_emitter_instance;

private:
	typedef xray::platform_pointer<particle_action, xray::platform_pointer_default>::type			particle_action_ptr_type;
	typedef xray::platform_pointer<particle_event, xray::platform_pointer_default>::type			particle_event_ptr_type;
	typedef xray::platform_pointer<particle_action_data_type, xray::platform_pointer_default>::type particle_action_data_type_ptr_type;
	typedef xray::platform_pointer<particle_action_source, xray::platform_pointer_default>::type	particle_action_source_ptr_type;
	typedef xray::platform_pointer<particle_system, xray::platform_pointer_default>::type			particle_system_ptr_type;
	typedef xray::platform_pointer<burst_entry, xray::platform_pointer_default>::type				burst_entry_ptr_type;
	
private:
	char								m_material_name[128];
	
	curve_line_ranged_float				m_particle_lifetime_curve;
	curve_line_ranged_float				m_particle_spawn_rate_curve;
	
	particle_action_source_ptr_type		m_source_action;
	particle_action_data_type_ptr_type	m_data_type_action;
	
public:
	particle_action_ptr_type			m_actions;
	particle_action_ptr_type			m_last_action;
	particle_system_ptr_type			m_particle_system;
	burst_entry_ptr_type				m_burst_entries;
	particle_event_ptr_type				m_event;

	u32									m_num_actions;
	u32									m_num_burst_entries;

private:
	float								m_delay;
	float								m_duration;
	float								m_duration_variance;
	
	u32									m_max_num_particles;
	u32									m_num_loops;

	bool								m_world_space;
	bool								m_visibility;

	u16									m_event_index;
}; // class particle_emitter

} // namespace particle
} // namespace xray

#include "particle_emitter_inline.h"

#endif // #ifndef PARTICLE_EMITTER_H_INCLUDED
