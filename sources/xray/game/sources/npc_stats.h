////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_STATS_H_INCLUDED
#define NPC_STATS_H_INCLUDED

#include <xray/render/facade/game_renderer.h>

namespace xray {

namespace render {
namespace ui { 
	class renderer; 
} // namespace ui
} // namespace render

namespace ui {
	struct text;
	struct window;
	struct world;
} // namespace ui

} // namespace xray

namespace stalker2 {

class human_npc;

class npc_stats : private boost::noncopyable
{
public:
					npc_stats					( xray::ui::world& ui_world );
					~npc_stats					( );

	void			draw						(
						xray::render::ui::renderer& ui_renderer,
						xray::render::scene_view_ptr const& scene_view
					);
	void			set_stats					( human_npc const* const );

private:
	enum column_types_enum
	{
		column_1,
		column_2,
		column_3
	};

private:
	ui::text*		create_new_group			(
						column_types_enum column_number,
						u32 font_color,
						pcstr text,
						ui::window* upper_window = 0
					);

private:
	ui::world&		m_ui_world;
	ui::window*		m_main_window;
	
	u32 const		m_caption_color;
	u32 const		m_text_color;
	float const		m_line_height;
	float const		m_column_width;
}; // class npc_stats

} //namespace stalker2

#endif // #ifndef NPC_STATS_H_INCLUDED