////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

namespace xray {

namespace render{ 
namespace ui{ 
	class renderer; 
} // namespace ui
} // namespace render

namespace ui{
	struct text;
	struct window;
	struct world;
} // namespace ui

} // namespace xray

namespace stalker2{

class stats : private boost::noncopyable
{
public:
					stats						( xray::ui::world& ui_world );
					~stats						( );

	void			draw						( xray::render::ui::renderer& w );
	void			set_fps_stats				( float fps );
	void			set_camera_stats			( float3 const& pos, float3 const& dir );
	void			set_resources_stats			( pcstr str );
	void			set_rtp_controllers_dump	( pcstr str );
	void			set_navmesh_info			( pcstr str );

protected:
	void			create						( );
	
protected:	
	ui::world&		m_ui_world;
	ui::window*		m_main_window;
	ui::text*		m_fps;
	ui::text*		m_camera_position;
	ui::text*		m_camera_direction;
	ui::text*		m_resources_activity;
	ui::text*		m_rtp_controllers_dump;
	ui::text*		m_navmesh_info;

}; //class stats

} //namespace stalker2

#endif //STATS_H_INCLUDED