////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_CAMERA_H_INCLUDED
#define GAME_CAMERA_H_INCLUDED

#include "object.h"
#include "object_behaviour.h"
#include "camera_director.h"

namespace stalker2 {

enum e_type_camera{
	type_camera_free_fly,
	type_camera_terrain_along_fly,
};

class camera_director;

class game_camera : public game_object_, public object_controlled
{
	typedef game_object_ super;
public:
								game_camera					( game_world& w );
	math::float4x4				get_projection_matrix		( ) const;
	math::float4x4 const&		get_inverted_view_matrix	( ) const			{ return m_inverted_view_matrix; }
	math::float4x4&				inverted_view_matrix		( )					{ return m_inverted_view_matrix; }
	void						set_position_direction		( math::float3 const& p, math::float3 const& d );
	virtual void				on_activate					( camera_director* cd );
	virtual void				on_deactivate				( )					{ };
	virtual void				on_focus					( bool /*b_focus_enter*/) {};
	virtual void				tick						( ) {};// for non-logic cameras(rtp)

protected:
	float4x4	m_inverted_view_matrix;
	float		m_vertical_fov;
	float		m_near_plane;
	float		m_far_plane;
}; //class game_camera

} // namespace stalker2

#endif // #ifndef GAME_CAMERA_H_INCLUDED