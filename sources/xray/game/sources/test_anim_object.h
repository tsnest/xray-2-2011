////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_ANIM_OBJECT_H_INCLUDED
#define TEST_ANIM_OBJECT_H_INCLUDED

#include <xray/animation/type_definitions.h>
#include "weapon.h"

namespace xray {
namespace animation {

class animation_player;

typedef	resources::resource_ptr <
	animation_player,
	resources::unmanaged_intrusive_base
> animation_player_ptr;

} // namespace animation

namespace render {

class skeleton_model_instance;

typedef	resources::resource_ptr<
	skeleton_model_instance,
	resources::unmanaged_intrusive_base >
skeleton_model_ptr;

} // namespace render
} // namespace xray

namespace stalker2{

class game_world;

struct test_anim_object : private boost::noncopyable
{
						test_anim_object				( game_world& w);
	void				on_resources_loaded				( resources::queries_result& data );
	void				on_weapon_loaded				( resources::queries_result& data );
	void				tick							( );

	render::skeleton_model_ptr			m_test_model;
	animation::animation_player*		m_test_animation_player;
	animation::skeleton_animation_ptr	m_test_base;
	animation::skeleton_animation_ptr	m_test_add;
	timing::timer						m_test_timer;
	game_world&							m_game_world;

protected:
	void				load							( );

	weapon_ptr		m_weapon;
};

} // namespace stalker2

#endif // #ifndef TEST_ANIM_OBJECT_H_INCLUDED