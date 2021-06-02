////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "simple_animation_controller.h"
#include <xray/animation/mixing_expression.h>

using xray::animation::mixing::expression;

namespace stalker2 {

simple_animation_controller::simple_animation_controller	( )
{
}

simple_animation_controller::~simple_animation_controller	( )
{
}

void simple_animation_controller::initialize				( )
{
}

void simple_animation_controller::set_target				( animation_controller_parameters const& target )
{
	m_target_parameters										= static_cast_checked< simple_animation_controller_parameters const& >( target );
}

/*
expression simple_animation_controller::finalize			( base_animation_controller& next_controller )
{

}

expression simple_animation_controller::selected_animations( )
{

}
*/

} // namespace stalker2