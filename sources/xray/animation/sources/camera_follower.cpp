////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/camera_follower.h>
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>

namespace xray {
namespace animation {

camera_follower::camera_follower( ):
	m_follow_object			( 0 ),
	m_cam_height			( 2.f ),
	m_cam_dist				( 5.f ),
	factor					( 0.01f )
{
}
	
void camera_follower::set_object( camera_follower_interface const* const follow_object )
{
	m_follow_object			= follow_object;
}

void camera_follower::on_attach()
{
	m_focus_point_position	= m_follow_object->get_object_matrix_for_camera().c.xyz();
}

void camera_follower::move_control( float4x4 &view_inverted, input::world& input_world )
{
	if ( !m_follow_object )
		return;

	input::keyboard const* const keyboard	= &*input_world.get_keyboard();

	float delta_dist	= 0;
	float delta_height	= 0;

	if ( keyboard->is_key_down( input::key_w ) )
		delta_height	+= 1;
	
	if ( keyboard->is_key_down( input::key_s ) )
		delta_height	-= 1;

	update_position		( delta_height, delta_dist );
	process_camera		( view_inverted );
}
	
void camera_follower::update_position( float delta_height, float delta_dist )
{
	m_cam_dist			= math::clamp_r( m_cam_dist + delta_dist * factor * 3.f, 0.1f, 35.f );
	m_cam_height		= math::clamp_r( m_cam_height + delta_height * factor, -5.f, 5.f );
}

void camera_follower::process_camera( float4x4 &view_inverted )
{
	R_ASSERT						( view_inverted.valid() );
	if ( !m_follow_object )
		return;

	float4x4 const& object_transform	= m_follow_object->get_object_matrix_for_camera( );
	R_ASSERT					( object_transform.valid() );
	float3 focus_point_position	= object_transform.c.xyz();
	R_ASSERT					( focus_point_position.valid() );
	R_ASSERT					( m_focus_point_position.valid() );
	view_inverted.c.xyz()		+= (focus_point_position-m_focus_point_position);
	m_focus_point_position		= focus_point_position;
}


} // namespace animation
} // namespace xray
