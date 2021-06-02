////////////////////////////////////////////////////////////////////////////
//	Created		: 03.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_api.h"

namespace xray {
namespace render {

namespace ps3{
	render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle);
} // namespace ps3


render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	return ps3::create_render_platform( wrapper, window_handle);
}


} // namespace render
} // namespace xray

