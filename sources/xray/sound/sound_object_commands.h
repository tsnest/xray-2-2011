////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OBJECT_COMMANDS_H_INCLUDED
#define SOUND_OBJECT_COMMANDS_H_INCLUDED

#include <xray/intrusive_spsc_queue.h>
#include <xray/intrusive_mpsc_queue.h>
#include <xray/sound/sound.h>

namespace xray {
namespace sound {

class sound_world;
struct sound_listener;
class sound_scene;

struct sound_order : public boost::noncopyable
{
			sound_order					( );
	virtual	~sound_order				( );
	
	virtual void	execute	( );

	sound_order*				m_next_for_orders;
	sound_order*				m_next_for_postponed_orders;
}; // struct sound_order

struct sound_response : public boost::noncopyable
{
			sound_response				( );
	virtual	~sound_response				( );
	
	virtual void	execute	( );

	sound_response*				m_next;
}; // struct sound_response

//struct sound_object_order : public sound_order
//{
//public:
//		sound_object_order	( sound_object_impl& owner );
//	virtual void	execute	( );
//
//private:
//	sound_object_impl&	m_owner;
//}; // struct sound_object_order
//
//class playback_command : public sound_object_order
//{
//public:
//	enum playback_event
//	{
//		ev_play,
//		ev_stop
//	};
//
//						playback_command	( playback_event const& event,
//											  sound_object_impl* const owner );
//	virtual	void			execute			( );
//
//	playback_event const	m_event;
//
//private:
//	typedef sound_object_order super;
//}; // class playback_command
//
//class sound_position_command : public sound_object_order
//{
//public:
//						sound_position_command	( float3 const& position,
//												  sound_object_impl* const owner );
//	virtual	void		execute					( );
//
//	float3				m_position;
//
//private:
//	typedef sound_object_order super;
//}; // class sound_position_command
//
//
//struct callback_command : public sound_object_order
//{
//public:
//	enum cb_event
//	{
//		ev_unknown,
//		ev_stream_end,
//		ev_buffer_end,
//		ev_buffer_error
//	};
//
//				callback_command		( cb_event const& event,
//										  sound_object_impl* const owner,
//										  void* context );
//		void	execute					( );
//
//	cb_event	m_event_type;
//	void*		m_context;
//
//private:
//	typedef sound_object_order super;
//}; // struct callback_command

class listener_properties_order : public sound_order
{
public:

						listener_properties_order	(	sound_world& world,
														sound_scene& scene,
														float4x4 const& inv_view_matrix );
	virtual	void	execute				( );

	sound_world&	m_world;
	sound_scene&	m_scene;
	float4x4		m_inv_view_matrix;

private:
	typedef sound_order super;
}; // class listener_properties_order



} // namespace sound
} // namespace xray


#endif // #ifndef SOUND_OBJECT_COMMANDS_H_INCLUDED