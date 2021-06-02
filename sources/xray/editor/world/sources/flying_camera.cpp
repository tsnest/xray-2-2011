////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "flying_camera.h"
#include "editor_world.h"

namespace xray {
namespace editor {

flying_camera::flying_camera ( editor_world*	world ):
m_world			( world ),
m_animating		( false ),
m_speed_starting( 0.f ),
m_speed			( 0.f ),
m_acceleration	( 0.002f ),
m_acceleration_starting ( 0.f ),
m_acceleration_increase ( 0.0001f ),
m_distance_factor		( 1.f/2000.f )
{
}

void flying_camera::tick()
{
	if( !m_animating )
		return;

	m_destination_vector	= math::invert4x3(m_destination).c.xyz() - m_world->get_inverted_view_matrix().c.xyz();
	float magnitude			= m_destination_vector.magnitude();

	if( math::is_zero( magnitude ) )
	{
		m_animating = false;
		return;
	}
	m_destination_vector	/= magnitude;

	float time				= m_timer.get_elapsed_sec();
	float distance			= 0.f; 

	
	distance = m_speed_starting*time + 0.5f*(m_acceleration_starting*math::sqr(time) + m_acceleration_increase*math::sqr(time)*time);

	if( m_distance/distance < 2 )
	{
		time		= 0.5f - time;
		distance	= m_distance - (m_speed_starting*time + 0.5f*(m_acceleration_starting*math::sqr(time) + m_acceleration_increase*math::sqr(time)*time));
	}
		

	if( m_distance > distance/*m_speed*/)
	{
		float4x4 tmp_mat			= m_world->get_inverted_view_matrix();
		tmp_mat.c.xyz()				+=  (distance - m_distance + magnitude)*m_destination_vector;
		m_world->set_view_matrix	( math::invert4x3( tmp_mat ) );
	}
	else
	{
		m_world->set_view_matrix	( m_destination );
		m_animating					= false;
		return;
	}
}

void flying_camera::set_destination( float4x4 view_tranform )
{
	m_destination			= view_tranform;

	m_destination_vector	= math::invert4x3(m_destination).c.xyz() - m_world->get_inverted_view_matrix().c.xyz();
	m_distance				= m_destination_vector.magnitude();


	m_acceleration_starting = m_distance * m_distance_factor;
	m_speed_starting		= 200 * m_acceleration_starting; 
	m_acceleration			= m_acceleration_starting;
	m_speed					= m_speed_starting;

	float time				= 0.5f * 0.5f;

	m_acceleration_increase = ( 2*(m_distance/2-m_speed_starting*time) - m_acceleration_starting*math::sqr(time) ) / (math::sqr(time)*time);

	m_animating				= true;

	m_timer.start			();
	tick					();
	return;
}

}// namespace editor
}// namespace xray
