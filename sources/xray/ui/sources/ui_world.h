#ifndef UI_WORLD_H_INCLUDED
#define UI_WORLD_H_INCLUDED

#include <xray/render/engine/base_classes.h>
#include <xray/ui/world.h>
#include "ui_font.h"

namespace xray {

namespace render {	
class world;

namespace ui {
	class renderer;
} // namespace ui 
} // namespace render

namespace ui {

struct engine;

class ui_world :
	public world,
	private boost::noncopyable
{
public:
							ui_world				(
								engine& engine,
								xray::render::ui::renderer& renderer,
								memory::base_allocator& allocator,
								xray::render::scene_view_ptr const& scene_view
							);
	virtual	void			tick					( );
	virtual	void			clear_resources			( );
	virtual window*			create_window			( );
	virtual dialog*			create_dialog			( );
	virtual text*			create_text				( );
	virtual text_edit*		create_text_edit		( );
	virtual image*			create_image			( );
	virtual scroll_view*	create_scroll_view		( );
	virtual progress_bar*	create_progress_bar		( );
	virtual void			destroy_window			( window* w );
	virtual	xray::ui::font const *default_font			( );
	virtual render::ui::renderer& get_renderer( )	{ return m_renderer; }
	virtual xray::render::scene_view_ptr const& get_scene_view	( ) const { return m_scene_view; }

	virtual void			on_device_reset			( );
	void					client_to_screen_scaled ( float2 const& src, float2& dst );
	void					align_pixel				( float2 const& src, float2& dst );
	float2 const&			base_screen_size		( ) const								{ return m_base_screen_size; }
	
	

	font_manager&			get_font_manager		( )										{ return m_font_manager; }
	timing::timer const&	timer					( ) const								{ return m_timer; }
	memory::base_allocator&	allocator				( ) const								{ return m_allocator; }

private:
	engine&							m_engine;
	render::ui::renderer&			m_renderer;	
	memory::base_allocator&			m_allocator;
	xray::render::scene_view_ptr	m_scene_view;
	float2							m_base_screen_size;
	font_manager					m_font_manager;
	timing::timer					m_timer;
}; // ui_class world

} // namespace xray
} // namespace ui

#endif // #ifndef UI_WORLD_H_INCLUDED
