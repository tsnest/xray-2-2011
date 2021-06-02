////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_POSITION_ANIMATION_CONTROLLER_H_INCLUDED
#define SINGLE_POSITION_ANIMATION_CONTROLLER_H_INCLUDED

#include "base_animation_controller.h"
#include "animation_controller_parameters.h"

namespace stalker2 {

class single_position_animation_controller : public base_animation_controller
{
public:
							single_position_animation_controller	( );
							~single_position_animation_controller	( );

	virtual	void								initialize			( );
	virtual	xray::animation::mixing::expression	finalize			( base_animation_controller& next_controller );
	virtual xray::animation::mixing::expression	selected_animations	( );
	virtual	void								set_target			( animation_controller_parameters const& target );

private:
	movement_animation_controller_parameters	m_current_parameters;
	movement_animation_controller_parameters	m_target_parameters;
}; // class single_position_animation_controller

} // namespace stalker2

#endif // #ifndef SINGLE_POSITION_ANIMATION_CONTROLLER_H_INCLUDED