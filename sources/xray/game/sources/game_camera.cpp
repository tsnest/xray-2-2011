////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_camera.h"
#include "game_world.h"
#include "game.h"
#include "camera_director.h"

namespace stalker2{

game_camera::game_camera( game_world& w )
:super			( w ),
m_vertical_fov	( 57.5f ),
m_near_plane	( 0.2f ),
m_far_plane		( 5000.0f )
{
}

void game_camera::set_position_direction( math::float3 const& p, math::float3 const& d )
{
	m_inverted_view_matrix	= math::invert4x3( math::create_camera_direction( p, d, float3(0.f, 1.f, 0.f ) ) );
}

void game_camera::on_activate( camera_director* cd )
{
	m_inverted_view_matrix = cd->get_inverted_view_matrix( );
}

math::float4x4  game_camera::get_projection_matrix( ) const			
{ 
	float2 size			= m_game_world.get_game().engine().get_render_window_size();
	float const aspect	= size.x / size.y; //width / height

	return math::create_perspective_projection( m_vertical_fov*math::pi/180.0f , 
												aspect, 
												m_near_plane, 
												m_far_plane );
}

}