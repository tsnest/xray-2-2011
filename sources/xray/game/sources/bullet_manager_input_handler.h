////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BULLET_MANAGER_INPUT_HANDLER_H_INCLUDED
#define BULLET_MANAGER_INPUT_HANDLER_H_INCLUDED

#include <xray/input/handler.h>

namespace stalker2
{
	class bullet_manager;

	class bullet_manager_input_handler: public xray::input::handler, private boost::noncopyable
	{
	public:
						bullet_manager_input_handler		( bullet_manager& bullet_manager );
		virtual			~bullet_manager_input_handler		( );

		virtual	bool	on_keyboard_action	( input::world* input_world, input::enum_keyboard key, input::enum_keyboard_action action );
		virtual	bool	on_gamepad_action	( input::world* input_world, input::gamepad_button button, input::enum_gamepad_action action );
		virtual	bool	on_mouse_key_action	( input::world* input_world, input::mouse_button button, input::enum_mouse_key_action action );
		virtual	bool	on_mouse_move		( input::world* input_world, int x, int y, int z );
		virtual int		input_priority		( ) {return 100000;}

	protected:
		bullet_manager&			m_bullet_manager;
	};//class bullet_manager_input_handler

} // namespace stalker2

#endif // #ifndef BULLET_MANAGER_INPUT_HANDLER_H_INCLUDED