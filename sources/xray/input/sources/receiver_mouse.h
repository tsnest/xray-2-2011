////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RECEIVER_MOUSE_H_INCLUDED
#define RECEIVER_MOUSE_H_INCLUDED

#include <xray/input/world.h>
#include <xray/input/mouse.h>

struct IDirectInputDevice8A;
struct IDirectInput8A;

namespace xray {
namespace input {

struct handler;

namespace receiver {

class mouse :
	public input::mouse,
	private boost::noncopyable
{
typedef vector< handler* >		handlers_type;
public:
							mouse			( IDirectInput8A& direct_input, HWND window_handle, xray::input::world& input_world );
	virtual					~mouse			( );
	virtual	state const&	get_state		( ) const;
			void			on_activate		( );
			void			on_deactivate	( );
			void			execute			( );
			void			process			( handlers_type& handlers );
private:
	state					m_current_state;
	state					m_previous_state;
	HWND					m_window_handle;
	IDirectInputDevice8A*	m_device;
	xray::input::world&		m_world;
}; // class world

} // namespace receiver
} // namespace input
} // namespace xray

#endif // #ifndef RECEIVER_MOUSE_H_INCLUDED