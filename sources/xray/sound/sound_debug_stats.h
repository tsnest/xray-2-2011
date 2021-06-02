////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_DEBUG_STATS_H_INCLUDED
#define XRAY_SOUND_DEBUG_STATS_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/render/facade/game_renderer.h>

namespace xray {

namespace render {
namespace ui { 
	class renderer; 
} // namespace ui
} // namespace render

namespace ui {
	struct window;
	struct world;
	struct progress_bar;
} // namespace ui

namespace strings {
class text_tree_item;
} // namespace strings

namespace sound {

class world_user;
class sound_scene;
struct debug_statistic;

namespace statistics {
	struct sound_scene_stats;
	struct propagator_stats;
	struct proxy_stats;
} // namespace statistics

class XRAY_SOUND_API sound_debug_stats : private sound::noncopyable
{
public:
					sound_debug_stats			( memory::base_allocator* allocator, world_user& user, sound_scene_ptr scene, ui::world& ui_world );
					~sound_debug_stats			( );

			void	draw						( render::scene_ptr scene );
	inline	bool	is_stats_available			( ) const { return m_actual_statistic != -1; }
			void	clear_resources				( world_user& user );
public:
	enum mode
	{
		none = 0,
		overall,
		detail,
		hdr,
		mode_count
	};
	static	void	set_debug_draw_mode			( mode debug_draw_mode );
	static	void	set_detail_view_proxy_id	( u32 proxy_id );
private:
			void	draw_overall_stats			( render::scene_ptr scene );
			void	draw_hdr_stats				( render::scene_ptr scene );
			void	draw_detail_stats			( render::scene_ptr scene, u32 proxy_id );

			void	create_statistic			( );
			void	update_statistic			( );
			void	on_statistic_updated		( );
			void	update_window				( strings::text_tree_item* item );
private:
	debug_statistic*				m_statistic[2];
	ui::progress_bar**				m_progress_bars;
	ui::world&						m_ui_world;
	ui::window*						m_main_window;
	memory::base_allocator&			m_allocator;
	threading::atomic32_type		m_actual_statistic;
	world_user&						m_world_user;
	sound_scene*					m_scene;

	static threading::atomic32_type	m_s_debug_draw_mode;
	static threading::atomic32_type	m_s_proxy_id;

	friend class sound_scene;
}; // class sound_debug_stats

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_DEBUG_STATS_H_INCLUDED