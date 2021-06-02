////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_RENDER_STATISTICS_H_INCLUDED
#define XRAY_RENDER_RENDER_STATISTICS_H_INCLUDED

namespace xray {
	namespace ui{
		struct world;
	}
namespace render {

class render_statistics
{
	virtual void render(xray::ui::ui_world, u32 x, u32 y) = 0;
};

} // namespace render
} // namespace xray

#endif // XRAY_RENDER_RENDER_STATISTICS_H_INCLUDED