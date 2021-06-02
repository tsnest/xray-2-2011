////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "single_position_animation_controller.h"
#include <xray/animation/mixing_expression.h>

using xray::animation::mixing::expression;

namespace stalker2 {

single_position_animation_controller::single_position_animation_controller	( )
{
}

single_position_animation_controller::~single_position_animation_controller	( )
{
}

void single_position_animation_controller::initialize				( )
{
}

/*
expression single_position_animation_controller::finalize			( base_animation_controller& next_controller )
{

}

expression single_position_animation_controller::selected_animations( )
{

}
*/

void single_position_animation_controller::set_target				( animation_controller_parameters const& target )
{
	m_target_parameters												= static_cast_checked< movement_animation_controller_parameters const& >( target );
}

} // namespace stalker2