////////////////////////////////////////////////////////////////////////////
//	Created		: 02.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_SCENE_H_INCLUDED
#define XRAY_SOUND_SCENE_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/unmanaged_allocation_resource.h>
#include <xray/memory_single_size_buffer_allocator.h>
#include <xray/collision/object.h>
#include <xray/sound/sound_receiver.h>
#include "sound_instance_proxy_internal.h"
#include "sound_voice.h"
#include "panning_lut.h"
#ifndef MASTER_GOLD
#include "debug_snapshot.h"
#include "debug_statistic.h"
#endif // MASTER_GOLD


namespace xray {
namespace sound {

class sound_debug_stats;

namespace statistics {

struct proxy_stats
{
	float3			m_position;
	float3			m_direction;
	emitter_type	m_emitter_type; // single, composite, collection
	sound_type		m_sound_type; // point, spot, volumetric
	u32				m_id;
};

struct propagator_stats {
	fs::path_string m_filename;
	float3			m_position;
	sound_rms_ptr	m_rms;
	u32				m_owner_proxy_id;
	float			m_inner_radius;
	float			m_outer_radius;
	u32				m_before_playing_offset;
	u32				m_after_playing_offset;
	u32				m_sound_lenght;
	u32				m_playing_offset;
	bool			m_has_voice;
}; // struct propagator_stats

struct sound_scene_stats {
	buffer_vector<propagator_stats>*	m_propagators;
	u32									m_current_playing_files_count;
	u32									m_active_proxies_count;
}; // struct sound_scene_stats

} // namespace statistaics

class receiver_collision :	private sound::noncopyable
{
public:
	typedef non_null<collision::object>::ptr			collision_ptr;
public:
							receiver_collision		( sound_receiver* receiver, atomic_half3* position );
							~receiver_collision		( );

	inline	collision_ptr	get_collision_object	( ) const { R_ASSERT( m_collision ); return m_collision; }
	inline	void			set_position			( float3 const& position ) { m_position->set( position ); }
	inline	float3			get_position			( ) const { R_ASSERT ( m_position ); return m_position->get( ); }
			void			delete_position			( sound_scene& scene );
	// do not call any methods of returned sound_receiver
	inline	sound_receiver*	get_sound_receiver		( ) const { return m_receiver; }
private:
	atomic_half3*			m_position;
	sound_receiver*			m_receiver;
	collision::object*		m_collision;
public:
	receiver_collision*		m_next;
}; // struct receiver_collision

class sound_scene :	public resources::unmanaged_resource,
					private boost::noncopyable
{
public:
								sound_scene					( IXAudio2SubmixVoice* submix_voice, u32 dbg_id );
								~sound_scene				( );

	void						tick						( sound_world& world, u32 time_delta );

	// proxies
	sound_instance_proxy_ptr	create_sound_instance_proxy	
								(
									sound_emitter_ptr emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user
								);

	sound_instance_proxy_ptr	create_sound_instance_proxy	
								(
									sound_emitter_ptr emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user,
									sound_cone_type cone_type
								);

	sound_instance_proxy_ptr	create_sound_instance_proxy	
								(
									sound_emitter_ptr emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user,
									collision::geometry_instance& geometry,
									float radius
								);

		void					free_sound_instance_proxy	( sound_instance_proxy* proxy );

		// propagators
		void					emit_sound_propagators			
								(	
									sound_instance_proxy_internal& proxy,
									playback_mode mode, 
									sound_producer const* const producer,
									sound_receiver const* const ignorable_receiver
								);

		void					emit_sound_propagators			
								(	
									sound_instance_proxy_internal& proxy,
									source_params const& params,
									playback_mode mode, 
									sound_producer const* const producer,
									sound_receiver const* const ignorable_receiver
								);

	sound_propagator*			create_sound_propagator
								(	
									sound_propagator_emitter const& owner,
									sound_instance_proxy_internal& proxy,
									playback_mode mode,
									u32 playing_offset,
									u32 before_playing_offset,
									u32 after_playing_offset,
									sound_producer const* const producer,
									sound_receiver const* const ignorable_receiver = 0
								);

	sound_propagator*			create_sound_propagator_for_looped
											(	
												sound_propagator& original
											);

				void			delete_sound_propagator	
											( 
												sound_instance_proxy_internal& proxy,
												sound_propagator* propagator
											);

				void			stop_produce_sound			( sound_instance_proxy_internal& proxy ) const;
				void			stop_propagate_sound		( sound_instance_proxy_internal& proxy );
				void			pause_produce_sound			( sound_instance_proxy_internal& proxy ) const;
				void			pause_propagate_sound		( sound_instance_proxy_internal& proxy ) const;

				void			resume_produce_sound		( sound_instance_proxy_internal& proxy );
				void			resume_propagate_sound		( sound_instance_proxy_internal& proxy ) const;


				void			register_receiver			( sound_receiver* receiver, atomic_half3* position );
				void			unregister_receiver			( world_user& user, sound_receiver* receiver );

				void			set_listener_properties		( float4x4 const& inv_view_matrix );
				void			set_listener_properties		(	
																float3 const& position,
																float3 const& orient_front,
																float3 const& orient_top
															);


	inline	float3 const&		get_listenet_position		( ) const { return m_listener.m_position; }
	inline	float3 const&		get_listenet_orient_front	( ) const { return m_listener.m_orient_front; }
	inline	float3 const&		get_listenet_orient_top		( ) const { return m_listener.m_orient_top; }

	IXAudio2SubmixVoice*		get_submix_voice			( ) const { return m_submix_voice; }
				void			fade_in						( sound_world& world, u32 time_in_msec );
				void			fade_out					( u32 time_in_msec );

				void			stop						( );

				atomic_half3*	create_receiver_position	( );
				void			delete_receiver_position	( atomic_half3* pos );

				void			add_active_voice			( sound_voice& voice );
				void			remove_active_voice			( sound_voice& voice );

				void			calculate_3d_sound			( sound_voice& voice, panning_lut_ptr panning_lut );

#ifndef MASTER_GOLD
				void			update_stats				( sound_debug_stats& stats ) const;

				void			enable_debug_stream_writing			( );
				void			disable_debug_stream_writing		( );
				bool			is_debug_stream_writing_enabled		( ) const { return m_is_debug_stream_writing_enabled != 0; }
				void			dump_debug_stream_writing			( ) const;

			debug_statistic*	create_statistic					( ) const;
				void			delete_statistic					( debug_statistic* statistic ) const;
#endif //#ifndef MASTER_GOLD

				void			pause								( );
				void			resume								( );
				bool			is_paused							( ) const;

private:
	// orders
	void			emit_sound_propagators_impl
								(
									create_sound_propagator_params const& params
								);

private:
			void		init_allocators						( );
			void		on_unmanaged_resources_allocated	( resources::queries_result& queries );

			void		update_receivers_position			( );
			void		notify_receivers					( );
			void		update_listener_properties			( );
			void		notify_listener						( sound_world const& world );

			void		calculate_hdr_audio					( );
			//void		calculate_3d_sound					( sound_voice& voice );
			//void		calculate_channel_matrix			( sound_voice& voice, float* channel_matrix );

			void		process_fade						( sound_world& world, u64 time_delta );
			void		pause_propagate_all_sounds			( ) const;
			void		resume_propagate_all_sounds			( ) const;
	//		void		fill_statistic						( debug_statistic& statistic ) const;
private:
	typedef memory::single_size_buffer_allocator<	sizeof( sound_instance_proxy_internal ),
													threading::multi_threading_policy
												>	sound_proxies_allocator;

	typedef memory::single_size_buffer_allocator<	sizeof( sound_propagator ),
													threading::single_threading_policy
												>	sound_propagators_allocator;

	typedef memory::single_size_buffer_allocator<	sizeof( atomic_half3 ),
													threading::single_threading_policy
												>	receiver_position_allocator;

	typedef memory::single_size_buffer_allocator<	sizeof( receiver_collision ),
													threading::single_threading_policy
												>	receiver_collision_allocator;

#ifdef MASTER_GOLD
	typedef intrusive_list	<	sound_instance_proxy_internal,
								sound_instance_proxy_internal*,
								&sound_instance_proxy_internal::m_next_for_sound_world,
								threading::single_threading_policy 
							>	proxies_list;

	typedef intrusive_list	<	receiver_collision,
								receiver_collision*,
								&receiver_collision::m_next >	sound_receivers_list;

	typedef intrusive_list	<	sound_voice,
								sound_voice*,
								&sound_voice::m_next_for_active > active_voices;
#else // MASTER_GOLD
	typedef intrusive_list	<	sound_instance_proxy_internal,
								sound_instance_proxy_internal*,
								&sound_instance_proxy_internal::m_next_for_sound_world,
								threading::single_threading_policy,
								size_policy			
							>	proxies_list;

	typedef intrusive_list	<	receiver_collision,
								receiver_collision*,
								&receiver_collision::m_next,
								threading::single_threading_policy,
								size_policy	>	sound_receivers_list;

	typedef intrusive_list	<	sound_voice,
								sound_voice*,
								&sound_voice::m_next_for_active,
								threading::single_threading_policy,
								size_policy	>	active_voices;
#endif // MASTER_GOLD


public:
	sound_scene*		m_next;
private:
	struct listener
	{
		float3			m_orient_front; // orientation of front direction, used only for matrix and delay calculations or listeners with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
		float3			m_orient_top;   // orientation of top direction, used only for matrix and delay calculations, must be orthonormal with OrientFront when used
		float3			m_position;		// position in user-defined world units, does not affect Velocity
	}														m_listener;

	atomic_half3											m_list_position;
	atomic_half3											m_list_orient_front;
	atomic_half3											m_list_orient_top;

	resources::unmanaged_allocation_resource_ptr			m_unmanaged_resources_ptr;

	u32 const												m_proxies_count;
	uninitialized_reference< sound_proxies_allocator >		m_proxies_allocator;

	u32	const												m_propagators_count;
	uninitialized_reference< sound_propagators_allocator >	m_propagators_allocator;

	u32	const												m_receivers_count;
	uninitialized_reference< receiver_position_allocator >	m_receiver_positions_allocator;

	uninitialized_reference< receiver_collision_allocator > m_receiver_collisions_allocator;

	collision::space_partitioning_tree*						m_spatial_tree;
	sound_receivers_list									m_receivers;

	proxies_list											m_active_proxies;

	active_voices											m_active_voices;

	IXAudio2SubmixVoice*									m_submix_voice;

#ifndef MASTER_GOLD
	debug_snapshot*											m_debug_snapshot;
	threading::atomic32_type								m_is_debug_stream_writing_enabled;
#endif // #ifndef MASTER_GOLD

	float													m_fade_vol_per_msec;
	float													m_volume;
	u32														m_fade_in_time;
	u32														m_fade_out_time;
	u32														m_dbg_id;
	bool													m_is_paused;
	bool													m_is_active;
	enum
	{
		none,
		fade_in_state,
		fade_out_state
	}														m_fade_state;

}; // class sound_scene


typedef intrusive_list	<	sound_scene,
							sound_scene*,
							&sound_scene::m_next,
							threading::single_threading_policy
						>	sound_scenes_list;

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_SCENE_H_INCLUDED