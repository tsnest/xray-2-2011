////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FLYING_CAMERA_H_INCLUDED
#define FLYING_CAMERA_H_INCLUDED

#include <xray/timing_timer.h>

namespace xray {
namespace editor {

class editor_world;

class flying_camera : private boost::noncopyable {
public:
	flying_camera			( editor_world*	world );
	
	void tick				( );

	void set_destination	( float4x4 view_tranform );

private:
	editor_world*	m_world;
	bool			m_animating;
	float4x4 		m_destination;
	float3 			m_destination_vector;
	float			m_speed_starting;
	float			m_speed;
	float			m_acceleration;
	float			m_acceleration_starting;
	float			m_acceleration_increase;
	float			m_distance_factor;
	float			m_distance;
	timing::timer	m_timer;
	
}; // class flying_camera

}// namespace editor
}// namespace xray


#endif // #ifndef FLYING_CAMERA_H_INCLUDED