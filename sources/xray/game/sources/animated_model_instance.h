////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATED_MODEL_INSTANCE_H_INCLUDED
#define ANIMATED_MODEL_INSTANCE_H_INCLUDED

#include <xray/render/facade/model.h>
#include <xray/physics/model.h>
#include <xray/collision/animated_object.h>
#include <xray/animation/animation_player.h>

namespace stalker2 {

struct animated_model_instance : public resources::unmanaged_resource 
{
	render::animated_model_instance_ptr		m_render_model;
	physics::animated_model_instance_ptr	m_physics_model;
	collision::animated_object*				m_damage_collision;
	animation::animation_player*			m_animation_player;
}; // struct animated_model_instance

typedef	resources::resource_ptr<
	animated_model_instance,
	resources::unmanaged_intrusive_base
> animated_model_instance_ptr;

} // namespace stalker2

#endif // #ifndef ANIMATED_MODEL_INSTANCE_H_INCLUDED