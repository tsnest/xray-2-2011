#ifndef UI_IMAGE_H_INCLUDED
#define UI_IMAGE_H_INCLUDED

#include <xray/render/engine/base_classes.h>
#include "ui_window.h"

namespace xray {
namespace ui {

class ui_image :
	public image,
	public ui_window
{
public:
					ui_image		( memory::base_allocator& a );
	virtual			~ui_image		( );
	virtual void	init_texture	( pcstr texture_name );
	virtual void	draw			( xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view );
	virtual void	set_color		( u32 clr )						{ m_color = clr; }
	virtual window*	w				( )								{ return this; }

protected:
	float4			m_tex_coords;	// ready (uv) format, lt-rb 
	u32				m_color;
	int				m_point_type;	//temporary
}; // class ui_image

} // namespace xray
} // namespace ui

#endif //UI_IMAGE_H_INCLUDED