////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_INSTANCE_PROXY_INTERNAL_H_INCLUDED
#define SOUND_INSTANCE_PROXY_INTERNAL_H_INCLUDED

#include <xray/sound/sound_instance_proxy.h>
#include <xray/sound/sound_object_commands.h>
#include <xray/sound/sound_producer.h>
#include <xray/sound/sound_emitter.h>
#include <xray/sound/atomic_half3.h>
#include "sound_propagator.h"

namespace xray {

namespace collision {
	struct space_partitioning_tree;
	class object;
	typedef vectora< object const* >		objects_type;
	class geometry_instance;
} // namespace collision 

namespace sound {

class sound_scene;
struct proxy_statistic;

enum sound_type
{
	point,
	cone,
	volumetric
};

class sound_instance_proxy_internal : public sound_instance_proxy
{
public:
	typedef intrusive_list	<	sound_order,
								sound_order*,
								&sound_order::m_next_for_postponed_orders,
								threading::single_threading_policy 
							>	sound_orders_type;

public:
							sound_instance_proxy_internal	( 
									sound_scene& scene,
									sound_emitter_ptr sound_emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user 
									);

							sound_instance_proxy_internal	( 
									sound_scene& scene,
									sound_emitter_ptr sound_emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user,
									sound_cone_type cone_type
									);

							sound_instance_proxy_internal	( 
									sound_scene& scene,
									sound_emitter_ptr sound_emitter,
									sound_propagator_emitter const& propagator_emitter,
									world_user& user,
									collision::geometry_instance& geometry,
									float radius
									);

							~sound_instance_proxy_internal	( );

	virtual	void			play					(	playback_mode mode = once,
														sound_producer const* const producer = 0,
														sound_receiver const* const ignorable_receiver = 0
													);

	virtual	void			play_once				(	float3 const& position,
														sound_producer const* const producer = 0,
														sound_receiver const* const ignorable_receiver = 0
													);

	virtual	void			pause					( );
	virtual	void			resume					( );
	virtual	void			stop					( );


	virtual	void			pause_propagate_sound	( );
	virtual	void			resume_propagate_sound	( );

	virtual	void			stop_propagate_sound	( );

	virtual	void			set_position				( float3 const& position );
	inline	float3			get_position				( ) const { R_ASSERT( m_type != volumetric ); return m_position.get( ); }
	
	virtual	void			set_position_and_direction	( float3 const& position, float3 const& direction );
	inline	float3			get_direction				( ) const { R_ASSERT ( m_type == cone ); return m_direction.get(); }
	inline	sound_cone_type	get_cone_type				( ) const { R_ASSERT ( m_type == cone ); return m_cone_type; }

	virtual void			set_matrix					( float4x4 const& matrix );
			float3			get_volumetric_position		( float3 const& listener_position ) const;

			void			tick					( u32 delta_time );
			void			notify_receivers		( collision::space_partitioning_tree& spatial_tree );

			void			add_sound_propagator	( sound_propagator& propagator ) { m_propagators.push_back( &propagator ); }

	inline	void			set_callback_pending	( bool is_pending ) { threading::interlocked_exchange( m_callback_pending, is_pending ); }
			void			execute_callback		( );
	inline	bool			is_destruction_pending	( ) const { return m_destruction_pending == 1; }

	inline	propagators_list&		get_propagators				( ) { return m_propagators; }
	inline	propagators_list const&	get_propagators				( ) const { return m_propagators; }
	inline	world_user&				get_world_user				( ) const { return m_user; }
	inline	sound_scene&			get_sound_scene				( ) const { return m_scene; }

	inline	sound_propagator_emitter const& get_sound_propagator_emitter( ) const { return m_propagator_emitter; }
	inline	float					get_volumetric_radius		( ) const { return m_volumetric_radius; }
	inline	sound_type				get_sound_type				( ) const { return m_type; }
	
	virtual	void			serialize						( boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer* writer );
	
	virtual	void			deserialize						( 
																boost::function < void ( void ) >& fn,
																memory::reader* r,
																callback_type const& callback
															);

	
			void			on_propagators_serialized		( boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer* sound_thread_writer, memory::writer* w );
	//virtual	void			dbg_draw					( render::game::renderer& render, render::scene_ptr const& scene ) const;

			void			on_finish_callback				( sound_instance_proxy_ptr last_reference );

sound_emitter_ptr const&	get_sound_emitter				( ) const { return m_sound_emitter; }

#ifndef MASTER_GOLD
	virtual void			dbg_set_quality					( u32 quality );
			void			dbg_set_quality_impl			( u32 quality );
			void			dump_debug_snapshot				( configs::lua_config_value& val ) const;
			void			fill_statistic					( proxy_statistic& statistic ) const;
#endif // #ifndef MASTER_GOLD
public:
	sound_instance_proxy_internal*		m_next_for_sound_world;
private:
	virtual	void			free_object						( );
	
			void			set_quality_for_resource		( float3 const& position );

			void			get_receivers_in_radius			( 
												collision::space_partitioning_tree& spatial_tree,
												float3 const& position,
												float radius,
												collision::objects_type& query_result
											) const;
private:
	atomic_half3									m_position;
	atomic_half3									m_direction;

	propagators_list								m_propagators;
	world_user&										m_user;
	sound_scene&									m_scene;
	sound_emitter_ptr								m_sound_emitter;
	sound_propagator_emitter const&					m_propagator_emitter;

	threading::atomic32_type						m_destruction_pending;
	collision::geometry_instance*					m_collision;


	sound_type										m_type;
	sound_cone_type									m_cone_type;
	float											m_volumetric_radius;
}; // class sound_instance_proxy_internal

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_INSTANCE_PROXY_INTERNAL_H_INCLUDED