////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_SOUND_INSTANCE_PROXY_H_INCLUDED
#define XRAY_SOUND_SOUND_INSTANCE_PROXY_H_INCLUDED

#include <xray/sound/api.h>
#include <xray/sound/sound.h>
#include <xray/sound/playback_mode.h>
#include <xray/sound/sound_producer.h>

namespace xray {

namespace collision {
	class geometry_instance;
} // namespace collision
namespace sound {

class sound_emitter;
class sound_propagator_emitter;
class world_user;
class sound_instance_proxy;
class sound_receiver;

template class XRAY_SOUND_API intrusive_ptr	<	sound_instance_proxy,
												sound_instance_proxy,
												threading::single_threading_policy >;

typedef intrusive_ptr<	sound_instance_proxy,
						sound_instance_proxy,
						threading::single_threading_policy > sound_instance_proxy_ptr;

#ifndef SN_TARGET_PS3
template class XRAY_SOUND_API boost::function< void ( ) >;
#endif // #ifndef SN_TARGET_PS3

struct source_params
{
	float3		m_position;
	float3		m_direction;
	float		m_cone_inner_angle;
	float		m_cone_outer_angle;
	float		m_outer_gain;
};

class XRAY_SOUND_API sound_instance_proxy : public resources::positional_unmanaged_resource,
											private noncopyable
{
public:
	typedef boost::function< void ( ) >	callback_type;


							sound_instance_proxy	( );
 	
	virtual	void			play					(	playback_mode mode = once,
														sound_producer const* const producer = 0,
														sound_receiver const* const ignorable_receiver = 0
													) = 0;

	virtual	void			pause					( ) = 0;
	virtual	void			resume					( ) = 0;
	virtual	void			stop					( ) = 0;

	// for point sound
	virtual	void			set_position				( float3 const& position ) = 0;
	// for cone sound
	virtual	void			set_position_and_direction	( float3 const& position, float3 const& direction ) = 0;
	// for volumetric sound
	virtual void			set_matrix					( float4x4 const& matrix ) = 0;

	inline	void			set_callback			( callback_type const& callback ) { m_callback = callback; }

	inline	bool			is_playing				( ) const { return m_is_playing; }
	inline	bool			is_producing_paused		( ) const { return m_is_producing_paused; }
	inline	bool			is_propagating_paused	( ) const { return m_is_propagating_paused; }

	inline	bool			is_callback_pending		( ) const { return m_callback_pending == 1; }
	inline	bool			is_callback_set			( ) const { return m_callback; }
	inline	bool			is_playing_once			( ) const { return m_is_playing_once == 1; } 

	inline	u32				get_id					( ) const { return m_id; }

	virtual	void			serialize				
							( 
							boost::function < void ( memory::writer*, memory::writer* ) >&,
							memory::writer* writer
							) = 0;

	static sound_instance_proxy_ptr deserialize		
							( 
							boost::function < void ( void ) >& fn,
							memory::reader* r,
							sound_scene_ptr& scene,
							world_user& user,
							sound_emitter const& emt,
							callback_type const& callback
							);


//	virtual	void			dbg_draw				( render::game::renderer& render, render::scene_ptr const& scene ) const = 0;
#ifndef MASTER_GOLD
	virtual void			dbg_set_quality			( u32 quality ) = 0;
#endif // #ifndef MASTER_GOLD

	virtual	void			free_object				( )	= 0;

public:
	template < typename T >
	static inline void		destroy					( T* object )
	{
		object->free_object();
	}

public:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( sound_instance_proxy )
protected:
	callback_type									m_callback;

	u32												m_id;

	friend class threading::single_threading_policy;
	threading::atomic32_type						m_reference_count;
	threading::atomic32_type						m_callback_pending;
	
	threading::atomic32_type						m_is_playing_once;

	bool											m_is_playing;
	bool											m_is_producing_paused;
	bool											m_is_propagating_paused;
}; // class sound_instance_proxy

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_SOUND_INSTANCE_PROXY_H_INCLUDED
