#include "pch.h"
#include "ui_world.h"
#include <xray/ui/api.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(ui_entry_point)

namespace xray {
namespace ui {

world* xray::ui::create_world(engine& engine, render::ui::renderer& renderer, memory::base_allocator& allocator, render::scene_view_ptr const& scene_view )
{
	return						XRAY_NEW_IMPL( allocator, ui_world )( engine, renderer, allocator, scene_view );
}

void destroy_world( world*& world )
{
	ui_world* world_inherited	= static_cast_checked<ui_world*>(world);
	XRAY_DELETE_IMPL			( world_inherited->allocator(), world_inherited );
	world						= 0;
}

} // namespace xray
} // namespace ui