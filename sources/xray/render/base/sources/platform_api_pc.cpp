////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_api.h"

namespace xray {
namespace render {

namespace dx9{
	render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle);
} // namespace dx9

namespace dx10{
	render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle);
} // namespace dx10



static xray::command_line::key		s_dx10("dx10", "", "render", "");

render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	if( !s_dx10)
		return dx9::create_render_platform( wrapper, window_handle);
	else
		return dx10::create_render_platform( wrapper, window_handle);
}

} // namespace render
} // namespace xray
