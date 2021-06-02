////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OBJECT_COMMANDS_H_INCLUDED
#define SOUND_OBJECT_COMMANDS_H_INCLUDED

#include <xray/intrusive_spsc_queue.h>
#include <xray/intrusive_mpsc_queue.h>

namespace xray {
namespace sound {

class sound_world;
class sound_object_impl;

struct sound_command :public boost::noncopyable
{
				sound_command			( base_allocator_type* a):m_creator(a)	{};
	virtual		~sound_command			( )		{};
	virtual void execute				( )		{};

	sound_command* 				m_next;

	base_allocator_type*		m_creator;
}; // struct sound_command

struct sound_command_object :public sound_command
{
private:
	typedef sound_command super;
public:
				sound_command_object			( base_allocator_type* a, sound_object_impl* o ):super(a),m_owner(o){};
	virtual void execute				( )		{};

	sound_command* 				m_next;

	sound_object_impl*			m_owner;
	base_allocator_type*		m_creator;
}; // struct sound_command

class playback_command : public sound_command_object
{
private:
	typedef sound_command_object super;
public:
	enum playback_event{ev_play, ev_stop};

						playback_command	( base_allocator_type* a, playback_event const& ev, sound_object_impl* o );
	virtual	void		execute				( );

	playback_event		m_event;
}; // class playback_command

class sound_position_command : public sound_command_object
{
private:
	typedef sound_command_object super;
public:
						sound_position_command( base_allocator_type* a, float3 const& position, sound_object_impl* o );
	virtual	void		execute					( );

	float3				m_position;
}; // class playback_command


struct callback_command : public sound_command_object
{
private:
	typedef sound_command_object super;
public:

	enum cb_event{ev_unknown, ev_stream_end, ev_buffer_end, ev_buffer_error};

				callback_command		( base_allocator_type* a, cb_event const& ev, sound_object_impl* o, void* context );
		void	execute					( );

	cb_event						m_event_type;
	void*							m_context;
}; // struct callback_command

class listener_properties_command : public sound_command
{
private:
	typedef sound_command super;
public:

						listener_properties_command	( base_allocator_type* a, sound_world& sound_world, float4x4 const& inv_view_matrix );
	virtual	void		execute						( );

	sound_world&	m_sound_world;
	float4x4		m_inv_view_matrix;
}; // class playback_command

typedef intrusive_spsc_queue<sound_command, sound_command, &sound_command::m_next>	sound_commands_spsc_type;
typedef intrusive_mpsc_queue<sound_command, sound_command, &sound_command::m_next>	sound_commands_mpsc_type;

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_OBJECT_COMMANDS_H_INCLUDED