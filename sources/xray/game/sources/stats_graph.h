////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STATS_GRAPH_H_INCLUDED
#define STATS_GRAPH_H_INCLUDED

#include <xray/render/world.h>

//#define FIXED_FPS	100.f

namespace stalker2{

namespace render {
namespace ui {
	struct command;
} // namespace ui
} // namespace render

class stats_graph : private boost::noncopyable {
public:
						stats_graph			(
							float time_interval,
							float invalid_value,
							float important_value0,
							float important_value1,
							u32 color = math::color_xrgb( 0, 255, 0 )
						);
						~stats_graph		( );
			void		add_value			( float time, float value );
			void		set_time_interval	( float new_time_interval );
			void		render				(
							xray::render::ui::renderer& renderer,
							xray::render::scene_view_ptr const& scene_view,
							u32 const current_frame_id,
							u32 left_margin,
							u32 top_margin,
							u32 width,
							u32 height
						);
			void		stop_rendering		( );
	inline	u32			count				( ) const { return m_count; }
			float		stats_time			( ) const;
			float		average_value		( ) const;

private:
			void		adjust_time_interval( );

private:
	struct stats_value {
		stats_value*	next;
		stats_value*	previous;
		float			time;
		float			value;
	}; // struct stats_value

private:
	typedef render::ui::command		command_type;
	enum { command_count = 5 };

private:
	stats_value*		m_current_value;
	stats_value*		m_values_pool;
#ifdef FIXED_FPS
	command_type*		m_last_commands[command_count];
	float				m_last_render_time;
#endif // #ifdef FIXED_FPS
	float				m_time_interval;
	float const			m_invalid_value;
	float const			m_important_value0;
	float const			m_important_value1;
	u32					m_count;
	u32					m_color;
}; // class stats_graph

} // namespace stalker2

#endif // #ifndef STATS_GRAPH_H_INCLUDED