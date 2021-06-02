////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_track.h"
#include "free_fly_camera.h"
#include <xray/animation/anim_track.h>

namespace stalker2{

object_track::object_track( game_world& w )
:super( w )
{
	m_track = NEW(xray::animation::anm_track)(g_allocator, 10);
	m_track->initialize_empty(true, true, true);
}

object_track::~object_track( )
{
	DELETE	( m_track );
}

void object_track::load( configs::binary_config_value const& t )
{
	super::load		( t );
	m_track->load	( t["path"] );
}

bool object_track::evaluate( float const time, math::float4x4& m )
{
	m_track->evaluate( m, time );
	m = m * m_transform;
	return true;
}

float object_track::length( ) const
{
	float mn = m_track->min_time();
	float mx = m_track->max_time();
	return mx-mn;
}

void object_track::load_contents( )
{
	//cut_scene_camera* cs			= NEW(cut_scene_camera)( m_game.get_game_world() );
	//cs->attach_track				( this );
	//cs->play						( );
}

void object_track::unload_contents( )
{}


} //namespace stalker2