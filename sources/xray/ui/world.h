////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_UI_WORLD_H_INCLUDED
#define XRAY_UI_WORLD_H_INCLUDED

#include <xray/ui/ui.h>
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace ui {

struct XRAY_NOVTABLE world {
	virtual	void			tick						( )						= 0;
	virtual	void			clear_resources				( )						= 0;
	virtual window*			create_window				( )						= 0;
	virtual dialog*			create_dialog				( )						= 0;
	virtual text*			create_text					( )						= 0;
	virtual text_edit*		create_text_edit			( )						= 0;
	virtual image*			create_image				( )						= 0;
	virtual scroll_view*	create_scroll_view			( )						= 0;
	virtual	progress_bar*	create_progress_bar			( )						= 0;
	virtual void			destroy_window				( window* w )			= 0;
	virtual	xray::ui::font const *default_font			( )						= 0;
	
	virtual render::ui::renderer& get_renderer			( )						= 0;
	virtual xray::render::scene_view_ptr const& get_scene_view( ) const			= 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace ui
} // namespace xray

#endif // #ifndef XRAY_UI_WORLD_H_INCLUDED
