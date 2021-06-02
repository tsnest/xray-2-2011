////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_WORLD_USER_BASE_H_INCLUDED
#define XRAY_WORLD_USER_BASE_H_INCLUDED

#include <xray/sound/api.h>
#include <xray/sound/sound.h>
#include <xray/sound/two_way_thread_channel.h>

namespace xray {
namespace sound {

struct sound_order;
class sound_receiver;
class sound_producer;
struct sound_response;
class sound_world;
class sound_debug_stats;

#ifndef SN_TARGET_PS3
template class XRAY_SOUND_API ::std::_STLP_PRIV_NAME::_STLP_alloc_proxy<u64*, u64, ::std::_Alloc_traits<u64, vectora_allocator< u64 > >::allocator_type >;
template class XRAY_SOUND_API ::std::_STLP_PRIV_NAME::_Vector_base< u64, vectora_allocator< typename qualified_type<u64>::result > >;
template class XRAY_SOUND_API ::std::vector< u64, vectora_allocator< typename qualified_type<u64>::result > >;
template class XRAY_SOUND_API vectora<u64>;
#endif // #ifndef SN_TARGET_PS3

class XRAY_SOUND_API world_user : private sound::noncopyable
{
public:
						world_user							( sound_world* owner, base_allocator_type* allocator );
						~world_user							( );

			void		initialize							( );
			void		finalize							( );

			void		mute_all_sounds						( bool mute );

//			void		set_listener_properties				( sound_scene_ptr& scene, float4x4 const& inv_view_matrix );

			void		set_listener_properties_interlocked	(	sound_scene_ptr& scene, 
																float3 const& position,
																float3 const& orient_front,
																float3 const& orient_top
															);

#ifndef MASTER_GOLD
			void		enable_debug_stream_writing				( sound_scene_ptr& scene );
			void		disable_debug_stream_writing			( sound_scene_ptr& scene );
			bool		is_debug_stream_writing_enabled			( sound_scene_ptr& scene ) const;
			void		dump_debug_stream_writing				( sound_scene_ptr& scene );
			void		enable_current_scene_stream_writing		( );
			void		disable_current_scene_stream_writing	( );
			void		dump_current_scene_stream_writing		( );
			bool		is_current_scene_stream_writing_enabled	( ) const;
#endif // #ifndef MASTER_GOLD

			void		set_active_sound_scene				(	sound_scene_ptr& scene,
																u32 fade_in_time,
																u32 fade_out_old_scene_time  );
			void		remove_sound_scene					( sound_scene_ptr& scene );


			void		register_receiver					( sound_scene_ptr& scene, sound_receiver& receiver );
			void		unregister_receiver					( sound_scene_ptr& scene, sound_receiver& receiver );

			// !time scale factor will be applayed for all sound world users
			void		set_time_scale_factor				( float factor );
			float		get_time_scale_factor				( ) const;

			void		dispatch_callbacks					( );

			void		process_orders						( );
			void		process_responses					( );

			void		add_order							( sound_order* order );
			void		add_response						( sound_response* response );

			// temp
			two_way_threads_channel&	get_channel			( );
			base_allocator_type*		get_allocator		( ) const;

	inline	sound_world*				get_sound_world	( ) const { return m_owner_world; }

			void		mark_producer_as_deleted			( u64 producer_address );
			void		unmark_producer_as_deleted			( u64 producer_address );
			bool		is_producer_deleted					( u64 producer_address ) const;
			void		on_producer_deleted					( u64 producer_address );

			void		mark_receiver_as_deleted			( u64 receiver_address );
			void		unmark_receiver_as_deleted			( u64 receiver_address );
			bool		is_receiver_deleted					( u64 receiver_address ) const;
			void		on_receiver_deleted					( u64 receiver_address );
private:
	two_way_threads_channel			m_channel;

	vectora<u64>*					m_deleted_producers;
	vectora<u64>*					m_deleted_receivers;

	sound_world*					m_owner_world;
	base_allocator_type*			m_allocator;

	bool							m_is_paused;
}; // class world_user

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_WORLD_USER_BASE_H_INCLUDED