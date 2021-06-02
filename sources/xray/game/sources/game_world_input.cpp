////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game.h"
#include "free_fly_camera.h"
#include "game_world.h"

#include <xray/rtp/world.h>
#include <xray/animation/world.h>

namespace stalker2{

class camera_director;
bool b_rtp_dbg_input = false;

class rtp_debug_camera : public free_fly_camera
{
	typedef free_fly_camera super;
public:
						rtp_debug_camera		( game_world& w );
	virtual void		tick					( );
	virtual void		on_activate				( camera_director* cd )	{ super::on_activate(cd); }
	virtual void		on_deactivate			( )							{ super::on_deactivate(); }
			void		start					( );
			void		stop					( );
private:
	game_camera*	m_prev_camera;
};

rtp_debug_camera::rtp_debug_camera( game_world& w )
:super			( w ),
m_prev_camera	( NULL )
{};

void rtp_debug_camera::start( )
{
	camera_director_ptr cd = m_game_world.get_camera_director();
	m_prev_camera		= cd->get_active_camera();
	cd->switch_to_camera	( this, "[RTP debug]" );
}

void rtp_debug_camera::stop( )
{
	camera_director_ptr cd = m_game_world.get_camera_director();
	cd->switch_to_camera	( m_prev_camera, "[prev]" );
}

void rtp_debug_camera::tick( )
{
	super::tick								( );
	game& g = m_game_world.get_game();
	g.rtp().dbg_move_control			( m_inverted_view_matrix, g.input_world() );
//	g.animation_world().debug_control	( g.input_world()  );
}

rtp_debug_camera* g_rtp_debug_camera = NULL;

void game_world::turn_rtp_debug( pcstr )
{
	//if(b_rtp_dbg_input)
	//{
	//	if(!g_rtp_debug_camera)
	//		g_rtp_debug_camera		= NEW(rtp_debug_camera)(*this);
	//	g_rtp_debug_camera->start	( );
	//}else
	//{
	//	if(g_rtp_debug_camera)
	//	{
	//		g_rtp_debug_camera->stop( );
	//		DELETE					( g_rtp_debug_camera );
	//		g_rtp_debug_camera		= NULL;
	//	}
	//}
}

} // namespace stalker2