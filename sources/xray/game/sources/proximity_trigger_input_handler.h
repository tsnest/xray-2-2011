////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROXIMITY_TRIGGER_INPUT_HANDLER_H_INCLUDED
#define PROXIMITY_TRIGGER_INPUT_HANDLER_H_INCLUDED

#ifndef MASTER_GOLD

#include <xray/input/handler.h>

namespace stalker2
{
	class proximity_trigger_behaviour;

	class proximity_trigger_input_handler: public xray::input::handler, private boost::noncopyable
	{
	public:
						proximity_trigger_input_handler		( proximity_trigger_behaviour& proximity_trigger_behaviour );
		virtual			~proximity_trigger_input_handler	( );

		void			add_handler			( );
		void			remove_handler		( );

		virtual	bool	on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
		virtual	bool	on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
		virtual	bool	on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
		virtual	bool	on_mouse_move		( input::world* input_world, int x, int y, int z );
		virtual int		input_priority		( ) {return 100000;}

	protected:
		proximity_trigger_behaviour&		m_proximity_trigger_behaviour;
		bool								m_is_control_pressed;
		bool								m_is_attached;

	}; // class proximity_trigger_input_handler

} // namespace stalker2

#endif //#ifndef MASTER_GOLD

#endif // #ifndef PROXIMITY_TRIGGER_INPUT_HANDLER_H_INCLUDED