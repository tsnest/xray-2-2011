////////////////////////////////////////////////////////////////////////////
//	Created		: 21.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATIONS_SELECTOR_H_INCLUDED
#define ANIMATIONS_SELECTOR_H_INCLUDED

#include "animation_controller_parameters.h"
#include "single_position_animation_controller.h"
#include "simple_animation_controller.h"

namespace xray {
namespace animation {
	class animation_player;
} // namespace animation

namespace ai {
	struct animation_item;
	struct movement_target;
} // namespace ai
} // namespace xray

namespace stalker2 {

struct base_animation_controller;

class animations_selector : private boost::noncopyable
{
public:
			animations_selector					( xray::animation::animation_player& player );

	void	set_target							( xray::ai::animation_item const& animation_emitter );
	void	set_target							( xray::ai::movement_target const& target_position );

private:
	single_position_animation_controller		m_single_position_animation_controller;
	simple_animation_controller					m_simple_animation_controller;
	
	simple_animation_controller_parameters		m_simple_animation_parameters;
	movement_animation_controller_parameters	m_movement_animation_parameters;
	
	xray::animation::animation_player&			m_animation_player;
	base_animation_controller*					m_current_controller;
	base_animation_controller*					m_target_controller;
	animation_controller_parameters*			m_target_controller_parameters;
}; // class animations_selector

} // namespace stalker2

#endif // #ifndef ANIMATIONS_SELECTOR_H_INCLUDED