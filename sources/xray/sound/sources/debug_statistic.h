////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_STATISTIC_H_INCLUDED
#define DEBUG_STATISTIC_H_INCLUDED

#include "sound_instance_proxy_internal.h"

namespace xray {

namespace strings {
	class text_tree_item;
}

namespace sound {


struct propagator_statistic
{
							propagator_statistic( ) : next ( 0 ) { }

	void					fill_text_tree		( strings::text_tree_item* item ) const;
public:
	propagator_statistic*	next;
public:
	fs::path_string			m_filename;
	sound_rms_ptr			m_rms;
	u32						m_length;
	u32						m_current_playing_time;
	playback_mode			m_playback_mode;
};

struct proxy_statistic
{
							proxy_statistic	( ) : next ( 0 ) { }

	void					fill_text_tree	( strings::text_tree_item* item, bool draw_rpopagators_stats ) const;
public:
	typedef intrusive_list	<	propagator_statistic,
								propagator_statistic*,
								&propagator_statistic::next,
								threading::single_threading_policy,
								size_policy
							>	propagators_statistic_list;
public:
	proxy_statistic*		next;
public:
	float3						m_position;
	float3						m_direction;

	propagators_statistic_list	m_propagator_statistics;

	u32							m_id;
	sound_type					m_sound_type;
	sound_cone_type				m_cone_type;
	emitter_type				m_emitter_type;
};


struct debug_statistic
{
public:
							debug_statistic	( ) {}

	void					fill_text_tree	( strings::text_tree_item* item ) const;
public:
	typedef intrusive_list	<	proxy_statistic,
								proxy_statistic*,
								&proxy_statistic::next,
								threading::single_threading_policy,
								size_policy
							>	proxy_statistic_list;
public:
	float3					m_listener_position;
	float3					m_listener_orient_front;
	float3					m_listener_orient_top;
	

	u32						m_registered_receivers_count;
	u32						m_active_proxies_count;
	u32						m_active_voices_count;

	proxy_statistic_list	m_proxies_statistic;
}; // struc debug_statistics

} // namespace sound
} // namespace xray

#endif // #ifndef DEBUG_STATISTIC_H_INCLUDED